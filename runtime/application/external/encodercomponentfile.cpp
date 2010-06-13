 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponentfile.h>
#include <boca/common/config.h>
#include <boca/common/utilities.h>

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

using namespace smooth::IO;

BoCA::AS::EncoderComponentExternalFile::EncoderComponentExternalFile(ComponentSpecs *specs) : EncoderComponentExternal(specs)
{
	out = NIL;
}

BoCA::AS::EncoderComponentExternalFile::~EncoderComponentExternalFile()
{
}

Bool BoCA::AS::EncoderComponentExternalFile::Activate()
{
	/* Create temporary WAVE file
	 */
	nOfSamples = 0;

	wavFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".wav");
	encFileName = Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".").Append(GetOutputFileExtension());

	out = new OutStream(STREAM_FILE, wavFileName, OS_OVERWRITE);

	/* Write WAVE header
	 */
	out->OutputString("RIFF");
	out->OutputNumber(track.length * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");
	out->OutputString("fmt ");

	out->OutputNumber(16, 4);
	out->OutputNumber(1, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);

	out->OutputNumber(format.bits, 2);
	out->OutputString("data");
	out->OutputNumber(track.length * (format.bits / 8), 4);

	return True;
}

Bool BoCA::AS::EncoderComponentExternalFile::Deactivate()
{
	/* Finalize and close the uncompressed temporary file
	 */
	Int	 size = nOfSamples * (format.bits / 8) + 36;

	out->Seek(4);
	out->OutputData((unsigned char *) &size, 4);

	size -= 36;

	out->Seek(40);
	out->OutputData((unsigned char *) &size, 4);

	delete out;

	/* Start 3rd party command line encoder
	 */
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	const Info		&info = track.GetInfo();

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= String(specs->external_command).Replace("/", "\\");
	execInfo.lpParameters	= String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
								   .Replace("%INFILE", String("\"").Append(wavFileName).Append("\""))
								   .Replace("%OUTFILE", String("\"").Append(encFileName).Append("\""))
								   .Replace("%ARTIST", String("\"").Append((char *) info.artist).Append("\""))
								   .Replace("%ALBUM", String("\"").Append((char *) info.album).Append("\""))
								   .Replace("%TITLE", String("\"").Append((char *) info.title).Append("\""))
								   .Replace("%TRACK", String("\"").Append(String::FromInt(info.track)).Append("\""))
								   .Replace("%YEAR", String("\"").Append(String::FromInt(info.year)).Append("\""))
								   .Replace("%GENRE", String("\"").Append((char *) info.genre).Append("\""));

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

	File(wavFileName).Delete();

	/* Check if anything went wrong
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0)
	{
		/* Remove output file
		 */
		File(encFileName).Delete();

		return False;
	}

	Config	*config = Config::Get();

	/* Get tagging mode and type
	 */
	Int	 tagMode = specs->formats.GetFirst()->GetTagMode();
	String	 tagType = specs->formats.GetFirst()->GetTagType();

	/* Create tag if requested
	 */
	Buffer<UnsignedByte>	 tag;

	if (tagMode != TAG_MODE_NONE && (info.artist != NIL || info.title != NIL))
	{
		String			 taggerID;

		if	(tagType == "ID3v1"	  && config->GetIntValue("Tags", "EnableID3v1", False))      taggerID = "id3v1-tag";
		else if	(tagType == "ID3v2"	  && config->GetIntValue("Tags", "EnableID3v2", True))	     taggerID = "id3v2-tag";
		else if	(tagType == "APEv2"	  && config->GetIntValue("Tags", "EnableAPEv2", True))	     taggerID = "apev2-tag";
		else if (tagType == "MP4Metadata" && config->GetIntValue("Tags", "EnableMP4Metadata", True)) taggerID = "mp4-tag";

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			if (tagMode == TAG_MODE_OTHER)	tagger->RenderStreamInfo(encFileName, track);
			else				tagger->RenderBuffer(tag, track);

			boca.DeleteComponent(tagger);
		}
	}

	/* Prepend tag
	 */
	if (tagMode == TAG_MODE_PREPEND)
	{
		driver->WriteData(tag, tag.Size());
	}

	/* Stream contents of created file to output driver
	 */
	InStream		 in(STREAM_FILE, encFileName, IS_READONLY);
	Buffer<UnsignedByte>	 buffer(1024);
	Int			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		driver->WriteData((UnsignedByte *) in.InputData(buffer, Math::Min(1024, bytesLeft)), Math::Min(1024, bytesLeft));

		bytesLeft -= Math::Min(1024, bytesLeft);
	}

	/* Append tag
	 */
	if (tagMode == TAG_MODE_APPEND)
	{
		driver->WriteData(tag, tag.Size());
	}

	in.Close();

	File(encFileName).Delete();

	return True;
}

Int BoCA::AS::EncoderComponentExternalFile::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	/* Hand data over to the output file
	 */
	nOfSamples += (size / (format.bits / 8));

	out->OutputData(data, size);

	return size;
}
