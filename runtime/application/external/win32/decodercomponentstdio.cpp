 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/decodercomponentstdio.h>
#include <boca/application/external/utilities.h>
#include <boca/application/taggercomponent.h>
#include <boca/application/registry.h>
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

String BoCA::AS::DecoderComponentExternalStdIO::GetMD5(const String &encFileName)
{
	return ExternalUtilities::GetMD5(specs, encFileName);
}

Float BoCA::AS::DecoderComponentExternalStdIO::GetApproximateDuration(const String &encFileName)
{
	if (!specs->external_command.EndsWith("ffmpeg.exe") && !specs->external_command.EndsWith("avconv.exe")) return -1;

	/* Set up security attributes.
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	HANDLE	 rPipe = NIL;
	HANDLE	 wPipe = NIL;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String("-i \"").Append(encFileName).Append("\"");

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= NIL;
	startupInfo.hStdError	= wPipe;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		CloseHandle(rPipe);

		return -1;
	}

	/* Read output into buffer.
	 */
	String		 output;
	Buffer<char>	 buffer(4096);
	DWORD		 bytesRead = 0;

	while (True)
	{
		if (!ReadFile(rPipe, buffer, 4096, &bytesRead, NIL) || bytesRead == 0) break;

		output.Append((char *) buffer);
	}

	CloseHandle(rPipe);

	TerminateProcess(processInfo.hProcess, 0);

	/* Wait until the decoder exits.
	 */
	while (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	/* Extract duration from output.
	 */
	if (output.Find("Duration: ") >= 0)
	{
		String			 duration = output.SubString(output.Find("Duration: ") + 10, 11);
		const Array<String>	&parts	  = duration.Explode(":");

		if (parts.Length() == 3) return parts.GetNth(0).ToInt() * 60 * 60 +
						parts.GetNth(1).ToInt()      * 60 +
						parts.GetNth(2).ToFloat();
	}

	return -1;
}

Error BoCA::AS::DecoderComponentExternalStdIO::GetStreamInfo(const String &streamURI, Track &track)
{
	/* Return cached track from previous call.
	 */
	if (this->track.fileName == streamURI)
	{
		track = this->track;

		return Success();
	}

	String	 encFileName = streamURI;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	/* Get approximate duration (FFmpeg decoder only).
	 */
	Float	 approximateDuration = GetApproximateDuration(encFileName);

	/* Set up security attributes.
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	HANDLE	 rPipe = NIL;
	HANDLE	 wPipe = NIL;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString(GetConfiguration()))
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= wPipe;
	startupInfo.hStdError	= specs->debug ? GetStdHandle(STD_ERROR_HANDLE) : NIL;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		CloseHandle(rPipe);

		errorState  = True;
		errorString = String("Unable to run decoder ").Append(command).Append(".");

		/* Remove temporary file if necessary.
		 */
		if (String::IsUnicode(streamURI)) File(encFileName).Delete();

		return Error();
	}

	/* Read WAVE header into buffer.
	 */
	Buffer<UnsignedByte>	 buffer(4096);
	Int64			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		if (!ReadFile(rPipe, buffer + bytesReadTotal, 4096 - bytesReadTotal, &bytesRead, NIL) || bytesRead == 0) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	if (bytesReadTotal >= 44)
	{
		InStream	*in = new InStream(STREAM_BUFFER, buffer, bytesReadTotal);

		/* Read decoded WAVE file header.
		 */
		track.fileName = streamURI;
		track.fileSize = File(streamURI).GetFileSize();

		/* Read RIFF chunk.
		 */
		if (in->InputString(4) != "RIFF") { errorState = True; errorString = "Unknown file type"; }

		in->RelSeek(4);

		if (in->InputString(4) != "WAVE") { errorState = True; errorString = "Unknown file type"; }

		String		 chunk;

		do
		{
			/* Read next chunk.
			 */
			chunk = in->InputString(4);

			UnsignedInt32	 cSize = in->InputNumber(4);

			if (chunk == "fmt ")
			{
				Int	 waveFormat = in->InputNumber(2);

				if (waveFormat != WAVE_FORMAT_PCM	 &&
				    waveFormat != WAVE_FORMAT_IEEE_FLOAT &&
				    waveFormat != WAVE_FORMAT_EXTENSIBLE) { errorState = True; errorString = "Unsupported audio format"; }

				Format	 format = track.GetFormat();

				format.channels	= (unsigned short) in->InputNumber(2);
				format.rate	= (unsigned long) in->InputNumber(4);

				in->RelSeek(6);

				format.fp	= (waveFormat == WAVE_FORMAT_IEEE_FLOAT);
				format.bits	= (unsigned short) in->InputNumber(2);
				format.order	= BYTE_INTEL;

				if (format.bits == 8) format.sign = False;

				track.SetFormat(format);

				if (approximateDuration >= 0) track.approxLength = approximateDuration * format.rate;

				/* Skip rest of chunk.
				 */
				in->RelSeek(cSize - 16 + cSize % 2);
			}
			else if (chunk == "LIST")
			{
				/* Copy chunk to separate buffer.
				 */
				Buffer<UnsignedByte>	 info(8 + cSize + cSize % 2);

				in->RelSeek(-8);
				in->InputData(info, info.Size());

				/* Parse LIST INFO chunk.
				 */
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*riffTagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

				if (riffTagger != NIL)
				{
					riffTagger->SetConfiguration(GetConfiguration());
					riffTagger->ParseBuffer(info, track);

					boca.DeleteComponent(riffTagger);
				}
			}
			else if (chunk == "data")
			{
				if (cSize == 0xffffffff || cSize == 0) track.length = -1;
				else				       track.length = cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);

				/* Read the rest of the file to find actual size.
				 */
				if (track.length == -1 && track.approxLength == -1)
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
				/* Skip chunk.
				 */
				in->RelSeek(cSize + cSize % 2);
			}
		}
		while (!errorState && chunk != "data");

		/* Close stdio pipe.
		 */
		delete in;
	}

	CloseHandle(rPipe);

	TerminateProcess(processInfo.hProcess, 0);

	/* Wait until the decoder exits.
	 */
	while (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	if (specs->debug)
	{
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

	/* Query MD5.
	 */
	track.md5 = GetMD5(encFileName);

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI)) File(encFileName).Delete();

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (track.length == -1 && track.approxLength == -1 && !specs->external_ignoreExitCode && exitCode != 0)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");
	}

	if (errorState) return Error();

	/* Query tags and update track.
	 */
	QueryTags(streamURI, track);

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Activate()
{
	encFileName = track.fileName;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.fileName))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.fileName).Copy(encFileName);

		/* Look for a companion file.
		 */
		File	 companionFile = GetCompanionFile(track.fileName);

		if (companionFile.Exists()) companionFile.Copy(GetCompanionFile(encFileName));
	}

	const Format	&format = track.GetFormat();

	/* Set up security attributes.
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 32768 * format.channels * (format.bits / 8));
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString(GetConfiguration()))
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= wPipe;
	startupInfo.hStdError	= specs->debug ? GetStdHandle(STD_ERROR_HANDLE) : NIL;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	hProcess = processInfo.hProcess;

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		CloseHandle(rPipe);

		errorState  = True;
		errorString = String("Unable to run decoder ").Append(command).Append(".");

		/* Remove temporary file if necessary.
		 */
		if (String::IsUnicode(track.fileName)) File(encFileName).Delete();

		return False;
	}

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
	while (WaitForSingleObject(hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	if (specs->debug)
	{
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.fileName))
	{
		File(encFileName).Delete();

		/* Delete companion file too.
		 */
		File	 companionFile = GetCompanionFile(encFileName);

		if (companionFile.Exists()) companionFile.Delete();
	}

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(hProcess, &exitCode);

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
	Int64			 bytesLeft = (samplePosition - samplesRead) * format.channels * (format.bits / 8) - preBuffer.Size();

	while (bytesLeft > 0)
	{
		Int	 size = Math::Min(bytesLeft, (Int64) buffer.Size());

		if (!ReadFile(rPipe, buffer, size, (DWORD *) &size, NIL) || size == 0)
		{
			errorState  = True;
			errorString = "Decoder quit prematurely.";

			return False;
		}

		bytesLeft -= size;
	}

	samplesRead = samplePosition;

	preBuffer.Resize(0);

	return True;
}

Int BoCA::AS::DecoderComponentExternalStdIO::ReadData(Buffer<UnsignedByte> &data)
{
	/* Hand data over from the input file.
	 */
	Int	 preread = preBuffer.Size();
	Int	 size	 = 12288 - preread;

	data.Resize(preread + size);

	memcpy(data, preBuffer, preread);

	if (!ReadFile(rPipe, data + preread, size, (DWORD *) &size, NIL) || size == 0)
	{
		if (track.length != -1)
		{
			errorState  = True;
			errorString = "Decoder quit prematurely.";
		}

		return -1;
	}

	/* Make sure to read whole samples.
	 */
	const Format	&format	  = track.GetFormat();
	Int		 overread = (preread + size) % (format.channels * (format.bits / 8));

	preBuffer.Resize(overread);

	memcpy(preBuffer, data + preread + size - overread, overread);

	data.Resize(preread + size - overread);

	/* Adjust byte order and calculate MD5.
	 */
	ProcessData(data);

	/* Increment number of samples read.
	 */
	samplesRead += data.Size() / format.channels / (format.bits / 8);

	if (track.length == -1 && track.approxLength > 0) inBytes = track.fileSize / Math::Max(1.f, Float(track.approxLength) / samplesRead);

	return data.Size();
}
