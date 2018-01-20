 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponentfile.h>
#include <boca/common/utilities.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM	       0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003

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
	if (!EncoderComponentExternal::Activate()) return False;

	/* Create temporary WAVE file.
	 */
	nOfSamples = 0;

	wavFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".wav");
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".").Append(GetOutputFileExtension());

	out = new OutStream(STREAM_FILE, wavFileName, OS_REPLACE);

	/* Write WAVE header.
	 */
	out->OutputString("RIFF");
	out->OutputNumber(track.length * format.channels * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");
	out->OutputString("fmt ");

	out->OutputNumber(16, 4);
	out->OutputNumber(format.fp ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM, 2);
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
	EncoderComponentExternal::Deactivate();

	/* Finalize and close the uncompressed temporary file.
	 */
	Int	 size = nOfSamples * (format.bits / 8);

	out->Seek(4);
	out->OutputNumber(size + 36, 4);

	out->Seek(40);
	out->OutputNumber(size, 4);

	delete out;

	/* Start 3rd party command line encoder.
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

	/* Wait until the encoder exits.
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	File(wavFileName).Delete();

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		/* Remove output file.
		 */
		File(encFileName).Delete();

		errorState  = True;
		errorString = String("Encoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute ").Append(command).Append(".");
		else if (exitCode == 127) errorString = String("External encoder ").Append(command).Append(" not found.");

		return False;
	}

	/* Create tag buffers.
	 */
	Buffer<UnsignedByte>	 tagBufferPrepend;
	Buffer<UnsignedByte>	 tagBufferAppend;

	RenderTags(encFileName, track, tagBufferPrepend, tagBufferAppend);

	/* Prepend tags.
	 */
	driver->WriteData(tagBufferPrepend, tagBufferPrepend.Size());

	/* Stream contents of created file to output driver.
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

	/* Append tags.
	 */
	driver->WriteData(tagBufferAppend, tagBufferAppend.Size());

	in.Close();

	File(encFileName).Delete();

	return True;
}

Int BoCA::AS::EncoderComponentExternalFile::WriteData(Buffer<UnsignedByte> &data)
{
	ProcessData(data);

	/* Convert to little-endian byte order.
	 */
	static Endianness	 endianness = CPU().GetEndianness();

	if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, format.bits / 8);

	/* Convert 8 bit samples to unsigned.
	 */
	if (format.bits == 8 && format.sign == True)
	{
		for (Int i = 0; i < data.Size(); i++) data[i] = SignedByte(data[i]) + 128;
	}

	/* Hand data over to the output file.
	 */
	nOfSamples += (data.Size() / (format.bits / 8));

	out->OutputData(data, data.Size());

	return data.Size();
}
