 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <smooth/io/drivers/driver_posix.h>

#include "flac_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::FLACIn::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>FLAC Audio Decoder</name>		\
	    <version>1.0</version>			\
	    <id>flac-in</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>FLAC Audio Files</name>		\
	      <extension>flac</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFLACDLL();
}

Void smooth::DetachDLL()
{
	FreeFLACDLL();
}

namespace BoCA
{
	FLAC__StreamDecoderReadStatus	 FLACStreamDecoderReadCallback(const FLAC__StreamDecoder *, FLAC__byte [], size_t *, void *);
	FLAC__StreamDecoderWriteStatus	 FLACStreamDecoderWriteCallback(const FLAC__StreamDecoder *, const FLAC__Frame *, const FLAC__int32 * const [], void *);
	FLAC__StreamDecoderSeekStatus	 FLACStreamDecoderSeekCallback(const FLAC__StreamDecoder *, FLAC__uint64, void *);
	FLAC__StreamDecoderTellStatus	 FLACStreamDecoderTellCallback(const FLAC__StreamDecoder *, FLAC__uint64 *, void *);
	FLAC__StreamDecoderLengthStatus	 FLACStreamDecoderLengthCallback(const FLAC__StreamDecoder *, FLAC__uint64 *, void *);
	FLAC__bool			 FLACStreamDecoderEofCallback(const FLAC__StreamDecoder *, void *);
	void				 FLACStreamDecoderMetadataCallback(const FLAC__StreamDecoder *, const FLAC__StreamMetadata *, void *);
	void				 FLACStreamDecoderErrorCallback(const FLAC__StreamDecoder *, FLAC__StreamDecoderErrorStatus, void *);
};

Bool BoCA::FLACIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".flac");
}

Error BoCA::FLACIn::GetStreamInfo(const String &streamURI, Track &format)
{
	Driver		*ioDriver = new DriverPOSIX(streamURI, IS_READONLY);
	InStream	*f_in = new InStream(STREAM_DRIVER, ioDriver);

	format.order	= BYTE_INTEL;
	format.fileSize	= f_in->Size();

	infoFormat = new Track;
	finished = False;

	driver = ioDriver;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	decoderThread = NonBlocking1<Bool>(&FLACIn::ReadFLAC, this).Call(False);

	while (decoderThread->GetStatus() == THREAD_RUNNING) Sleep(10);

	delete readDataMutex;
	delete samplesBufferMutex;

	delete f_in;
	delete ioDriver;

	format.length	= infoFormat->length;
	format.bits	= infoFormat->bits;
	format.channels	= infoFormat->channels;
	format.rate	= infoFormat->rate;
	format.artist	= infoFormat->artist;
	format.title	= infoFormat->title;
	format.album	= infoFormat->album;
	format.genre	= infoFormat->genre;
	format.year	= infoFormat->year;
	format.track	= infoFormat->track;

	for (Int i = 0; i < infoFormat->pictures.Length(); i++)
	{
		format.pictures.Add(infoFormat->pictures.GetNth(i));
	}

	delete infoFormat;

	return Success();
}

BoCA::FLACIn::FLACIn()
{
	packageSize = 0;
}

BoCA::FLACIn::~FLACIn()
{
}

Bool BoCA::FLACIn::Activate()
{
	finished = False;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	readDataMutex->Lock();

	infoFormat = new Track();

	decoderThread = NonBlocking1<Bool>(&FLACIn::ReadFLAC, this).Call(True);

	return True;
}

Bool BoCA::FLACIn::Deactivate()
{
	decoderThread->Stop();

	delete readDataMutex;
	delete samplesBufferMutex;

	delete infoFormat;

	return True;
}

Int BoCA::FLACIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	readDataMutex->Release();

	while (decoderThread->GetStatus() == THREAD_RUNNING && samplesBuffer.Size() <= 0) Sleep(10);

	readDataMutex->Lock();

	samplesBufferMutex->Lock();

	size = samplesBuffer.Size() * (format.bits / 8);

	data.Resize(size);

	for (Int i = 0; i < samplesBuffer.Size(); i++)
	{
		if	(format.bits == 8)  data[i] = samplesBuffer[i] + 128;
		else if (format.bits == 16) ((Short *) (unsigned char *) data)[i] = samplesBuffer[i];
		else if (format.bits == 24) { data[3 * i] = samplesBuffer[i] & 255; data[3 * i + 1] = (samplesBuffer[i] >> 8) & 255; data[3 * i + 2] = (samplesBuffer[i] >> 16) & 255; }
	}

	samplesBuffer.Resize(0);

	samplesBufferMutex->Release();

	return size;
}

