 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponentfile.h>
#include <boca/common/utilities.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM 0x0001

using namespace smooth::IO;

BoCA::AS::EncoderComponentExternalFile::EncoderComponentExternalFile(ComponentSpecs *specs) : EncoderComponentExternal(specs)
{
	out	   = NIL;

	nOfSamples = 0;
}

BoCA::AS::EncoderComponentExternalFile::~EncoderComponentExternalFile()
{
}

Bool BoCA::AS::EncoderComponentExternalFile::Activate()
{
	/* Create temporary WAVE file
	 */
	nOfSamples = 0;

	wavFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".wav");
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".").Append(GetOutputFileExtension());

	out = new OutStream(STREAM_FILE, wavFileName, OS_REPLACE);

	/* Write WAVE header
	 */
	out->OutputString("RIFF");
	out->OutputNumber(track.length * format.channels * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");
	out->OutputString("fmt ");

	out->OutputNumber(16, 4);
	out->OutputNumber(WAVE_FORMAT_PCM, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);

	out->OutputNumber(format.bits, 2);
	out->OutputString("data");
	out->OutputNumber(track.length * format.channels * (format.bits / 8), 4);

	return True;
}

Bool BoCA::AS::EncoderComponentExternalFile::Deactivate()
{
	/* Finalize and close the uncompressed temporary file
	 */
	Int	 size = nOfSamples * (format.bits / 8) + 36;

	out->Seek(4);
	out->OutputNumber(size, 4);

	size -= 36;

	out->Seek(40);
	out->OutputNumber(size, 4);

	delete out;

	/* Start 3rd party command line encoder
	 */
	const Info	&info = track.GetInfo();

	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String(wavFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												     .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												     .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												     .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"))
							      .Replace("%OUTFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												      .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												      .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												      .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"))
							      .Replace("%ARTIST", String("\"").Append((char *) info.artist).Append("\""))
							      .Replace("%ALBUM", String("\"").Append((char *) info.album).Append("\""))
							      .Replace("%TITLE", String("\"").Append((char *) info.title).Append("\""))
							      .Replace("%TRACK", String("\"").Append(String::FromInt(info.track)).Append("\""))
							      .Replace("%YEAR", String("\"").Append(String::FromInt(info.year)).Append("\""))
							      .Replace("%GENRE", String("\"").Append((char *) info.genre).Append("\""));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "r");

	/* Wait until the encoder exits
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	File(wavFileName).Delete();

	/* Check if anything went wrong
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		/* Remove output file
		 */
		File(encFileName).Delete();

		errorState  = True;
		errorString = String("Encoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute ").Append(command).Append(".");
		else if (exitCode == 127) errorString = String("External encoder ").Append(command).Append(" not found.");

		return False;
	}

	/* Create tag buffer
	 */
	Buffer<UnsignedByte>	 tagBuffer;
	Int			 tagMode = RenderTag(encFileName, track, tagBuffer);

	/* Prepend tag
	 */
	if (tagMode == TAG_MODE_PREPEND)
	{
		driver->WriteData(tagBuffer, tagBuffer.Size());
	}

	/* Stream contents of created file to output driver
	 */
	InStream		 in(STREAM_FILE, encFileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(1024);
	Int			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		in.InputData(buffer, Math::Min(1024, bytesLeft));

		driver->WriteData(buffer, Math::Min(1024, bytesLeft));

		bytesLeft -= Math::Min(1024, bytesLeft);
	}

	/* Append tag
	 */
	if (tagMode == TAG_MODE_APPEND)
	{
		driver->WriteData(tagBuffer, tagBuffer.Size());
	}

	in.Close();

	File(encFileName).Delete();

	return True;
}

Int BoCA::AS::EncoderComponentExternalFile::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, track.GetFormat().bits / 8);

	/* Hand data over to the output file
	 */
	nOfSamples += (size / (format.bits / 8));

	out->OutputData(data, size);

	return size;
}
