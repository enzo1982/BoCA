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

#include <boca/application/external/decodercomponentfile.h>
#include <boca/application/taggercomponent.h>
#include <boca/application/registry.h>
#include <boca/common/utilities.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM	       0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

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

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
													 .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
													 .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
													 .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*rPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->external_md5_stderr ? " 2>&1" : (specs->debug ? NIL : " 2> /dev/null")), "r");

	/* Read output into buffer.
	 */
	Buffer<char>	 buffer(4096);
	Int		 bytesReadTotal = 0;
	Int		 bytesRead = 0;

	do
	{
		bytesRead = fread(buffer + bytesReadTotal, 1, 4096 - bytesReadTotal, rPipe);

		if (bytesRead != 4096 - bytesReadTotal && (ferror(rPipe) || bytesRead == 0)) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	String	 output = (bytesReadTotal > 0 ? (char *) buffer : NIL);

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(rPipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE) return NIL;

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
							      .Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												     .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												     .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												     .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"))
							      .Replace("%OUTFILE", String(wavFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												      .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												      .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												      .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "r");

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

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
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		/* Remove temporary WAVE file.
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute ").Append(command).Append(".");
		else if (exitCode == 127) errorString = String("External decoder ").Append(command).Append(" not found.");

		return Error();
	}

	/* Open decoded WAVE file and read header.
	 */
	InStream	*in = new InStream(STREAM_FILE, wavFileName, IS_READ);

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

		/* Look for a companion file.
		 */
		File	 companionFile = GetCompanionFile(track.fileName);

		if (companionFile.Exists()) companionFile.Copy(GetCompanionFile(encFileName));
	}

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												     .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												     .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												     .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"))
							      .Replace("%OUTFILE", String(wavFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												      .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												      .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												      .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "r");

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

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
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		/* Remove temporary WAVE file.
		 */
		File(wavFileName).Delete();

		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute ").Append(command).Append(".");
		else if (exitCode == 127) errorString = String("External decoder ").Append(command).Append(" not found.");

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
