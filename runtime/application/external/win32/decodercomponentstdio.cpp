 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/decodercomponentstdio.h>
#include <boca/common/config.h>
#include <boca/common/utilities.h>

#include <smooth/io/drivers/driver_win32.h>

#include <windows.h>
#include <mmreg.h>

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalStdIO::DecoderComponentExternalStdIO(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
	rPipe	    = NIL;
	wPipe	    = NIL;

	hProcess    = NIL;

	samplesRead = 0;
}

BoCA::AS::DecoderComponentExternalStdIO::~DecoderComponentExternalStdIO()
{
}

Error BoCA::AS::DecoderComponentExternalStdIO::GetStreamInfo(const String &streamURI, Track &track)
{
	String	 encFileName = streamURI;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	/* Set up security attributes
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	HANDLE	 rPipe = NIL;
	HANDLE	 wPipe = NIL;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= NIL;
	startupInfo.hStdOutput	= wPipe;
	startupInfo.hStdError	= NIL;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	HANDLE	 hProcess = processInfo.hProcess;

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Read WAVE header into buffer.
	 */
	Buffer<UnsignedByte>	 buffer(4096);
	Int			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		if (!ReadFile(rPipe, buffer + bytesReadTotal, 4096 - bytesReadTotal, &bytesRead, NIL) || bytesRead == 0) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	if (bytesReadTotal >= 44)
	{
		InStream		*in = new InStream(STREAM_BUFFER, buffer, bytesReadTotal);

		/* Read decoded WAVE file header
		 */
		track.origFilename = streamURI;
		track.fileSize	   = File(streamURI).GetFileSize();

		/* Read RIFF chunk
		 */
		if (in->InputString(4) != "RIFF") { errorState = True; errorString = "Unknown file type"; }

		in->RelSeek(4);

		if (in->InputString(4) != "WAVE") { errorState = True; errorString = "Unknown file type"; }

		String		 chunk;

		do
		{
			/* Read next chunk
			 */
			chunk = in->InputString(4);

			Int	 cSize = in->InputNumber(4);

			if (chunk == "fmt ")
			{
				Int	 waveFormat = in->InputNumber(2);

				if (waveFormat != WAVE_FORMAT_PCM &&
				    waveFormat != WAVE_FORMAT_EXTENSIBLE) { errorState = True; errorString = "Unsupported audio format"; }

				Format	 format = track.GetFormat();

				format.channels	= (unsigned short) in->InputNumber(2);
				format.rate	= (unsigned long) in->InputNumber(4);

				in->RelSeek(6);

				format.bits	= (unsigned short) in->InputNumber(2);
				format.order	= BYTE_INTEL;

				track.SetFormat(format);

				/* Skip rest of chunk
				 */
				in->RelSeek(cSize - 16 + cSize % 2);
			}
			else if (chunk == "data")
			{
				if ((unsigned) cSize == 0xffffffff || (unsigned) cSize == 0) track.length = -1;
				else							     track.length = (unsigned long) cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);

				/* Read the rest of the file to find actual size.
				 */
				if (track.length == -1)
				{
					Buffer<UnsignedByte>	 data(12288);

					while (True)
					{
						Int	 size = 0;

						if (!ReadFile(rPipe, data, data.Size(), (DWORD *) &size, NIL) || size == 0) break;

						bytesReadTotal += size;
					}

					track.length = (bytesReadTotal - in->GetPos()) / track.GetFormat().channels / (track.GetFormat().bits / 8);
				}
			}
			else
			{
				/* Skip chunk
				 */
				in->RelSeek(cSize + cSize % 2);
			}
		}
		while (!errorState && chunk != "data");

		/* Close stdio pipe
		 */
		delete in;
	}

	CloseHandle(rPipe);

	TerminateProcess(hProcess, 0);

	/* Wait until the decoder exits.
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

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(encFileName).Delete();
	}

	/* Query tags and update track
	 */
	QueryTags(streamURI, track);

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return Error();
	}

	if (errorState) return Error();

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Activate()
{
	encFileName = track.origFilename;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.origFilename).Copy(encFileName);
	}

	/* Set up security attributes
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= NIL;
	startupInfo.hStdOutput	= wPipe;
	startupInfo.hStdError	= NIL;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	hProcess = processInfo.hProcess;

	/* Close stdio pipe write handle before reading.
	 */
	CloseHandle(wPipe);

	/* Skip the WAVE header.
	 */
	Buffer<UnsignedByte>	 buffer(8);
	DWORD			 bytesRead = 0;
	DWORD			 chunkSize = 0;

	ReadFile(rPipe, buffer,		8, &bytesRead, NIL); // RIFF chunk
	ReadFile(rPipe, buffer,		4, &bytesRead, NIL); // WAVE ID
	ReadFile(rPipe, buffer,		4, &bytesRead, NIL); //  fmt FOURCC
	ReadFile(rPipe, buffer,		4, &bytesRead, NIL); //  fmt chunk size

	chunkSize = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;

	buffer.Resize(Math::Max(8, chunkSize));

	ReadFile(rPipe, buffer, chunkSize, &bytesRead, NIL); // rest of  fmt chunk

	do
	{
		ReadFile(rPipe, buffer,		8, &bytesRead, NIL); // chunk header

		if (buffer[0] == 'd' && buffer[1] == 'a' && buffer[2] == 't' && buffer[3] == 'a') break;

		chunkSize = buffer[4] | buffer[5] << 8 | buffer[6] << 16 | buffer[7] << 24;

		buffer.Resize(Math::Max(8, chunkSize));

		ReadFile(rPipe, buffer, chunkSize, &bytesRead, NIL); // rest of chunk
	}
	while (True);

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Deactivate()
{
	/* Close stdio pipe read handle.
	 */
	CloseHandle(rPipe);

	TerminateProcess(hProcess, 0);

	/* Wait until the decoder exits.
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

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		File(encFileName).Delete();
	}

	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return False;
	}

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Seek(Int64 samplePosition)
{
	const Format		&format = track.GetFormat();
	Buffer<UnsignedByte>	 buffer(12288);

	while (samplesRead < samplePosition)
	{
		Int	 size = Math::Min((samplePosition - samplesRead) * format.channels * (format.bits / 8), (Int64) buffer.Size());

		if (!ReadFile(rPipe, buffer, size, (DWORD *) &size, NIL) || size == 0)
		{
			errorState  = True;
			errorString = "Decoder quit prematurely.";

			return False;
		}

		samplesRead += size / format.channels / (format.bits / 8);
	}

	return True;
}

Int BoCA::AS::DecoderComponentExternalStdIO::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	/* Hand data over from the input file.
	 */
	size = 12288;

	data.Resize(size);

	if (!ReadFile(rPipe, data, size, (DWORD *) &size, NIL) || size == 0)
	{
		errorState  = True;
		errorString = "Decoder quit prematurely.";

		return -1;
	}

	/* Increment number of samples read.
	 */
	const Format	&format = track.GetFormat();

	samplesRead += size / format.channels / (format.bits / 8);

	return size;
}
