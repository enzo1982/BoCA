 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/decodercomponentexternal.h>
#include <boca/common/utilities.h>

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternal::DecoderComponentExternal(ComponentSpecs *specs) : DecoderComponent(specs)
{
	errorState	= False;
	errorString	= "Unknown error";

	in = NIL;
}

BoCA::AS::DecoderComponentExternal::~DecoderComponentExternal()
{
}

Bool BoCA::AS::DecoderComponentExternal::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	for (Int i = 0; i < specs->formats.Length(); i++)
	{
		Format	*format = specs->formats.GetNth(i);

		for (Int j = 0; j < format->GetExtensions().Length(); j++)
		{
			if (lcURI.EndsWith(String(".").Append(format->GetExtensions().GetNth(j)))) return True;
		}
	}

	return False;
}

Error BoCA::AS::DecoderComponentExternal::GetStreamInfo(const String &streamURI, Track &format)
{
	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".wav");

	/* Start 3rd party command line decoder
	 */
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= specs->external_command;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tak"));

		execInfo.lpParameters	= String(specs->external_arguments).Replace("%INFILE", String("\"").Append(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tak")).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	}
	else
	{
		execInfo.lpParameters	= String(specs->external_arguments).Replace("%INFILE", String("\"").Append(streamURI).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	}

	execInfo.lpDirectory	= Application::GetApplicationDirectory();
	execInfo.nShow		= SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
	 */
	unsigned long	 exitCode = 0;

	do
	{
		Sleep(100);

		GetExitCodeProcess(execInfo.hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".tak")).Delete();
	}

	/* Open decoded WAVE file and read header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READONLY);

	format.fileSize	= File(streamURI).GetFileSize();
	format.order	= BYTE_INTEL;

	// Read RIFF chunk
	if (in->InputString(4) != "RIFF") { errorState = True; errorString = "Unknown file type"; }

	in->RelSeek(4);

	if (in->InputString(4) != "WAVE") { errorState = True; errorString = "Unknown file type"; }

	String		 chunk;

	do
	{
		// Read next chunk
		chunk = in->InputString(4);

		Int	 cSize = in->InputNumber(4);

		if (chunk == "fmt ")
		{
			if (in->InputNumber(2) != 1) { errorState = True; errorString = "Unsupported audio format"; }

			format.channels	= (unsigned short) in->InputNumber(2);
			format.rate	= (unsigned long) in->InputNumber(4);

			in->RelSeek(6);

			format.bits	= (unsigned short) in->InputNumber(2);

			// Skip rest of chunk
			in->RelSeek(cSize - 16);
		}
		else if (chunk == "data")
		{
			format.length	= (unsigned long) cSize / (format.bits / 8);
		}
		else
		{
			// Skip chunk
			in->RelSeek(cSize);
		}
	}
	while (!errorState && chunk != "data");

	/* Close input stream
	 */
	delete in;

	/* Remove temporary WAVE file
	 */
	File(wavFileName).Delete();

	return Success();
}

Void BoCA::AS::DecoderComponentExternal::SetInputFormat(const Track &track)
{
	format = track;
}

Int BoCA::AS::DecoderComponentExternal::GetPackageSize()
{
	return 0;
}

Int BoCA::AS::DecoderComponentExternal::SetDriver(IO::Driver *driver)
{
	return IO::Filter::SetDriver(driver);
}

Int64 BoCA::AS::DecoderComponentExternal::GetInBytes()
{
	return 0;
}

Bool BoCA::AS::DecoderComponentExternal::Activate()
{
	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".wav");

	/* Start 3rd party command line decoder
	 */
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= specs->external_command;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(format.origFilename))
	{
		File(format.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".tak"));

		execInfo.lpParameters	= String(specs->external_arguments).Replace("%INFILE", String("\"").Append(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".tak")).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	}
	else
	{
		execInfo.lpParameters	= String(specs->external_arguments).Replace("%INFILE", String("\"").Append(format.origFilename).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	}

	execInfo.lpDirectory	= Application::GetApplicationDirectory();
	execInfo.nShow		= SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
	 */
	unsigned long	 exitCode = 0;

	do
	{
		Sleep(100);

		GetExitCodeProcess(execInfo.hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(format.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(format.origFilename).Append(".tak")).Delete();
	}

	/* Open decoded WAVE file and skip the header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READONLY);

	in->Seek(44);

	return True;
}

Bool BoCA::AS::DecoderComponentExternal::Deactivate()
{
	/* Close input stream
	 */
	delete in;

	/* Remove temporary WAVE file
	 */
	File(wavFileName).Delete();

	return True;
}

Int BoCA::AS::DecoderComponentExternal::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (in->GetPos() == in->Size()) return -1;

	size = Math::Min(2048, in->Size() - in->GetPos());

	/* Hand data over from the input file
	 */
	data.Resize(size);

	in->InputData(data, size);

	return size;
}

BoCA::ConfigLayer *BoCA::AS::DecoderComponentExternal::GetConfigurationLayer()
{
	return NIL;
}

Void BoCA::AS::DecoderComponentExternal::FreeConfigurationLayer()
{
}
