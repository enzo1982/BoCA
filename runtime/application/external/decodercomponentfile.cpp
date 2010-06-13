 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
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

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalFile::DecoderComponentExternalFile(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
	in = NIL;
}

BoCA::AS::DecoderComponentExternalFile::~DecoderComponentExternalFile()
{
}

Error BoCA::AS::DecoderComponentExternalFile::GetStreamInfo(const String &streamURI, Track &track)
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
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".wav");
	encFileName = streamURI;

	/* Start 3rd party command line decoder
	 */
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= String(specs->external_command).Replace("/", "\\");

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	execInfo.lpParameters	= String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String("\"").Append(encFileName).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	execInfo.lpDirectory	= Application::GetApplicationDirectory();
	execInfo.nShow		= SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
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

		errorState = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt(exitCode)).Append(".");

		return Error();
	}

	/* Open decoded WAVE file and read header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READONLY);

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
		}
		else
		{
			/* Skip chunk
			 */
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

Bool BoCA::AS::DecoderComponentExternalFile::Activate()
{
	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".wav");
	encFileName = track.origFilename;

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
	if (String::IsUnicode(track.origFilename))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.origFilename).Copy(encFileName);
	}

	execInfo.lpParameters	= String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String("\"").Append(encFileName).Append("\"")).Replace("%OUTFILE", String("\"").Append(wavFileName).Append("\""));
	execInfo.lpDirectory	= Application::GetApplicationDirectory();
	execInfo.nShow		= specs->debug ? SW_SHOW : SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
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

		return False;
	}

	/* Open decoded WAVE file and skip the header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READONLY);

	in->Seek(44);

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

Int BoCA::AS::DecoderComponentExternalFile::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (in->GetPos() == in->Size()) return -1;

	size = Math::Min(2048, in->Size() - in->GetPos());

	/* Hand data over from the input file
	 */
	data.Resize(size);

	in->InputData(data, size);

	return size;
}
