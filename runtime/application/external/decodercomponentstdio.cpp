 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

#include <smooth/io/drivers/driver_win32.h>

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalStdIO::DecoderComponentExternalStdIO(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
}

BoCA::AS::DecoderComponentExternalStdIO::~DecoderComponentExternalStdIO()
{
}

Error BoCA::AS::DecoderComponentExternalStdIO::GetStreamInfo(const String &streamURI, Track &track)
{
	/* Get tagging mode and type
	 */
	Int	 tagMode = TAG_MODE_NONE;
	String	 tagType;

	String	 lcURI = streamURI.ToLower();

	for (Int i = 0; i < specs->formats.Length(); i++)
	{
		FileFormat	*format = specs->formats.GetNth(i);

		for (Int j = 0; j < format->GetExtensions().Length(); j++)
		{
			if (lcURI.EndsWith(String(".").Append(format->GetExtensions().GetNth(j))))
			{
				tagMode = format->GetTagMode();
				tagType = format->GetTagType();
			}
		}
	}

	/* Read tag if requested
	 */
	if (tagMode != TAG_MODE_NONE)
	{
		String			 taggerID;

		if	(tagType == "ID3v1")		taggerID = "id3v1-tag";
		else if	(tagType == "ID3v2")		taggerID = "id3v2-tag";
		else if	(tagType == "APEv2")		taggerID = "apev2-tag";
		else if (tagType == "MP4Metadata")	taggerID = "mp4-tag";

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) AS::Registry::Get().CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder
	 */
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

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	encFileName = streamURI;

	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	CreateProcessA(NIL, String(specs->external_command).Append(" ").Append(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String("\"").Append(encFileName).Append("\"")), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	hProcess = processInfo.hProcess;

	Buffer<UnsignedByte>	 buffer(44);
	Int			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		ReadFile(rPipe, buffer + bytesReadTotal, 44 - bytesReadTotal, &bytesRead, NIL);

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 44);

	InStream		*in = new InStream(STREAM_BUFFER, buffer, buffer.Size());

	/* Read decoded WAVE file header
	 */
	Format	&format = track.GetFormat();

	track.fileSize	= File(streamURI).GetFileSize();
	format.order	= BYTE_INTEL;

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
			if (in->InputNumber(2) != 1) { errorState = True; errorString = "Unsupported audio format"; }

			format.channels	= (unsigned short) in->InputNumber(2);
			format.rate	= (unsigned long) in->InputNumber(4);

			in->RelSeek(6);

			format.bits	= (unsigned short) in->InputNumber(2);

			/* Skip rest of chunk
			 */
			in->RelSeek(cSize - 16);
		}
		else if (chunk == "data")
		{
			track.length	= (unsigned long) cSize / (format.bits / 8);

			if (track.length == 0x3fffffff ||
			    track.length == 0x7fffffff) track.length = -1;
		}
		else
		{
			/* Skip chunk
			 */
			in->RelSeek(cSize);
		}
	}
	while (!errorState && chunk != "data");

	/* Close stdio pipe
	 */
	delete in;

	CloseHandle(rPipe);
	CloseHandle(wPipe);

	TerminateProcess(hProcess, 0);

	/* Wait until the decoder exits
	 */
	unsigned long	 exitCode = 0;

	do
	{
		Sleep(100);

		GetExitCodeProcess(hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(encFileName).Delete();
	}

	if (exitCode != 0) return Error();

	if (errorState) return Error();

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Activate()
{
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line encoder
	 */
	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= SW_HIDE;
	startupInfo.hStdInput	= NIL;
	startupInfo.hStdOutput	= wPipe;
	startupInfo.hStdError	= NIL;

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	encFileName = track.origFilename;

	if (String::IsUnicode(track.origFilename))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.origFilename).Copy(encFileName);
	}

	CreateProcessA(NIL, String(specs->external_command).Append(" ").Append(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String("\"").Append(encFileName).Append("\"")), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	hProcess = processInfo.hProcess;

	/* Skip the WAVE header
	 */
	Buffer<UnsignedByte>	 buffer(44);
	Int			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		ReadFile(rPipe, buffer + bytesReadTotal, 44 - bytesReadTotal, &bytesRead, NIL);

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 44);

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Deactivate()
{
	/* Close stdio pipe
	 */
	CloseHandle(rPipe);
	CloseHandle(wPipe);

	TerminateProcess(hProcess, 0);

	/* Wait until the decoder exits
	 */
	unsigned long	 exitCode = 0;

	do
	{
		Sleep(100);

		GetExitCodeProcess(hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		File(encFileName).Delete();
	}

	if (exitCode != 0) return False;

	return True;
}

Int BoCA::AS::DecoderComponentExternalStdIO::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	/* Check if external decoder still exists.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(hProcess, &exitCode);

	if (exitCode != STILL_ACTIVE) return -1;

	/* Hand data over from the input file
	 */
	size = 6144;

	data.Resize(size);

	ReadFile(rPipe, data, size, (DWORD *) &size, NIL);

	if (size == 0) return -1;

	return size;
}
