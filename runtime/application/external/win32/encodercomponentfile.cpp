 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include <windows.h>
#include <mmreg.h>

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

	wavFileName = Utilities::GetNonUnicodeTempFileName(track.outputFile).Append(".wav");
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outputFile).Append(".").Append(GetOutputFileExtension());

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
							      .Replace("%INFILE", String("\"").Append(wavFileName).Append("\""))
							      .Replace("%OUTFILE", String("\"").Append(encFileName).Append("\""))
							      .Replace("%ARTIST", String("\"").Append((char *) info.artist).Append("\""))
							      .Replace("%ALBUM", String("\"").Append((char *) info.album).Append("\""))
							      .Replace("%TITLE", String("\"").Append((char *) info.title).Append("\""))
							      .Replace("%TRACK", String("\"").Append(String::FromInt(info.track)).Append("\""))
							      .Replace("%YEAR", String("\"").Append(String::FromInt(info.year)).Append("\""))
							      .Replace("%GENRE", String("\"").Append((char *) info.genre).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
	startupInfo.hStdError	= GetStdHandle(STD_ERROR_HANDLE);

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		errorState  = True;
		errorString = String("Unable to run encoder ").Append(command).Append(".");

		return False;
	}

	/* Wait until the encoder exits.
	 */
	while (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	if (specs->debug)
	{
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

	File(wavFileName).Delete();

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove output file.
		 */
		File(encFileName).Delete();

		errorState  = True;
		errorString = String("Encoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

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

Int BoCA::AS::EncoderComponentExternalFile::WriteData(const Buffer<UnsignedByte> &data)
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

	/* Hand data over to the output file.
	 */
	nOfSamples += (buffer.Size() / (format.bits / 8));

	out->OutputData(buffer, buffer.Size());

	return buffer.Size();
}
