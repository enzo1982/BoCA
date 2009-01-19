 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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
	execInfo.lpFile		= specs->external_command;
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

	do
	{
		Sleep(100);

		GetExitCodeProcess(execInfo.hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	File(wavFileName).Delete();

	if (exitCode != 0) return False;

	Config	*config = Config::Get();

	/* Create tag if requested
	 */
	Buffer<UnsignedByte>	 tag;
	Int			 tagSize = 0;

	if (specs->external_tagmode != TAG_MODE_NONE && (info.artist != NIL || info.title != NIL))
	{
		if	(specs->external_tag == "ID3v1" && config->enable_id3v1 && config->enable_id3)	tagSize = track.RenderID3v1Tag(tag);
		else if (specs->external_tag == "ID3v2" && config->enable_id3v2 && config->enable_id3)	tagSize = track.RenderID3v2Tag(tag);
		else if (specs->external_tag == "MP4Metadata" && config->enable_mp4)			track.RenderMP4Meta(encFileName);
		else if (specs->external_tag == "APEv2")						tagSize = track.RenderAPETag(tag);
	}

	/* Prepend tag
	 */
	if (specs->external_tagmode == TAG_MODE_PREPEND)
	{
		driver->WriteData(tag, tagSize);
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
	if (specs->external_tagmode == TAG_MODE_APPEND)
	{
		driver->WriteData(tag, tagSize);
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
