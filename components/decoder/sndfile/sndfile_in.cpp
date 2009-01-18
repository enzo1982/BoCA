 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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

#include <io.h>
#include <fcntl.h>

using namespace smooth::IO;

const String &BoCA::SndFileIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>LibSndFile Input</name>		\
		    <version>1.0</version>			\
		    <id>sndfile-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Windows Wave Files</name>		\
		      <extension>wav</extension>		\
		    </format>					\
		    <format>					\
		      <name>Apple Audio Files</name>		\
		      <extension>aif</extension>		\
		      <extension>aiff</extension>		\
		      <extension>aifc</extension>		\
		    </format>					\
		    <format>					\
		      <name>Sun Audio Files</name>		\
		      <extension>au</extension>			\
		      <extension>snd</extension>		\
		    </format>					\
		    <format>					\
		      <name>Paris Audio Files</name>		\
		      <extension>paf</extension>		\
		    </format>					\
		    <format>					\
		      <name>Amiga Audio Files</name>		\
		      <extension>iff</extension>		\
		      <extension>svx</extension>		\
		    </format>					\
		    <format>					\
		      <name>IRCAM Sound Files</name>		\
		      <extension>sf</extension>			\
		    </format>					\
		    <format>					\
		      <name>Creative Voice Files</name>		\
		      <extension>voc</extension>		\
		    </format>					\
		    <format>					\
		      <name>Sound Forge Wave64</name>		\
		      <extension>w64</extension>		\
		    </format>					\
		    <format>					\
		      <name>Portable Voice Format</name>	\
		      <extension>pvf</extension>		\
		    </format>					\
		    <format>					\
		      <name>HMM Toolkit Format</name>		\
		      <extension>htk</extension>		\
		    </format>					\
		    <format>					\
		      <name>Apple Core Audio</name>		\
		      <extension>caf</extension>		\
		    </format>					\
		  </component>					\
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
	if (Setup::enableUnicode) file = _wopen(streamURI, O_RDONLY | O_BINARY);
	else			  file = open(streamURI, O_RDONLY | O_BINARY);

	SF_INFO	 sinfo;

	ZeroMemory(&sinfo, sizeof(SF_INFO));

	sndf = ex_sf_open_fd(file, SFM_READ, &sinfo, False);

	if (sndf != NIL) ex_sf_close(sndf);

	_close(file);

	return (sndf != NIL);
}

Error BoCA::SndFileIn::GetStreamInfo(const String &streamURI, Track &track)
{
	if (Setup::enableUnicode) file = _wopen(streamURI, O_RDONLY | O_BINARY);
	else			  file = open(streamURI, O_RDONLY | O_BINARY);

	SF_INFO	 sinfo;

	ZeroMemory(&sinfo, sizeof(SF_INFO));

	sndf = ex_sf_open_fd(file, SFM_READ, &sinfo, False);

	if (sndf != NIL)
	{
		Format	&format = track.GetFormat();

		track.fileSize = _lseeki64(file, 0, SEEK_END);
		format.order	= BYTE_INTEL;

		format.channels	= sinfo.channels;
		format.rate	= sinfo.samplerate;

		switch (sinfo.format & 0xFF)
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

		track.length	= sinfo.frames * sinfo.channels;

		Info	&info = track.GetInfo();

		info.artist	= ex_sf_get_string(sndf, SF_STR_ARTIST);
		info.title	= ex_sf_get_string(sndf, SF_STR_TITLE);
		info.year	= (Int64) Number::FromIntString(ex_sf_get_string(sndf, SF_STR_DATE));
		info.comment	= ex_sf_get_string(sndf, SF_STR_COMMENT);

		ex_sf_close(sndf);
	}

	if (sndf == NIL) { errorState = True; errorString = "Unsupported audio format"; }

	_close(file);

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
	if (Setup::enableUnicode) file = _wopen(track.origFilename, O_RDONLY | O_BINARY);
	else			  file = open(track.origFilename, O_RDONLY | O_BINARY);

	SF_INFO	 sinfo;

	ZeroMemory(&sinfo, sizeof(SF_INFO));

	sndf = ex_sf_open_fd(file, SFM_READ, &sinfo, False);

	return True;
}

Bool BoCA::SndFileIn::Deactivate()
{
	ex_sf_close(sndf);

	_close(file);

	return True;
}

Int BoCA::SndFileIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	data.Resize(size);

	if (track.GetFormat().bits == 8)
	{
		Buffer<short>	 buffer(size);

		size = ex_sf_read_short(sndf, buffer, size);

		for (Int i = 0; i < size; i++) data[i] = (buffer[i] >> 8) - 128;
	}
	else if	(track.GetFormat().bits == 16)
	{
		size = ex_sf_read_short(sndf, (short *) (UnsignedByte *) data, size / 2) * 2;
	}
	else if (track.GetFormat().bits == 24)
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
	else if (track.GetFormat().bits == 32)
	{
		size = ex_sf_read_int(sndf, (int *) (UnsignedByte *) data, size / 4) * 4;
	}

	if (size == 0)	return -1;
	else		return size;
}
