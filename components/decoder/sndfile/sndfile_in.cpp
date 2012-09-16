 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "sndfile_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::SndFileIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>LibSndFile Input</name>				\
		    <version>1.0</version>					\
		    <id>sndfile-in</id>						\
		    <type>decoder</type>					\
		    <format>							\
		      <name>Windows Wave Files</name>				\
		      <extension>wav</extension>				\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
		    </format>							\
		    <format>							\
		      <name>Apple Audio Files</name>				\
		      <extension>aif</extension>				\
		      <extension>aiff</extension>				\
		      <extension>aifc</extension>				\
		    </format>							\
		    <format>							\
		      <name>Sun Audio Files</name>				\
		      <extension>au</extension>					\
		      <extension>snd</extension>				\
		    </format>							\
		    <format>							\
		      <name>Paris Audio Files</name>				\
		      <extension>paf</extension>				\
		    </format>							\
		    <format>							\
		      <name>Amiga Audio Files</name>				\
		      <extension>iff</extension>				\
		      <extension>svx</extension>				\
		    </format>							\
		    <format>							\
		      <name>IRCAM Sound Files</name>				\
		      <extension>sf</extension>					\
		    </format>							\
		    <format>							\
		      <name>Creative Voice Files</name>				\
		      <extension>voc</extension>				\
		    </format>							\
		    <format>							\
		      <name>Sound Forge Wave64</name>				\
		      <extension>w64</extension>				\
		    </format>							\
		    <format>							\
		      <name>Portable Voice Format</name>			\
		      <extension>pvf</extension>				\
		    </format>							\
		    <format>							\
		      <name>HMM Toolkit Format</name>				\
		      <extension>htk</extension>				\
		    </format>							\
		    <format>							\
		      <name>Apple Core Audio</name>				\
		      <extension>caf</extension>				\
		    </format>							\
		  </component>							\
										\
		";
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

Bool BoCA::SndFileIn::CanOpenStream(const String &streamURI)
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
	if (Setup::enableUnicode) file = _wfopen(streamURI, L"rb");
	else			  file = fopen(streamURI, "rb");
#else
	file = fopen(streamURI.ConvertTo("UTF-8"), "r+b");
#endif

	if (file != NIL)
	{
		SF_INFO	 sinfo;

		memset(&sinfo, 0, sizeof(SF_INFO));

		SNDFILE	*sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

		if (sndf != NIL) ex_sf_close(sndf);

		fclose(file);

		return (sndf != NIL);
	}

	return False;
}

Error BoCA::SndFileIn::GetStreamInfo(const String &streamURI, Track &track)
{
	FILE	*file = 0;

#ifdef __WIN32__
	if (Setup::enableUnicode) file = _wfopen(streamURI, L"rb");
	else			  file = fopen(streamURI, "rb");
#else
	file = fopen(streamURI.ConvertTo("UTF-8"), "r+b");
#endif

	SF_INFO	 sinfo;

	memset(&sinfo, 0, sizeof(SF_INFO));

	SNDFILE	*sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

	if (sndf != NIL)
	{
		Format	 format = track.GetFormat();

		track.fileSize	= File(streamURI).GetFileSize();
		format.order	= BYTE_INTEL;

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

	if (sndf == NIL) { errorState = True; errorString = "Unsupported audio format"; }

	fclose(file);

	if (!errorState)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*cartTagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

		if (cartTagger != NIL)
		{
			cartTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(cartTagger);
		}

		AS::TaggerComponent	*riffTagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (riffTagger != NIL)
		{
			riffTagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(riffTagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::SndFileIn::SndFileIn()
{
	packageSize = 0;

	file = 0;
	sndf = NIL;
}

BoCA::SndFileIn::~SndFileIn()
{
}

Bool BoCA::SndFileIn::Activate()
{
#ifdef __WIN32__
	if (Setup::enableUnicode) file = _wfopen(track.origFilename, L"rb");
	else			  file = fopen(track.origFilename, "rb");
#else
	file = fopen(track.origFilename.ConvertTo("UTF-8"), "r+b");
#endif

	SF_INFO	 sinfo;

	memset(&sinfo, 0, sizeof(SF_INFO));

	sndf = ex_sf_open_fd(fileno(file), SFM_READ, &sinfo, False);

	return True;
}

Bool BoCA::SndFileIn::Deactivate()
{
	ex_sf_close(sndf);

	fclose(file);

	return True;
}

Bool BoCA::SndFileIn::Seek(Int64 samplePosition)
{
	if (ex_sf_seek(sndf, samplePosition, SEEK_CUR) != -1) return True;
	else						      return False;
}

Int BoCA::SndFileIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	const Format	&format = track.GetFormat();

	/* Set size to a sample number a multiple of the number of channels.
	 */
	size -= size % (format.bits / 8 * format.channels);

	data.Resize(size);

	if (format.bits == 8)
	{
		Buffer<short>	 buffer(size);

		size = ex_sf_read_short(sndf, buffer, size);

		for (Int i = 0; i < size; i++) data[i] = (buffer[i] >> 8) - 128;
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
			data[i * 3 + 0] = (buffer[i] >>  8) & 0xFF;
			data[i * 3 + 1] = (buffer[i] >> 16) & 0xFF;
			data[i * 3 + 2] = (buffer[i] >> 24) & 0xFF;
		}
	}
	else if (format.bits == 32)
	{
		size = ex_sf_read_int(sndf, (int *) (UnsignedByte *) data, size / 4) * 4;
	}

	if (size == 0)	return -1;
	else		return size;
}
