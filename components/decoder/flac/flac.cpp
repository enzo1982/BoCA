 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

#include <smooth/io/drivers/driver_ansi.h>

#include "flac.h"

using namespace smooth::IO;
using namespace smooth::Threads;

const String &BoCA::DecoderFLAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "								\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>FLAC Audio Decoder %VERSION%</name>					\
		    <version>1.0</version>							\
		    <id>flac-dec</id>								\
		    <type>decoder</type>							\
		    <format>									\
		      <name>FLAC Files</name>							\
		      <lossless>true</lossless>							\
		      <extension>flac</extension>						\
		      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>			\
		    </format>									\
												\
		";

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1 && oggdll != NIL)
		{
			componentSpecs.Append("							\
												\
			    <format>								\
			      <name>Ogg FLAC Files</name>					\
			      <lossless>true</lossless>						\
			      <extension>oga</extension>					\
			      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
			    </format>								\
												\
			");
		}

		componentSpecs.Append("								\
												\
		  </component>									\
												\
		");

		componentSpecs.Replace("%VERSION%", String("v").Append(*ex_FLAC__VERSION_STRING));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
	LoadFLACDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
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

	/* Use 512kB IO buffer for reading FLAC files.
	 */
	static const Int		 flacStreamDecoderBufferSize = 512 * 1024;
};

Bool BoCA::DecoderFLAC::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	SkipID3v2Tag(in);

	/* Check file signature.
	 */
	String	 fileType = in.InputString(4);

	if	(fileType == "fLaC") return True;
	else if (fileType != "OggS") return False;

	if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 0 || oggdll == NIL) return False;

	in.RelSeek(-4);

	/* Check if Ogg FLAC.
	 */
	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	Bool	 result	     = False;
	Bool	 initialized = False;

	do
	{
		Int	 size	= Math::Min((Int64) 4096, in.Size() - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		if (ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

			initialized = True;

			ex_ogg_stream_pagein(&os, &og);

			if (ex_ogg_stream_packetout(&os, &op) == 1)
			{
				if (op.packet[0] == 0x7F &&
				    op.packet[1] == 'F' && op.packet[2] == 'L' && op.packet[3] == 'A' && op.packet[4] == 'C' &&
				    op.packet[5] == 0x01) result = True;

				break;
			}
		}
	}
	while (in.GetPos() < in.Size());

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return result;
}

Error BoCA::DecoderFLAC::GetStreamInfo(const String &streamURI, Track &track)
{
	DriverANSI	 ioDriver(File(streamURI), IS_READ);
	InStream	 in(STREAM_DRIVER, &ioDriver);

	SkipID3v2Tag(in);

	/* Set up members.
	 */
	track.fileSize	    = in.Size();

	infoTrack	    = &track;
	stop		    = False;
	finished	    = False;

	driver		    = &ioDriver;
	driver->Seek(in.GetPos());

	/* Read metadata.
	 */
	ReadFLAC(False);

	in.Close();
	ioDriver.Close();

	return Success();
}

BoCA::DecoderFLAC::DecoderFLAC() : samplesRequestedSignal(1), samplesAvailableSignal(1)
{
	infoTrack     = NIL;
	decoderThread = NIL;

	stop	      = False;
	finished      = False;

	seekPosition  = 0;
}

BoCA::DecoderFLAC::~DecoderFLAC()
{
}

Bool BoCA::DecoderFLAC::Activate()
{
	/* Skip headers.
	 */
	InStream	 in(STREAM_DRIVER, driver);

	SkipID3v2Tag(in);

	driver->Seek(in.GetPos());

	/* Prepare decoder.
	 */
	infoTrack     = new Track();
	decoderThread = NIL;

	stop	      = False;
	finished      = False;

	seekPosition  = 0;

	samplesAvailableSignal.Wait();

	return True;
}

Bool BoCA::DecoderFLAC::Deactivate()
{
	if (decoderThread != NIL)
	{
		stop = True;

		samplesRequestedSignal.Release();

		decoderThread->Wait();
	}

	delete infoTrack;

	return True;
}

Bool BoCA::DecoderFLAC::Seek(Int64 samplePosition)
{
	seekPosition = samplePosition;

	return True;
}

