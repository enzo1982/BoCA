 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponentstdio.h>
#include <boca/common/utilities.h>

#include <smooth/io/drivers/driver_ansi.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM	       0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003

using namespace smooth::IO;

BoCA::AS::EncoderComponentExternalStdIO::EncoderComponentExternalStdIO(ComponentSpecs *specs) : EncoderComponentExternal(specs)
{
	out	     = NIL;

	driver_stdin = NIL;

	wPipe	     = NIL;
}

BoCA::AS::EncoderComponentExternalStdIO::~EncoderComponentExternalStdIO()
{
}

Bool BoCA::AS::EncoderComponentExternalStdIO::Activate()
{
	if (!EncoderComponentExternal::Activate()) return False;

	/* Build output file name.
	 */
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outputFile).Append(".").Append(GetOutputFileExtension());

	/* Remove temporary file if it exists.
	 * Might be a leftover of a previous encoding attempt.
	 */
	File(encFileName).Delete();

	/* Start 3rd party command line encoder.
	 */
	const Info	&info = track.GetInfo();

	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString(GetConfiguration()))
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

	wPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "w");

	driver_stdin = new DriverANSI(wPipe);
	out = new OutStream(STREAM_DRIVER, driver_stdin);

	/* Output WAVE header.
	 */
	out->OutputString("RIFF");
	out->OutputNumber(36, 4);
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
	out->OutputNumber(0, 4);

	out->Flush();

	return True;
}

Bool BoCA::AS::EncoderComponentExternalStdIO::Deactivate()
{
	EncoderComponentExternal::Deactivate();

	/* Close stdio pipe.
	 */
	delete out;
	delete driver_stdin;

	/* Wait until the encoder exits.
	 */
	unsigned long	 exitStatus = pclose(wPipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		/* Remove output file.
		 */
		File(encFileName).Delete();

		errorState  = True;
		errorString = String("Encoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute \"").Append(String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter())).Append("\".");
		else if (exitCode == 127) errorString = String("External encoder \"").Append(String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter())).Append("\" not found.");

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
	Buffer<UnsignedByte>	 buffer(128 * 1024);
	Int64			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		Int	 bytes = Math::Min(buffer.Size(), bytesLeft);

		in.InputData(buffer, bytes);

		driver->WriteData(buffer, bytes);

		bytesLeft -= bytes;
	}

	/* Append tags.
	 */
	driver->WriteData(tagBufferAppend, tagBufferAppend.Size());

	in.Close();

	File(encFileName).Delete();

	/* Look for a companion file.
	 */
	File	 companionFile = GetCompanionFile(encFileName);

	if (companionFile.Exists()) companionFile.Move(GetCompanionFile(track.outputFile));

	return True;
}

Int BoCA::AS::EncoderComponentExternalStdIO::WriteData(const Buffer<UnsignedByte> &data)
{
	Buffer<UnsignedByte>	&buffer = const_cast<Buffer<UnsignedByte> &>(data);

	ProcessData(buffer);

	/* Convert to little-endian byte order.
	 */
	static Endianness	 endianness = CPU().GetEndianness();

	if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(buffer, format.bits / 8);

	/* Convert 8 bit samples to unsigned.
	 */
	if (format.bits == 8 && format.sign == True)
	{
		for (Int i = 0; i < buffer.Size(); i++) buffer[i] = SignedByte(buffer[i]) + 128;
	}

	/* Hand data over to the encoder using the stdio pipe.
	 */
	out->OutputData(buffer, buffer.Size());
	out->Flush();

	return buffer.Size();
}
