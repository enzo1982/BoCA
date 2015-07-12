 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/decodercomponentfile.h>
#include <boca/common/config.h>
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

	/* Start 3rd party command line decoder
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

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

	/* Check process handle.
	 */
	if (hProcess == NIL) return NIL;

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

	String	 output = (char *) buffer;

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
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

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
	/* Create temporary WAVE file
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

	/* Start 3rd party command line decoder
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""))
							      .Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));

	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize	     = sizeof(execInfo);
	execInfo.fMask	     = SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb	     = "open";
	execInfo.lpDirectory = Application::GetApplicationDirectory();
	execInfo.nShow	     = specs->debug ? SW_SHOW : SW_HIDE;

	if (specs->debug)
	{
		execInfo.lpFile	      = String("cmd.exe");
		execInfo.lpParameters = String("/c ").Append(command).Append(" ").Append(arguments).Append(" & pause");
	}
	else
	{
		execInfo.lpFile	      = String(command);
		execInfo.lpParameters = String(arguments);
	}

	ShellExecuteExA(&execInfo);

	/* Check process handle.
	 */
	if (execInfo.hProcess == NIL)
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

	/* Wait until the decoder exits
	 */
	unsigned long	 exitCode = 0;

	while (True)
	{
		GetExitCodeProcess(execInfo.hProcess, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		S::System::System::Sleep(10);
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

	/* Check if anything went wrong
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove temporary WAVE file
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return Error();
	}

	/* Open decoded WAVE file and read header
	 */
	InStream	*in = new InStream(STREAM_FILE, wavFileName, IS_READ);

	track.origFilename = streamURI;
	track.fileSize	   = File(streamURI).GetFileSize();

	track.lossless	   = specs->formats.GetFirst()->IsLossless();

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
			track.length	= (unsigned long) cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);
		}
		else
		{
			/* Skip chunk
			 */
			in->RelSeek(cSize + cSize % 2);
		}
	}
	while (!errorState && chunk != "data");

	/* Close input stream
	 */
	delete in;

	/* Remove temporary WAVE file
	 */
	File(wavFileName).Delete();

	/* Query tags and update track
	 */
	QueryTags(streamURI, track);

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalFile::Activate()
{
	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".wav");
	encFileName = track.origFilename;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.origFilename).Copy(encFileName);
	}

	/* Start 3rd party command line decoder
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String("\"").Append(encFileName).Append("\""))
							      .Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));

	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize	     = sizeof(execInfo);
	execInfo.fMask	     = SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb	     = "open";
	execInfo.lpDirectory = Application::GetApplicationDirectory();
	execInfo.nShow	     = specs->debug ? SW_SHOW : SW_HIDE;

	if (specs->debug)
	{
		execInfo.lpFile	      = String("cmd.exe");
		execInfo.lpParameters = String("/c ").Append(command).Append(" ").Append(arguments).Append(" & pause");
	}
	else
	{
		execInfo.lpFile	      = String(command);
		execInfo.lpParameters = String(arguments);
	}

	ShellExecuteExA(&execInfo);

	/* Check process handle.
	 */
	if (execInfo.hProcess == NIL)
	{
		errorState  = True;
		errorString = String("Unable to run decoder ").Append(command).Append(".");

		/* Remove temporary file if necessary.
		 */
		if (String::IsUnicode(track.origFilename))
		{
			File(encFileName).Delete();
		}

		return False;
	}

	/* Wait until the decoder exits
	 */
	unsigned long	 exitCode = 0;

	while (True)
	{
		GetExitCodeProcess(execInfo.hProcess, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		S::System::System::Sleep(10);
	}

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		File(encFileName).Delete();
	}

	/* Check if anything went wrong
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove temporary WAVE file
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return False;
	}

	/* Open decoded WAVE file and skip the header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READ);

	in->Seek(12);

	String	 chunk;

	do
	{
		/* Read next chunk
		 */
		chunk = in->InputString(4);

		Int	 cSize = in->InputNumber(4);

		if (chunk != "data")
		{
			/* Skip chunk
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
	/* Close input stream
	 */
	delete in;

	/* Remove temporary WAVE file
	 */
	File(wavFileName).Delete();

	return True;
}

Bool BoCA::AS::DecoderComponentExternalFile::Seek(Int64 samplePosition)
{
	in->Seek(dataOffset + samplePosition * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return True;
}

Int BoCA::AS::DecoderComponentExternalFile::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (in->GetPos() == in->Size()) return -1;

	size = Math::Min((Int64) 2048, in->Size() - in->GetPos());

	/* Hand data over from the input file
	 */
	data.Resize(size);

	in->InputData(data, size);

	return size;
}