Int BoCA::DecoderFLAC::ReadData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (decoderThread == NIL) decoderThread = NonBlocking1<Bool>(&DecoderFLAC::ReadFLAC, this).Call(True);

	if (finished && samplesBuffer.Size() <= 0) return -1;

	samplesAvailableSignal.Wait();

	inBytes = driver->GetPos();

	/* Convert samples to target format.
	 */
	const Format	&format = track.GetFormat();
 
	data.Resize(samplesBuffer.Size() * (format.bits / 8));

	for (Int i = 0; i < samplesBuffer.Size(); i++)
	{
		if	(format.bits ==  8				) ((int8_t  *) (UnsignedByte *) data)[i] = samplesBuffer[i];
		else if (format.bits == 16				) ((int16_t *) (UnsignedByte *) data)[i] = samplesBuffer[i];

		else if (format.bits == 24 && endianness == EndianLittle) { data[3 * i + 2] = (samplesBuffer[i] >> 16) & 0xFF; data[3 * i + 1] = (samplesBuffer[i] >> 8) & 0xFF; data[3 * i    ] = samplesBuffer[i] & 0xFF; }
		else if (format.bits == 24 && endianness == EndianBig   ) { data[3 * i    ] = (samplesBuffer[i] >> 16) & 0xFF; data[3 * i + 1] = (samplesBuffer[i] >> 8) & 0xFF; data[3 * i + 2] = samplesBuffer[i] & 0xFF; }

		else if (format.bits == 32				) ((int32_t *) (UnsignedByte *) data)[i] = samplesBuffer[i];
	}

	samplesBuffer.Resize(0);

	samplesRequestedSignal.Release();

	return data.Size();
}

Int BoCA::DecoderFLAC::ReadFLAC(Bool readData)
{
	FLAC__StreamDecoder	*decoder = ex_FLAC__stream_decoder_new();

	if (!readData)
	{
		ex_FLAC__stream_decoder_set_metadata_respond(decoder, FLAC__METADATA_TYPE_VORBIS_COMMENT);
		ex_FLAC__stream_decoder_set_metadata_respond(decoder, FLAC__METADATA_TYPE_PICTURE);
	}

	/* Set larger read buffer size.
	 */
	if (readData && driver->IsBuffered()) driver->SetBufferSize(flacStreamDecoderBufferSize);

	/* Initialize decoder.
	 */
	char	 signature[5] = { 0, 0, 0, 0, 0 };

	driver->ReadData((UnsignedByte *) signature, 4);
	driver->Seek(driver->GetPos() - 4);

	if	(String(signature) == "fLaC") ex_FLAC__stream_decoder_init_stream(decoder, &FLACStreamDecoderReadCallback, &FLACStreamDecoderSeekCallback, &FLACStreamDecoderTellCallback, &FLACStreamDecoderLengthCallback, &FLACStreamDecoderEofCallback, &FLACStreamDecoderWriteCallback, &FLACStreamDecoderMetadataCallback, &FLACStreamDecoderErrorCallback, this);
	else if (String(signature) == "OggS") ex_FLAC__stream_decoder_init_ogg_stream(decoder, &FLACStreamDecoderReadCallback, &FLACStreamDecoderSeekCallback, &FLACStreamDecoderTellCallback, &FLACStreamDecoderLengthCallback, &FLACStreamDecoderEofCallback, &FLACStreamDecoderWriteCallback, &FLACStreamDecoderMetadataCallback, &FLACStreamDecoderErrorCallback, this);

	/* Process metadata and audio.
	 */
	ex_FLAC__stream_decoder_process_until_end_of_metadata(decoder);

	if (readData)
	{
		ex_FLAC__stream_decoder_seek_absolute(decoder, seekPosition);

		ex_FLAC__stream_decoder_process_until_end_of_stream(decoder);
	}

	/* Finish and free decoder.
	 */
	finished = True;

	samplesAvailableSignal.Release();

	ex_FLAC__stream_decoder_finish(decoder);
	ex_FLAC__stream_decoder_delete(decoder);

	/* Set chapter pictures.
	 */
	foreach (Track &track, infoTrack->tracks) track.pictures = infoTrack->pictures;

	return Success();
}

