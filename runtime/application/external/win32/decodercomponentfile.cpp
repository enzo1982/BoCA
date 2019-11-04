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

#include <boca/application/external/decodercomponentfile.h>
#include <boca/application/taggercomponent.h>
#include <boca/application/registry.h>
#include <boca/common/utilities.h>

#include <windows.h>
#include <mmreg.h>

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalFile::DecoderComponentExternalFile(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
	in	   = NIL;

	dataOffset = 0;
}

BoCA::AS::DecoderComponentExternalFile::~DecoderComponentExternalFile()
{
}

String BoCA::AS::DecoderComponentExternalFile::GetMD5(const String &encFileName)
{
	if (specs->external_md5_arguments == NIL) return NIL;

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
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= specs->external_md5_stderr ? GetStdHandle(STD_OUTPUT_HANDLE) : wPipe;
	startupInfo.hStdError	= specs->external_md5_stderr ? wPipe : GetStdHandle(STD_ERROR_HANDLE);

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL) return NIL;

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Read output into buffer.
	 */
	Buffer<char>		 buffer(4096);
	Int			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		if (!ReadFile(rPipe, buffer + bytesReadTotal, 4096 - bytesReadTotal, &bytesRead, NIL) || bytesRead == 0) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	String	 output = (bytesReadTotal > 0 ? (char *) buffer : NIL);

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

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (!specs->external_ignoreExitCode && exitCode != 0) return NIL;

	/* Extract MD5 from output.
	 */
	String	 md5;

	if (output.Contains(specs->external_md5_require) &&
	    output.Contains(specs->external_md5_prefix)) md5 = output.SubString(output.Find(specs->external_md5_prefix) + specs->external_md5_prefix.Length(),
										output.Length() - output.Find(specs->external_md5_prefix) - specs->external_md5_prefix.Length()).Trim().Head(32).ToLower();

	if (md5.Length() != 32 || md5.Contains("\n") || md5.Contains(" ")) md5 = NIL;

	return md5;
}

Error BoCA::AS::DecoderComponentExternalFile::GetStreamInfo(const String &streamURI, Track &track)
{
	/* Return cached track from previous call.
	 */
	if (this->track.fileName == streamURI)
	{
		track = this->track;

		return Success();
	}

	/* Create temporary WAVE file.
	 */
	String	 wavFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".wav");
	String	 encFileName = streamURI;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""))
							      .Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));

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
		errorString = String("Unable to run decoder ").Append(command).Append(".");

		/* Remove temporary file if necessary.
		 */
		if (String::IsUnicode(streamURI))
		{
			File(encFileName).Delete();
		}

		return Error();
	}

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
	if (String::IsUnicode(streamURI))
	{
		File(encFileName).Delete();
	}

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove temporary WAVE file.
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return Error();
	}

	/* Open decoded WAVE file and read header.
	 */
	InStream	*in = new InStream(STREAM_FILE, wavFileName, IS_READ);

	track.fileName = streamURI;
	track.fileSize = File(streamURI).GetFileSize();

	track.lossless = specs->formats.GetFirst()->IsLossless();

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
			if (cSize == 0xffffffff || cSize == 0) track.length = (in->Size() - in->GetPos()) / track.GetFormat().channels / (track.GetFormat().bits / 8);
			else				       track.length = cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);
		}
		else
		{
			/* Skip chunk.
			 */
			in->RelSeek(cSize + cSize % 2);
		}
	}
	while (!errorState && chunk != "data");

	/* Close input stream.
	 */
	delete in;

	/* Remove temporary WAVE file.
	 */
	File(wavFileName).Delete();

	/* Query tags and update track.
	 */
	QueryTags(streamURI, track);

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalFile::Activate()
{
	/* Create temporary WAVE file.
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".wav");
	encFileName = track.fileName;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.fileName))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.fileName).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.fileName).Copy(encFileName);
	}

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""))
							      .Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));

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
		errorString = String("Unable to run decoder ").Append(command).Append(".");

		/* Remove temporary file if necessary.
		 */
		if (String::IsUnicode(track.fileName))
		{
			File(encFileName).Delete();
		}

		return False;
	}

	/* Wait until the decoder exits.
	 */
	while (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

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
	}

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove temporary WAVE file.
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return False;
	}

	/* Open decoded WAVE file and skip the header.
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READ);

	in->Seek(12);

	String	 chunk;

	do
	{
		/* Read next chunk.
		 */
		chunk = in->InputString(4);

		UnsignedInt32	 cSize = in->InputNumber(4);

		if (chunk != "data")
		{
			/* Skip chunk.
			 */
			in->RelSeek(cSize + cSize % 2);
		}
	}
	while (chunk != "data");

	dataOffset = in->GetPos();

	return True;
}

Bool BoCA::AS::DecoderComponentExternalFile::Deactivate()
{
	/* Close input stream.
	 */
	delete in;

	/* Remove temporary WAVE file.
	 */
	File(wavFileName).Delete();

	return True;
}

Bool BoCA::AS::DecoderComponentExternalFile::Seek(Int64 samplePosition)
{
	in->Seek(dataOffset + samplePosition * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return True;
}

Int BoCA::AS::DecoderComponentExternalFile::ReadData(Buffer<UnsignedByte> &data)
{
	if (in->GetPos() == in->Size()) return -1;

	Int	 size = Math::Min((Int64) 2048, in->Size() - in->GetPos());

	/* Hand data over from the input file.
	 */
	data.Resize(size);

	in->InputData(data, size);

	ProcessData(data);

	return size;
}