Int BoCA::FLACIn::ReadFLAC(Bool readData)
{
	decoder = ex_FLAC__stream_decoder_new();

	if (!readData)
	{
		ex_FLAC__stream_decoder_set_metadata_respond(decoder, FLAC__METADATA_TYPE_VORBIS_COMMENT);
		ex_FLAC__stream_decoder_set_metadata_respond(decoder, FLAC__METADATA_TYPE_PICTURE);
	}

	ex_FLAC__stream_decoder_init_stream(decoder, &FLACStreamDecoderReadCallback, &FLACStreamDecoderSeekCallback, &FLACStreamDecoderTellCallback, &FLACStreamDecoderLengthCallback, &FLACStreamDecoderEofCallback, &FLACStreamDecoderWriteCallback, &FLACStreamDecoderMetadataCallback, &FLACStreamDecoderErrorCallback, this);

	ex_FLAC__stream_decoder_process_until_end_of_metadata(decoder);

	if (readData) ex_FLAC__stream_decoder_process_until_end_of_stream(decoder);

	ex_FLAC__stream_decoder_finish(decoder);
	ex_FLAC__stream_decoder_delete(decoder);

	return Success();
}

FLAC__StreamDecoderReadStatus BoCA::FLACStreamDecoderReadCallback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	filter->readDataMutex->Lock();

	*bytes = filter->driver->ReadData(buffer, *bytes);

	filter->readDataMutex->Release();

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus BoCA::FLACStreamDecoderWriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	filter->samplesBufferMutex->Lock();

	Int	 oSize = filter->samplesBuffer.Size();

	filter->samplesBuffer.Resize(oSize + frame->header.blocksize * filter->format.channels);

	for (Int i = 0; i < (signed) frame->header.blocksize; i++)
	{
		for (Int j = 0; j < filter->format.channels; j++)
		{
			filter->samplesBuffer[oSize + i * filter->format.channels + j] = buffer[j][i];
		}
	}

	filter->samplesBufferMutex->Release();

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus BoCA::FLACStreamDecoderSeekCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	filter->driver->Seek(absolute_byte_offset);

	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus BoCA::FLACStreamDecoderTellCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	*absolute_byte_offset = filter->driver->GetPos();

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus BoCA::FLACStreamDecoderLengthCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	*stream_length = filter->driver->GetSize();

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool BoCA::FLACStreamDecoderEofCallback(const FLAC__StreamDecoder *decoder, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	return (filter->driver->GetPos() == filter->driver->GetSize());
}

void BoCA::FLACStreamDecoderMetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		filter->infoFormat->bits	= metadata->data.stream_info.bits_per_sample;
		filter->infoFormat->channels	= metadata->data.stream_info.channels;
		filter->infoFormat->rate	= metadata->data.stream_info.sample_rate;
		filter->infoFormat->length	= metadata->data.stream_info.total_samples * filter->infoFormat->channels;
	}
	else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
		if (metadata->data.vorbis_comment.num_comments > 0)
		{
			filter->infoFormat->track = -1;
			filter->infoFormat->outfile = NIL;

			char	*prevInFormat = String::SetInputFormat("UTF-8");

			for (Int j = 0; j < (signed) metadata->data.vorbis_comment.num_comments; j++)
			{
				String	 comment = String((char *) metadata->data.vorbis_comment.comments[j].entry);
				String	 id = String().CopyN(comment, comment.Find("=")).ToUpper();

				if	(id == "TITLE")		filter->infoFormat->title  = comment.Tail(comment.Length() - 6);
				else if (id == "ARTIST")	filter->infoFormat->artist = comment.Tail(comment.Length() - 7);
				else if (id == "ALBUM")		filter->infoFormat->album  = comment.Tail(comment.Length() - 6);
				else if (id == "GENRE")		filter->infoFormat->genre  = comment.Tail(comment.Length() - 6);
				else if (id == "DATE")		filter->infoFormat->year   = comment.Tail(comment.Length() - 5).ToInt();
				else if (id == "TRACKNUMBER")	filter->infoFormat->track  = comment.Tail(comment.Length() - 12).ToInt();
			}

			String::SetInputFormat(prevInFormat);
		}
	}
	else if (metadata->type == FLAC__METADATA_TYPE_PICTURE)
	{
		Picture	 picture;

		picture.type = metadata->data.picture.type;
		picture.mime = metadata->data.picture.mime_type;
		picture.description.ImportFrom("UTF-8", (char *) metadata->data.picture.description);

		picture.data.Resize(metadata->data.picture.data_length);

		// FixMe: I don't know why, but without the memset statement memcpy hangs the process
		//	  in about 20% of all cases on Windows XP x64 when the buffers are > 1MB.

		memset(picture.data, 0, picture.data.Size());
		memcpy(picture.data, metadata->data.picture.data, picture.data.Size());

		filter->infoFormat->pictures.Add(picture);
	}
}

void BoCA::FLACStreamDecoderErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
}