Bool BoCA::DecoderFLAC::SkipID3v2Tag(InStream &in)
{
	/* Check for an ID3v2 tag at the beginning of the file
	 * and skip it if it exists. EAC and possibly other
	 * programs write ID3 tags fo FLAC if misconfigured.
	 */
	if (in.InputString(3) == "ID3")
	{
		in.InputNumber(2); // ID3 version
		in.InputNumber(1); // Flags

		/* Read tag size as a 4 byte unsynchronized integer.
		 */
		Int	 tagSize = (in.InputNumber(1) << 21) +
				   (in.InputNumber(1) << 14) +
				   (in.InputNumber(1) <<  7) +
				   (in.InputNumber(1)      );

		in.RelSeek(tagSize);

		inBytes += (tagSize + 10);
	}
	else
	{
		in.Seek(0);
	}

	return True;
}

FLAC__StreamDecoderReadStatus BoCA::FLACStreamDecoderReadCallback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	if (filter->stop)
	{
	    *bytes = 0;

	    return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
	}

	*bytes = filter->driver->ReadData(buffer, *bytes);

	if (*bytes == 0) return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	else		 return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

FLAC__StreamDecoderWriteStatus BoCA::FLACStreamDecoderWriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	if (filter->stop) return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	filter->samplesRequestedSignal.Wait();

	Int	 oSize = filter->samplesBuffer.Size();

	filter->samplesBuffer.Resize(oSize + frame->header.blocksize * frame->header.channels);

	for (Int i = 0; i < signed(frame->header.blocksize); i++)
	{
		for (Int j = 0; j < signed(frame->header.channels); j++)
		{
			filter->samplesBuffer[oSize + i * signed(frame->header.channels) + j] = buffer[j][i];
		}
	}

	filter->samplesAvailableSignal.Release();

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

FLAC__StreamDecoderSeekStatus BoCA::FLACStreamDecoderSeekCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	filter->driver->Seek(absolute_byte_offset);

	return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
}

FLAC__StreamDecoderTellStatus BoCA::FLACStreamDecoderTellCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	*absolute_byte_offset = filter->driver->GetPos();

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
}

FLAC__StreamDecoderLengthStatus BoCA::FLACStreamDecoderLengthCallback(const FLAC__StreamDecoder *decoder, FLAC__uint64 *stream_length, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	*stream_length = filter->driver->GetSize();

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__bool BoCA::FLACStreamDecoderEofCallback(const FLAC__StreamDecoder *decoder, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;

	return (filter->stop || filter->driver->GetPos() == filter->driver->GetSize());
}

void BoCA::FLACStreamDecoderMetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	DecoderFLAC	*filter = (DecoderFLAC *) client_data;
	const Config	*config = filter->GetConfiguration();

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		Format	 format = filter->infoTrack->GetFormat();

		format.bits	= metadata->data.stream_info.bits_per_sample;
		format.channels	= metadata->data.stream_info.channels;
		format.rate	= metadata->data.stream_info.sample_rate;

		if (metadata->data.stream_info.total_samples > 0) filter->infoTrack->length	  = metadata->data.stream_info.total_samples;
		else						  filter->infoTrack->approxLength = filter->driver->GetSize() / (format.bits / 8) / format.channels * 1.5;

		for (Int i = 0; i < 16; i++) filter->infoTrack->md5.Append(Number((Int64) metadata->data.stream_info.md5sum[i]).ToHexString(2));

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
				tagger->SetConfiguration(filter->GetConfiguration());
				tagger->ParseBuffer(vcBuffer, *filter->infoTrack);

				boca.DeleteComponent(tagger);
			}
		}
	}
	else if (metadata->type == FLAC__METADATA_TYPE_PICTURE && config->GetIntValue("Tags", "CoverArtReadFromTags", True))
	{
		Picture	 picture;

		picture.type = metadata->data.picture.type;
		picture.mime = metadata->data.picture.mime_type;

		picture.description.ImportFrom("UTF-8", (char *) metadata->data.picture.description);

		picture.data.Set(metadata->data.picture.data, metadata->data.picture.data_length);

		if (picture.data.Size() >= 16)
		{
			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			if (picture.data[0] != 0 && picture.data[1] != 0) filter->infoTrack->pictures.Add(picture);
		}
	}
}

void BoCA::FLACStreamDecoderErrorCallback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
}
