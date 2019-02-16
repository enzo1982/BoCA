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

#include <smooth.h>
#include <smooth/dll.h>

#include "sndfile.h"

using namespace smooth::IO;

const String &BoCA::DecoderSndFile::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>SndFile Input Component %VERSION%</name>		\
		    <version>1.0</version>					\
		    <id>sndfile-dec</id>					\
		    <type>decoder</type>					\
		    <replace>aiff-dec</replace>					\
		    <replace>au-dec</replace>					\
		    <replace>voc-dec</replace>					\
		    <replace>wave-dec</replace>					\
		    <format>							\
		      <name>Microsoft Wave Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>wav</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>		\
		    </format>							\
		    <format>							\
		      <name>Apple Audio Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>aif</extension>				\
		      <extension>aiff</extension>				\
		      <extension>aifc</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>		\
		      <tag id=\"tocplist-tag\" mode=\"other\">.TOC.plist</tag>	\
		    </format>							\
		    <format>							\
		      <name>Apple Core Audio Files</name>			\
		      <lossless>true</lossless>					\
		      <extension>caf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Sony Media Wave64 Files</name>			\
		      <lossless>true</lossless>					\
		      <extension>w64</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>RIFF 64 Audio Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>rf64</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>Sun Audio Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>au</extension>					\
		      <extension>snd</extension>				\
		    </format>							\
		    <format>							\
		      <name>Creative Voice Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>voc</extension>				\
		    </format>							\
		    <format>							\
		      <name>Amiga Audio Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>iff</extension>				\
		      <extension>svx</extension>				\
		    </format>							\
		    <format>							\
		      <name>IRCAM Sound Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>sf</extension>					\
		    </format>							\
		    <format>							\
		      <name>Paris Audio Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>paf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Portable Voice Format</name>			\
		      <lossless>true</lossless>					\
		      <extension>pvf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Psion WVE Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>wve</extension>				\
		    </format>							\
		    <format>							\
		      <name>HMM Toolkit Format</name>				\
		      <lossless>true</lossless>					\
		      <extension>htk</extension>				\
		    </format>							\
		    <format>							\
		      <name>Audio Visual Research Format</name>			\
		      <lossless>true</lossless>					\
		      <extension>avr</extension>				\
		    </format>							\
		  </component>							\
										\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_sf_version_string()).Replace("libsndfile-", NIL)));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadSndFileDLL();
}

Void smooth::DetachDLL()
{
	FreeSndFileDLL();
}

Bool BoCA::DecoderSndFile::CanOpenStream(const String &streamURI)
{
	/* Do not open Ogg files with SndFile.
	 */
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) == "OggS") return False;

	in.Close();

	/* Check if we can handle this file.
	 */
	FILE	*file = 0;

#ifdef __WIN32__
	file = _wfopen(streamURI, L"rb");
#else
	file = fopen(streamURI.ConvertTo("UTF-8"), "rb");
#endif

	if (file != NIL)
	{
		SF_INFO	 sinfo;

		memset(&sinfo, 0, sizeof(SF_INFO));

		SNDFILE	*sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

		if (sndf != NIL) ex_sf_close(sndf);

		fclose(file);

		if (sndf != NIL) return True;
	}

	return False;
}

