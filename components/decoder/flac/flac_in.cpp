 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
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
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>FLAC Audio Decoder</name>				\
		    <version>1.0</version>					\
		    <id>flac-in</id>						\
		    <type>decoder</type>					\
		    <format>							\
		      <name>FLAC Audio Files</name>				\
		      <extension>flac</extension>				\
		      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

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

Error BoCA::FLACIn::GetStreamInfo(const String &streamURI, Track &track)
{
	Driver		*ioDriver = new DriverPOSIX(streamURI, IS_READ);
	InStream	*f_in = new InStream(STREAM_DRIVER, ioDriver);

	track.fileSize	= f_in->Size();

	infoTrack = &track;
	stop = False;

	driver = ioDriver;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	decoderThread = NonBlocking1<Bool>(&FLACIn::ReadFLAC, this).Call(False);

	while (decoderThread->GetStatus() == THREAD_RUNNING) S::System::System::Sleep(0);

	delete readDataMutex;
	delete samplesBufferMutex;

	delete f_in;
	delete ioDriver;

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
	stop = False;

	seekPosition = 0;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	readDataMutex->Lock();

	infoTrack = new Track();

	decoderThread = NIL;

	return True;
}

Bool BoCA::FLACIn::Deactivate()
{
	if (decoderThread != NIL)
	{
		stop = True;

		readDataMutex->Release();

		while (decoderThread->GetStatus() == THREAD_RUNNING) S::System::System::Sleep(0);
	}

	delete readDataMutex;
	delete samplesBufferMutex;

	delete infoTrack;

	return True;
}

Bool BoCA::FLACIn::Seek(Int64 samplePosition)
{
	seekPosition = samplePosition;

	return True;
}

Int BoCA::FLACIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (decoderThread == NIL) decoderThread = NonBlocking1<Bool>(&FLACIn::ReadFLAC, this).Call(True);

	if (decoderThread->GetStatus() != THREAD_RUNNING && samplesBuffer.Size() <= 0) return -1;

	readDataMutex->Release();

	while (decoderThread->GetStatus() == THREAD_RUNNING && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	readDataMutex->Lock();

	samplesBufferMutex->Lock();

	size = samplesBuffer.Size() * (track.GetFormat().bits / 8);

	data.Resize(size);

	for (Int i = 0; i < samplesBuffer.Size(); i++)
	{
		if	(track.GetFormat().bits ==  8) data[i] = samplesBuffer[i] + 128;
		else if (track.GetFormat().bits == 16) ((Short *) (unsigned char *) data)[i] = samplesBuffer[i];
		else if (track.GetFormat().bits == 24) { data[3 * i] = samplesBuffer[i] & 255; data[3 * i + 1] = (samplesBuffer[i] >> 8) & 255; data[3 * i + 2] = (samplesBuffer[i] >> 16) & 255; }
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

	if (readData)
	{
		ex_FLAC__stream_decoder_seek_absolute(decoder, seekPosition);

		ex_FLAC__stream_decoder_process_until_end_of_stream(decoder);
	}

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

	filter->samplesBuffer.Resize(oSize + frame->header.blocksize * frame->header.channels);

	for (Int i = 0; i < signed(frame->header.blocksize); i++)
	{
		for (Int j = 0; j < signed(frame->header.channels); j++)
		{
			filter->samplesBuffer[oSize + i * signed(frame->header.channels) + j] = buffer[j][i];
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

	return (filter->stop || filter->driver->GetPos() == filter->driver->GetSize());
}

void BoCA::FLACStreamDecoderMetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	FLACIn	*filter = (FLACIn *) client_data;

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		Format	 format = filter->infoTrack->GetFormat();

		format.bits	= metadata->data.stream_info.bits_per_sample;
		format.order	= BYTE_INTEL;
		format.channels	= metadata->data.stream_info.channels;
		format.rate	= metadata->data.stream_info.sample_rate;

		filter->infoTrack->length = metadata->data.stream_info.total_samples;

		filter->infoTrack->SetFormat(format);
	}
	else if (metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
		if (metadata->data.vorbis_comment.num_comments > 0)
		{
			Buffer<UnsignedByte>	 vcBuffer(metadata->length);
			OutStream		 out(STREAM_BUFFER, vcBuffer, vcBuffer.Size());

			out.OutputNumber(metadata->data.vorbis_comment.vendor_string.length, 4);
			out.OutputData(metadata->data.vorbis_comment.vendor_string.entry, metadata->data.vorbis_comment.vendor_string.length);

			out.OutputNumber(metadata->data.vorbis_comment.num_comments, 4);

			for (UnsignedInt i = 0; i < metadata->data.vorbis_comment.num_comments; i++)
			{
				out.OutputNumber(metadata->data.vorbis_comment.comments[i].length, 4);
				out.OutputData(metadata->data.vorbis_comment.comments[i].entry, metadata->data.vorbis_comment.comments[i].length);			
			}

			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

			if (tagger != NIL)
			{
				tagger->ParseBuffer(vcBuffer, *filter->infoTrack);

				boca.DeleteComponent(tagger);
			}
		}
	}
	else if (metadata->type == FLAC__METADATA_TYPE_PICTURE && Config::Get()->GetIntValue("Tags", "CoverArtReadFromTags", True))
	{
		Picture	 picture;

		picture.type = metadata->data.picture.type;
		picture.mime = metadata->data.picture.mime_type;

		if	(picture.mime.ToLower() == "jpeg" || picture.mime.ToLower() == "jpg") picture.mime = "image/jpeg";
		else if (picture.mime.ToLower() == "png")				      picture.mime = "image/png";

		picture.description.ImportFrom("UTF-8", (char *) metadata->data.picture.description);

		picture.data.Set(metadata->data.picture.data, metadata->data.picture.data_length);

		filter->infoTrack->pictures.Add(picture);
	}
}

void BoCA::FLACStreamDecoderErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
}
