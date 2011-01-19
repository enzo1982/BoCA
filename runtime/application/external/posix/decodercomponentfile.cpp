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

#include <sys/wait.h>

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
	/* Create temporary WAVE file
	 */
	wavFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".wav");
	encFileName = streamURI;

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
	String	 command   = String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String(encFileName).Replace(" ", "\\ ")).Replace("%OUTFILE", String(wavFileName).Replace(" ", "\\ "));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? "" : " 2> /dev/null"), "r");

	/* Wait until the encoder exits
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WEXITSTATUS(exitStatus);

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
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return Error();
	}

	/* Open decoded WAVE file and read header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READ);

	track.fileSize	= File(streamURI).GetFileSize();

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

			Format	 format = track.GetFormat();

			format.channels	= (unsigned short) in->InputNumber(2);
			format.rate	= (unsigned long) in->InputNumber(4);

			in->RelSeek(6);

			format.order	= BYTE_INTEL;
			format.bits	= (unsigned short) in->InputNumber(2);

			track.SetFormat(format);

			/* Skip rest of chunk
			 */
			in->RelSeek(cSize - 16);
		}
		else if (chunk == "data")
		{
			track.length	= (unsigned long) cSize / (track.GetFormat().bits / 8);
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
	String	 command   = String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString()).Replace("%INFILE", String(encFileName).Replace(" ", "\\ ")).Replace("%OUTFILE", String(wavFileName).Replace(" ", "\\ "));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? "" : " 2> /dev/null"), "r");

	/* Wait until the encoder exits
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WEXITSTATUS(exitStatus);

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

		errorState = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return False;
	}

	/* Open decoded WAVE file and skip the header
	 */
	in = new InStream(STREAM_FILE, wavFileName, IS_READ);

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

Bool BoCA::AS::DecoderComponentExternalFile::Seek(Int64 samplePosition)
{
	return False;
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