Error BoCA::DecoderSndFile::GetStreamInfo(const String &streamURI, Track &track)
{
	FILE	*file = 0;

#ifdef __WIN32__
	file = _wfopen(streamURI, L"rb");
#else
	file = fopen(streamURI.ConvertTo("UTF-8"), "rb");
#endif

	if (file == NIL) { errorState = True; errorString = "File not found"; }

	if (file != NIL)
	{
		SF_INFO	 sinfo;

		memset(&sinfo, 0, sizeof(SF_INFO));

		SNDFILE	*sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

		if (sndf == NIL) { errorState = True; errorString = "Unknown file type"; }

		if (sndf != NIL)
		{
			Format	 format = track.GetFormat();

			track.fileSize	= File(streamURI).GetFileSize();

			format.channels	= sinfo.channels;
			format.rate	= sinfo.samplerate;

			switch (sinfo.format & SF_FORMAT_SUBMASK)
			{
				case SF_FORMAT_PCM_U8:
				case SF_FORMAT_PCM_S8:
					format.bits = 8;
					break;
				case SF_FORMAT_PCM_24:
					format.bits = 24;
					break;
				case SF_FORMAT_PCM_32:
					format.bits = 32;
					break;
				case SF_FORMAT_FLOAT:
				case SF_FORMAT_DOUBLE:
					format.bits = 32;
					format.fp   = True;
					break;
				default:
					format.bits = 16;
					break;
			}

			track.SetFormat(format);

			track.length	= sinfo.frames;

			Info	 info = track.GetInfo();

			info.artist	= ex_sf_get_string(sndf, SF_STR_ARTIST);
			info.title	= ex_sf_get_string(sndf, SF_STR_TITLE);
			info.album	= ex_sf_get_string(sndf, SF_STR_ALBUM);
			info.track	= (Int64) Number::FromIntString(ex_sf_get_string(sndf, SF_STR_TRACKNUMBER));
			info.year	= (Int64) Number::FromIntString(ex_sf_get_string(sndf, SF_STR_DATE));
			info.genre	= ex_sf_get_string(sndf, SF_STR_GENRE);
			info.comment	= ex_sf_get_string(sndf, SF_STR_COMMENT);

			track.SetInfo(info);

			ex_sf_close(sndf);
		}

		fclose(file);
	}

	if (!errorState)
	{
		AS::Registry		&boca = AS::Registry::Get();

		/* Read CART tag if any.
		 */
		AS::TaggerComponent	*cartTagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

		if (cartTagger != NIL)
		{
			cartTagger->SetConfiguration(GetConfiguration());
			cartTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(cartTagger);
		}

		/* Read RIFF tags if any.
		 */
		AS::TaggerComponent	*riffTagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (riffTagger != NIL)
		{
			riffTagger->SetConfiguration(GetConfiguration());
			riffTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(riffTagger);
		}

		/* Read .TOC.plist if it exists.
		 */
		AS::TaggerComponent	*tocTagger = (AS::TaggerComponent *) boca.CreateComponentByID("tocplist-tag");

		if (tocTagger != NIL)
		{
			tocTagger->SetConfiguration(GetConfiguration());
			tocTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tocTagger);
		}

		/* Read RIFF or AIFF embedded ID3 tag.
		 */
		InStream	 in(STREAM_FILE, streamURI, IS_READ);
		String		 magic = in.InputString(4);

		if (magic == "RIFF" || magic == "FORM")
		{
			UnsignedInt32	 rSize = (magic == "RIFF") ? in.InputNumber(4) : in.InputNumberRaw(4);
			String		 type = in.InputString(4);

			while (type == "WAVE" || type == "AIFF" || type == "AIFC")
			{
				if (in.GetPos() >= in.Size()) break;

				/* Read next chunk.
				 */
				String		 chunk = in.InputString(4);
				UnsignedInt32	 cSize = (magic == "RIFF") ? in.InputNumber(4) : in.InputNumberRaw(4);

				if (chunk == "id3 " || chunk == "ID3 ")
				{
					Buffer<UnsignedByte>	 buffer(cSize);

					in.InputData(buffer, cSize);

					AS::Registry		&boca = AS::Registry::Get();
					AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

					if (tagger != NIL)
					{
						tagger->SetConfiguration(GetConfiguration());
						tagger->ParseBuffer(buffer, track);

						boca.DeleteComponent(tagger);
					}

					break;
				}
				else if (chunk == "data")
				{
					if (rSize == 0xFFFFFFFF || rSize == 0 ||
					    cSize == 0xFFFFFFFF || cSize == 0) break;
				}

				/* Skip chunk.
				 */
				in.RelSeek(cSize + cSize % 2);
			}
		}

		in.Close();
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::DecoderSndFile::DecoderSndFile()
{
	fileFormat = 0;

	file	   = 0;
	sndf	   = NIL;
}

BoCA::DecoderSndFile::~DecoderSndFile()
{
}

Bool BoCA::DecoderSndFile::Activate()
{
	/* Open input file.
	 */
#ifdef __WIN32__
	file = _wfopen(track.origFilename, L"rb");
#else
	file = fopen(track.origFilename.ConvertTo("UTF-8"), "rb");
#endif

	if (file == NIL) return False;

	/* Open sndfile handle.
	 */
	SF_INFO	 sinfo;

	memset(&sinfo, 0, sizeof(SF_INFO));

	sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

	if (sndf == NIL) { fclose(file); return False; }

	fileFormat = sinfo.format & SF_FORMAT_TYPEMASK;

	return True;
}

Bool BoCA::DecoderSndFile::Deactivate()
{
	/* Close sndfile handle.
	 */
	ex_sf_close(sndf);

	fclose(file);

	return True;
}

Bool BoCA::DecoderSndFile::Seek(Int64 samplePosition)
{
	if (ex_sf_seek(sndf, samplePosition, SEEK_SET) != -1) return True;
	else						      return False;
}

Int BoCA::DecoderSndFile::ReadData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	/* Set size to a multiple of the number of channels.
	 */
	Int	 size = data.Size() - data.Size() % (format.bits / 8 * format.channels);

	data.Resize(size);

	if (format.bits == 8)
	{
		Buffer<short>	 buffer(size);

		size = ex_sf_read_short(sndf, buffer, size);

		for (Int i = 0; i < size; i++) ((signed char *) (UnsignedByte *) data)[i] = buffer[i] >> 8;
	}
	else if	(format.bits == 16)
	{
		size = ex_sf_read_short(sndf, (short *) (UnsignedByte *) data, size / 2) * 2;
	}
	else if (format.bits == 24)
	{
		Buffer<int>	 buffer(size / 2);

		size = ex_sf_read_int(sndf, buffer, size / 2) * 3;

		data.Resize(size);

		for (Int i = 0; i < size / 3; i++)
		{
			if (endianness == EndianLittle)	{ data[i * 3 + 0] = (buffer[i] >>  8) & 0xFF; data[i * 3 + 1] = (buffer[i] >> 16) & 0xFF; data[i * 3 + 2] = (buffer[i] >> 24) & 0xFF; }
			else				{ data[i * 3 + 2] = (buffer[i] >>  8) & 0xFF; data[i * 3 + 1] = (buffer[i] >> 16) & 0xFF; data[i * 3 + 0] = (buffer[i] >> 24) & 0xFF; }
		}
	}
	else if (format.bits == 32 && !format.fp)
	{
		size = ex_sf_read_int(sndf, (int *) (UnsignedByte *) data, size / 4) * 4;
	}
	else if (format.bits == 32 && format.fp)
	{
		size = ex_sf_read_float(sndf, (float *) (UnsignedByte *) data, size / 4) * 4;
	}

	/* Reorder channels.
	 */
	if (fileFormat == SF_FORMAT_AIFF ||
	    fileFormat == SF_FORMAT_CAF)
	{
		if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::AIFF_5_1, Channel::Default_5_1);
	}

	if (size == 0)	return -1;
	else		return size;
}
