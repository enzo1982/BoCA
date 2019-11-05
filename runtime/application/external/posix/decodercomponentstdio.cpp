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

#include <boca/application/external/decodercomponentstdio.h>
#include <boca/application/taggercomponent.h>
#include <boca/application/registry.h>
#include <boca/common/utilities.h>

#include <smooth/io/drivers/driver_ansi.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM	       0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalStdIO::DecoderComponentExternalStdIO(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
	rPipe	    = NIL;

	samplesRead = 0;
}

BoCA::AS::DecoderComponentExternalStdIO::~DecoderComponentExternalStdIO()
{
}


String BoCA::AS::DecoderComponentExternalStdIO::GetMD5(const String &encFileName)
{
	if (specs->external_md5_arguments == NIL) return NIL;

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
													 .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
													 .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
													 .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*rPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->external_md5_stderr ? "2>&1" : (specs->debug ? NIL : " 2> /dev/null")), "r");

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

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												     .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												     .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												     .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*rPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "r");

	/* Read WAVE header into buffer.
	 */
	Buffer<UnsignedByte>	 buffer(4096);
	Int64			 bytesReadTotal = 0;
	Int			 bytesRead = 0;

	do
	{
		bytesRead = fread(buffer + bytesReadTotal, 1, 4096 - bytesReadTotal, rPipe);

		if (bytesRead != 4096 - bytesReadTotal && (ferror(rPipe) || bytesRead == 0)) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	if (bytesReadTotal >= 44)
	{
		InStream	*in = new InStream(STREAM_BUFFER, buffer, bytesReadTotal);

		/* Read decoded WAVE file header.
		 */
		track.decoderID	= specs->id;

		track.fileName	= streamURI;
		track.fileSize	= File(streamURI).GetFileSize();

		track.lossless	= specs->formats.GetFirst()->IsLossless();

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
				if (cSize == 0xffffffff || cSize == 0) track.length = -1;
				else				       track.length = cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);

				/* Read the rest of the file to find actual size.
				 */
				if (track.length == -1)
				{
					Buffer<UnsignedByte>	 data(12288);

					while (True)
					{
						Int	 size = fread(data, 1, data.Size(), rPipe);

						if (size == 0) break;

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

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(rPipe);
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

	/* Query tags and update track.
	 */
	QueryTags(streamURI, track);

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute ").Append(command).Append(".");
		else if (exitCode == 127) errorString = String("External decoder ").Append(command).Append(" not found.");

		return Error();
	}

	if (errorState) return Error();

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
	}

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_arguments).Replace("%OPTIONS", specs->GetExternalArgumentsString())
							      .Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
												     .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
												     .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
												     .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	rPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->debug ? NIL : " 2> /dev/null"), "r");

	/* Skip the WAVE header.
	 */
	Buffer<UnsignedByte>	 buffer(8);
	Int32			 bytesRead = 0;
	Int32			 chunkSize = 0;

	bytesRead = fread(buffer, 1,	     8, rPipe); // RIFF chunk
	bytesRead = fread(buffer, 1,	     4, rPipe); // WAVE ID
	bytesRead = fread(buffer, 1,	     4, rPipe); //  fmt FOURCC
	bytesRead = fread(buffer, 1,	     4, rPipe); //  fmt chunk size

	chunkSize = buffer[0] | buffer[1] << 8 | buffer[2] << 16 | buffer[3] << 24;

	buffer.Resize(Math::Max(8, chunkSize));

	bytesRead = fread(buffer, 1, chunkSize, rPipe); // rest of  fmt chunk

	do
	{
		bytesRead = fread(buffer, 1,	     8, rPipe); // chunk header

		if (buffer[0] == 'd' && buffer[1] == 'a' && buffer[2] == 't' && buffer[3] == 'a') break;

		chunkSize = buffer[4] | buffer[5] << 8 | buffer[6] << 16 | buffer[7] << 24;

		buffer.Resize(Math::Max(8, chunkSize));

		bytesRead = fread(buffer, 1, chunkSize, rPipe); // rest of chunk
	}
	while (True);

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Deactivate()
{
	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(rPipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(track.fileName))
	{
		File(encFileName).Delete();
	}

	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		if	(exitCode == 126) errorString = String("Permission denied to execute \"").Append(String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter())).Append("\".");
		else if (exitCode == 127) errorString = String("External decoder \"").Append(String(specs->external_command).Replace("/", Directory::GetDirectoryDelimiter())).Append("\" not found.");

		return False;
	}

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Seek(Int64 samplePosition)
{
	const Format		&format = track.GetFormat();
	Buffer<UnsignedByte>	 buffer(12288);

	while (samplesRead < samplePosition)
	{
		Int	 size  = Math::Min((samplePosition - samplesRead) * format.channels * (format.bits / 8), (Int64) buffer.Size());
		Int	 bytes = fread(buffer, 1, size, rPipe);

		if (bytes != size && (ferror(rPipe) || bytes == 0))
		{
			errorState  = True;
			errorString = "Decoder quit prematurely.";

			return False;
		}

		samplesRead += bytes / format.channels / (format.bits / 8);
	}

	return True;
}

Int BoCA::AS::DecoderComponentExternalStdIO::ReadData(Buffer<UnsignedByte> &data)
{
	/* Hand data over from the input file.
	 */
	Int	 size = 12288;

	data.Resize(size);

	Int	 bytes = fread(data, 1, size, rPipe);

	if (bytes != size && (ferror(rPipe) || bytes == 0))
	{
		errorState  = True;
		errorString = "Decoder quit prematurely.";

		return -1;
	}

	data.Resize(bytes);

	ProcessData(data);

	/* Increment number of samples read.
	 */
	const Format	&format = track.GetFormat();

	samplesRead += bytes / format.channels / (format.bits / 8);

	return size;
}
