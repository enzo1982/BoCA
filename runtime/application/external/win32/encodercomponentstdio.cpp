 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

#include <smooth/io/drivers/driver_win32.h>

#include <windows.h>
#include <mmreg.h>

using namespace smooth::IO;

BoCA::AS::EncoderComponentExternalStdIO::EncoderComponentExternalStdIO(ComponentSpecs *specs) : EncoderComponentExternal(specs)
{
	out	     = NIL;

	driver_stdin = NIL;

	rPipe	     = NIL;
	wPipe	     = NIL;

	hProcess     = NIL;
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

	/* Set up security attributes.
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 32768 * format.channels * (format.bits / 8));
	SetHandleInformation(wPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder.
	 */
	const Info	&info = track.GetInfo();

	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
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
	startupInfo.hStdInput	= rPipe;
	startupInfo.hStdOutput	= GetStdHandle(STD_OUTPUT_HANDLE);
	startupInfo.hStdError	= GetStdHandle(STD_ERROR_HANDLE);

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	hProcess = processInfo.hProcess;

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		errorState  = True;
		errorString = String("Unable to run encoder ").Append(command).Append(".");

		return False;
	}

	/* Open pipe handle.
	 */
	driver_stdin = new DriverWin32(wPipe);
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

	/* Sleep for 1/8th second to avoid locking up in WriteData
	 * when the encoder exits because of invalid parameters or
	 * audio format as process exit is detected with some delay.
	 */
	S::System::System::Sleep(125);

	return True;
}

Bool BoCA::AS::EncoderComponentExternalStdIO::Deactivate()
{
	EncoderComponentExternal::Deactivate();

	/* Close stdio pipe.
	 */
	delete out;
	delete driver_stdin;

	CloseHandle(rPipe);
	CloseHandle(wPipe);

	/* Wait until the encoder exits.
	 */
	while (WaitForSingleObject(hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	if (specs->debug)
	{
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(hProcess, &exitCode);

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

	/* Check if external encoder still exists.
	 */
	if (WaitForSingleObject(hProcess, 0) != WAIT_TIMEOUT)
	{
		errorState  = True;
		errorString = "Encoder quit prematurely.";

		return -1;
	}

	/* Hand data over to the encoder using the stdio pipe.
	 */
	out->OutputData(buffer, buffer.Size());
	out->Flush();

	return buffer.Size();
}
