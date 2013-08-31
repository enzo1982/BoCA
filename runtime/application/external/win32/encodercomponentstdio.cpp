 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".").Append(GetOutputFileExtension());

	/* Remove temporary file if it exists.
	 * Might be a leftover of a previous encoding attempt.
	 */
	File(encFileName).Delete();

	/* Set up security attributes
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(wPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder
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

	driver_stdin = new DriverWin32(wPipe);
	out = new OutStream(STREAM_DRIVER, driver_stdin);

	/* Output WAVE header
	 */
	out->OutputString("RIFF");
	out->OutputNumber(36, 4);
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
	out->OutputNumber(0, 4);

	out->Flush();

	return True;
}

Bool BoCA::AS::EncoderComponentExternalStdIO::Deactivate()
{
	/* Close stdio pipe
	 */
	delete out;
	delete driver_stdin;

	CloseHandle(rPipe);
	CloseHandle(wPipe);

	/* Wait until the encoder exits
	 */
	unsigned long	 exitCode = 0;

	while (True)
	{
		GetExitCodeProcess(hProcess, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		S::System::System::Sleep(10);
	}

	if (specs->debug)
	{
		Dialogs::QuickMessage("Click OK to close console window.", "Info", Dialogs::Message::Buttons::Ok, Dialogs::Message::Icon::Information);

		FreeConsole();
	}

	/* Check if anything went wrong
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove output file
		 */
		File(encFileName).Delete();

		errorState  = True;
		errorString = String("Encoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

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

Int BoCA::AS::EncoderComponentExternalStdIO::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	/* Check if external encoder still exists.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(hProcess, &exitCode);

	if (exitCode != STILL_ACTIVE)
	{
		errorState  = True;
		errorString = "Encoder quit prematurely.";

		return -1;
	}

	/* Hand data over to the encoder using the stdio pipe
	 */
	out->OutputData(data, size);
	out->Flush();

	return size;
}
