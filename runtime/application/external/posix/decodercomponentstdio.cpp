 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
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

#include <smooth/io/drivers/driver_ansi.h>

#include <signal.h>
#include <sys/wait.h>

#define WAVE_FORMAT_PCM	       0x0001
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

using namespace smooth::IO;

BoCA::AS::DecoderComponentExternalStdIO::DecoderComponentExternalStdIO(ComponentSpecs *specs) : DecoderComponentExternal(specs)
{
	rPipe = NIL;
}

BoCA::AS::DecoderComponentExternalStdIO::~DecoderComponentExternalStdIO()
{
}

Error BoCA::AS::DecoderComponentExternalStdIO::GetStreamInfo(const String &streamURI, Track &track)
{
	String	 encFileName = streamURI;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(streamURI))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(streamURI).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(streamURI).Copy(encFileName);
	}

	/* Start 3rd party command line encoder
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
	Buffer<UnsignedByte>	 buffer(76);
	Int			 bytesReadTotal = 0;
	Int			 bytesRead = 0;

	do
	{
		bytesRead = fread(buffer + bytesReadTotal, 1, 76 - bytesReadTotal, rPipe);

		if (bytesRead != 76 - bytesReadTotal && (ferror(rPipe) || bytesRead == 0)) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 76);

	if (bytesReadTotal >= 44)
	{
		InStream		*in = new InStream(STREAM_BUFFER, buffer, bytesReadTotal);

		/* Read decoded WAVE file header
		 */
		track.origFilename = streamURI;
		track.fileSize	   = File(streamURI).GetFileSize();

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

				track.SetFormat(format);

				/* Skip rest of chunk
				 */
				in->RelSeek(cSize - 16 + cSize % 2);
			}
			else if (chunk == "data")
			{
				if ((unsigned) cSize == 0xffffffff || (unsigned) cSize == 0) track.length = -1;
				else							     track.length = (unsigned long) cSize / track.GetFormat().channels / (track.GetFormat().bits / 8);

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
				/* Skip chunk
				 */
				in->RelSeek(cSize + cSize % 2);
			}
		}
		while (!errorState && chunk != "data");

		/* Close stdio pipe
		 */
		delete in;
	}

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(rPipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	/* Remove temporary copy if necessary.
	 */
	if (String::IsUnicode(streamURI))
	{
		File(encFileName).Delete();
	}

	/* Query tags and update track
	 */
	QueryTags(streamURI, track);

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return Error();
	}

	if (errorState) return Error();

	return Success();
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Activate()
{
	encFileName = track.origFilename;

	/* Copy the file and decode the temporary copy
	 * if the file name contains Unicode characters.
	 */
	if (String::IsUnicode(track.origFilename))
	{
		encFileName = Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".").Append(specs->formats.GetFirst()->GetExtensions().GetFirst());

		File(track.origFilename).Copy(encFileName);
	}

	/* Start 3rd party command line encoder.
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
	if (String::IsUnicode(track.origFilename))
	{
		File(encFileName).Delete();
	}

	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE)
	{
		errorState  = True;
		errorString = String("Decoder returned exit code ").Append(String::FromInt((signed) exitCode)).Append(".");

		return False;
	}

	return True;
}

Bool BoCA::AS::DecoderComponentExternalStdIO::Seek(Int64 samplePosition)
{
	return False;
}

Int BoCA::AS::DecoderComponentExternalStdIO::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	/* Hand data over from the input file.
	 */
	size = 12288;

	data.Resize(size);

	Int	 bytes = fread(data, 1, size, rPipe);

	if (bytes != size && (ferror(rPipe) || bytes == 0)) return -1;

	return size;
}
