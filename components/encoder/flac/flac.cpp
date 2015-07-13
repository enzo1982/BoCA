 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>
#include <stdlib.h>

#include "flac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFLAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>FLAC Audio Encoder</name>					\
		    <version>1.0</version>						\
		    <id>flac-enc</id>							\
		    <type>encoder</type>						\
		    <format>								\
		      <name>FLAC Audio</name>						\
		      <lossless>true</lossless>						\
		      <extension>flac</extension>					\
		      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>		\
		    </format>								\
											\
		";

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>Ogg FLAC Files</name>				\
			      <lossless>true</lossless>					\
			      <extension>oga</extension>				\
			      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>	\
			    </format>							\
											\
			");
		}

		componentSpecs.Append("							\
											\
		  </component>								\
											\
		");
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
	FLAC__StreamEncoderReadStatus	 FLACStreamEncoderReadCallback(const FLAC__StreamEncoder *, FLAC__byte[], size_t *, void *);
	FLAC__StreamEncoderWriteStatus	 FLACStreamEncoderWriteCallback(const FLAC__StreamEncoder *, const FLAC__byte[], size_t, unsigned, unsigned, void *);
	FLAC__StreamEncoderSeekStatus	 FLACStreamEncoderSeekCallback(const FLAC__StreamEncoder *, FLAC__uint64, void *);
	FLAC__StreamEncoderTellStatus	 FLACStreamEncoderTellCallback(const FLAC__StreamEncoder *, FLAC__uint64 *, void *);
};

BoCA::EncoderFLAC::EncoderFLAC()
{
	configLayer  = NIL;

	encoder	     = NIL;

	bytesWritten = 0;
}

BoCA::EncoderFLAC::~EncoderFLAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFLAC::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	const Config	*config = GetConfiguration();

	srand(clock());

	encoder = ex_FLAC__stream_encoder_new();

	Buffer<unsigned char>	 vcBuffer;

	if (((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
	     (info.artist != NIL || info.title != NIL)) && config->GetIntValue("Tags", "EnableFLACMetadata", True))
	{
		FLAC__StreamMetadata	*vorbiscomment = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

		metadata.Add(vorbiscomment);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			/* Disable writing cover art to Vorbis comment tags for FLAC files.
			 */
			Config	*taggerConfig = Config::Copy(config);

			taggerConfig->SetIntValue("Tags", "CoverArtWriteToVorbisComment", False);

			tagger->SetConfiguration(taggerConfig);
			tagger->SetVendorString(*ex_FLAC__VENDOR_STRING);

			tagger->RenderBuffer(vcBuffer, track);

			boca.DeleteComponent(tagger);

			Config::Free(taggerConfig);
		}

		/* Process output comment tag and add it to FLAC metadata.
		 */
		InStream	in(STREAM_BUFFER, vcBuffer, vcBuffer.Size());

		in.RelSeek(in.InputNumber(4));

		Int	 numComments = in.InputNumber(4);

		for (Int i = 0; i < numComments; i++)
		{
			FLAC__StreamMetadata_VorbisComment_Entry	 entry;

			entry.length = in.InputNumber(4);
			entry.entry  = vcBuffer + in.GetPos();

			ex_FLAC__metadata_object_vorbiscomment_append_comment(vorbiscomment, entry, true);

			in.RelSeek(entry.length);
		}

		vorbiscomment->length = vcBuffer.Size();
	}

	if (config->GetIntValue("Tags", "CoverArtWriteToTags", True) && config->GetIntValue("Tags", "CoverArtWriteToFLACMetadata", True))
	{
		for (Int i = 0; i < track.pictures.Length(); i++)
		{
			FLAC__StreamMetadata	*picture = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PICTURE);
			const Picture		&picInfo = track.pictures.GetNth(i);

			metadata.Add(picture);

			if (picInfo.description != NIL) ex_FLAC__metadata_object_picture_set_description(picture, (FLAC__byte *) picInfo.description.ConvertTo("UTF-8"), true);

			ex_FLAC__metadata_object_picture_set_mime_type(picture, picInfo.mime, true);
			ex_FLAC__metadata_object_picture_set_data(picture, const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data), picInfo.data.Size(), true);

			picture->data.picture.type = (FLAC__StreamMetadata_Picture_Type) picInfo.type;
		}
	}

	FLAC__StreamMetadata	*padding = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING);

	padding->length = 4096;

	metadata.Add(padding);

	/* Put metadata in an array and hand it to the encoder.
	 */
	{
		FLAC__StreamMetadata	**metadataArray = new FLAC__StreamMetadata * [metadata.Length()];

		for (Int i = 0; i < metadata.Length(); i++) metadataArray[i] = metadata.GetNth(i);

		ex_FLAC__stream_encoder_set_metadata(encoder, metadataArray, metadata.Length());

		delete [] metadataArray;
	}

	ex_FLAC__stream_encoder_set_channels(encoder, format.channels);
	ex_FLAC__stream_encoder_set_sample_rate(encoder, format.rate);
	ex_FLAC__stream_encoder_set_bits_per_sample(encoder, format.bits == 32 ? 24 : format.bits);

	if (config->GetIntValue("FLAC", "Preset", 5) < 0)
	{
		ex_FLAC__stream_encoder_set_streamable_subset(encoder, config->GetIntValue("FLAC", "StreamableSubset", 1));
		ex_FLAC__stream_encoder_set_do_mid_side_stereo(encoder, config->GetIntValue("FLAC", "DoMidSideStereo", 1));
		ex_FLAC__stream_encoder_set_loose_mid_side_stereo(encoder, config->GetIntValue("FLAC", "LooseMidSideStereo", 0));
		ex_FLAC__stream_encoder_set_blocksize(encoder, config->GetIntValue("FLAC", "Blocksize", 4608));
		ex_FLAC__stream_encoder_set_apodization(encoder, config->GetStringValue("FLAC", "Apodization", "tukey(0.5)"));
		ex_FLAC__stream_encoder_set_max_lpc_order(encoder, config->GetIntValue("FLAC", "MaxLPCOrder", 8));
		ex_FLAC__stream_encoder_set_qlp_coeff_precision(encoder, config->GetIntValue("FLAC", "QLPCoeffPrecision", 0));
		ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search(encoder, config->GetIntValue("FLAC", "DoQLPCoeffPrecSearch", 0));
		ex_FLAC__stream_encoder_set_do_exhaustive_model_search(encoder, config->GetIntValue("FLAC", "DoExhaustiveModelSearch", 0));
		ex_FLAC__stream_encoder_set_min_residual_partition_order(encoder, config->GetIntValue("FLAC", "MinResidualPartitionOrder", 0));
		ex_FLAC__stream_encoder_set_max_residual_partition_order(encoder, config->GetIntValue("FLAC", "MaxResidualPartitionOrder", 5));
	}
	else
	{
		ex_FLAC__stream_encoder_set_streamable_subset(encoder, true);
		ex_FLAC__stream_encoder_set_compression_level(encoder, config->GetIntValue("FLAC", "Preset", 5));

		if (config->GetIntValue("FLAC", "Preset", 5) < 3) ex_FLAC__stream_encoder_set_blocksize(encoder, 1152);
		else						  ex_FLAC__stream_encoder_set_blocksize(encoder, 4608);
	}

	bytesWritten = 0;

	/* Init encoder and check status.
	 */
	FLAC__StreamEncoderInitStatus	 status = FLAC__STREAM_ENCODER_INIT_STATUS_OK;

	if (config->GetIntValue("FLAC", "FileFormat", 0) == 0 || *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0)
	{
		status = ex_FLAC__stream_encoder_init_stream(encoder, &FLACStreamEncoderWriteCallback, &FLACStreamEncoderSeekCallback, &FLACStreamEncoderTellCallback, NIL, this);
	}
	else
	{
		ex_FLAC__stream_encoder_set_ogg_serial_number(encoder, rand());

		status = ex_FLAC__stream_encoder_init_ogg_stream(encoder, &FLACStreamEncoderReadCallback, &FLACStreamEncoderWriteCallback, &FLACStreamEncoderSeekCallback, &FLACStreamEncoderTellCallback, NIL, this);
	}

	if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
	{
		errorString = "Could not initialize FLAC encoder! Please check the configuration!";
		errorState  = True;

		ex_FLAC__stream_encoder_delete(encoder);

		return False;
	}

	return True;
}

Bool BoCA::EncoderFLAC::Deactivate()
{
	ex_FLAC__stream_encoder_finish(encoder);
	ex_FLAC__stream_encoder_delete(encoder);

	for (Int i = 0; i < metadata.Length(); i++) ex_FLAC__metadata_object_delete(metadata.GetNth(i));

	/* Fix chapter marks in Vorbis Comments.
	 */
	FixChapterMarks();

	return True;
}

Int BoCA::EncoderFLAC::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	bytesWritten = 0;

	/* Convert samples to encoder input format.
	 */
	const Format	&format = track.GetFormat();

	buffer.Resize(size / (format.bits / 8));

	for (Int i = 0; i < size / (format.bits / 8); i++)
	{
		if	(format.bits ==  8				) buffer[i] =				   data [i] - 128;
		else if (format.bits == 16				) buffer[i] = ((Short *) (unsigned char *) data)[i];
		else if (format.bits == 32				) buffer[i] = ((Int32 *) (unsigned char *) data)[i] / 256;

		else if (format.bits == 24 && endianness == EndianLittle) buffer[i] = (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 256;
		else if (format.bits == 24 && endianness == EndianBig	) buffer[i] = (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 256;
	}

	ex_FLAC__stream_encoder_process_interleaved(encoder, buffer, size / (format.bits / 8) / format.channels);

	return bytesWritten;
}

Bool BoCA::EncoderFLAC::FixChapterMarks()
{
	if (track.tracks.Length() == 0 || !GetConfiguration()->GetIntValue("Tags", "WriteChapters", True)) return True;

	const Config	*config = GetConfiguration();

	/* Fill buffer with Vorbis Comment block.
	 */
	Buffer<UnsignedByte>	 buffer;
	Int			 position = 0;
	ogg_page		 og	  = { 0 };

	if (config->GetIntValue("FLAC", "FileFormat", 0) == 0 || *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0)
	{
		driver->Seek(4);

		/* Find Vorbis Comment block and read it into buffer.
		 */
		UnsignedByte	 header[4] = { 0 };

		while (!(header[0] & 0x80))
		{
			driver->ReadData(header, 4);

			Int	 size = header[1] << 16 | header[2] << 8 | header[3];

			if ((header[0] & 0x7F) == 4)
			{
				buffer.Resize(size);
				position = driver->GetPos();

				driver->ReadData(buffer, size);

				break;
			}

			driver->Seek(driver->GetPos() + size);
		}
	}
	else if (ex_ogg_page_checksum_set != NIL)
	{
		driver->Seek(0);

		/* Skip first Ogg page and read second into buffer.
		 */
		for (Int i = 0; i < 2; i++)
		{
			driver->Seek(driver->GetPos() + 26);

			Int		 dataSize    = 0;
			UnsignedByte	 segments    = 0;
			UnsignedByte	 segmentSize = 0;

			driver->ReadData(&segments, 1);

			for (Int i = 0; i < segments; i++) { driver->ReadData(&segmentSize, 1); dataSize += segmentSize; }

			buffer.Resize(27 + segments + dataSize);
			position = driver->GetPos() - segments - 27;

			driver->Seek(position);
			driver->ReadData(buffer, buffer.Size());

			og.header     = buffer;
			og.header_len = 27 + segments;
			og.body	      = buffer + og.header_len;
			og.body_len   = dataSize;
		}
	}

	/* Update chapter marks.
	 */
	if (buffer.Size() > 0)
	{
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Format	&chapterFormat = chapterTrack.GetFormat();

			for (Int b = 0; b < buffer.Size() - 23; b++)
			{
				if (buffer[b + 0] != 'C' || buffer[b + 1] != 'H' || buffer[b + 2] != 'A' || buffer[b +  3] != 'P' ||
				    buffer[b + 4] != 'T' || buffer[b + 5] != 'E' || buffer[b + 6] != 'R' || buffer[b + 10] != '=') continue;

				String	 id;

				id[0] = buffer[b + 7];
				id[1] = buffer[b + 8];
				id[2] = buffer[b + 9];

				if (id.ToInt() != i + 1) continue;

				String	 value	= String(offset / chapterFormat.rate / 60 / 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 / 60)).Append(":")
						 .Append(offset / chapterFormat.rate / 60 % 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 % 60)).Append(":")
						 .Append(offset / chapterFormat.rate % 60      < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate % 60)).Append(".")
						 .Append(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) < 100 ?
							(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) <  10 ?  "00" : "0") : "").Append(String::FromInt(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate)));

				for (Int p = 0; p < 12; p++) buffer[b + 11 + p] = value[p];

				break;
			}

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
		}

		/* Write page back to file.
		 */
		if (config->GetIntValue("FLAC", "FileFormat", 0) == 1 && *ex_FLAC_API_SUPPORTS_OGG_FLAC == 1) ex_ogg_page_checksum_set(&og);

		driver->Seek(position);
		driver->WriteData(buffer, buffer.Size());
	}

	driver->Seek(driver->GetSize());

	return True;
}

String BoCA::EncoderFLAC::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	switch (config->GetIntValue("FLAC", "FileFormat", 0))
	{
		default:
		case  0: return "flac";
		case  1: return *ex_FLAC_API_SUPPORTS_OGG_FLAC == 1 ? "oga" : "flac";
	}
}

ConfigLayer *BoCA::EncoderFLAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFLAC();

	return configLayer;
}

FLAC__StreamEncoderReadStatus BoCA::FLACStreamEncoderReadCallback(const FLAC__StreamEncoder *encoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
	EncoderFLAC	*filter = (EncoderFLAC *) client_data;

	*bytes = filter->driver->ReadData((UnsignedByte *) buffer, *bytes);

	return FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE;
}

FLAC__StreamEncoderWriteStatus BoCA::FLACStreamEncoderWriteCallback(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[], size_t bytes, unsigned samples, unsigned current_frame, void *client_data)
{
	EncoderFLAC	*filter = (EncoderFLAC *) client_data;

	filter->driver->WriteData((UnsignedByte *) buffer, bytes);
	filter->bytesWritten += bytes;

	return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
}

FLAC__StreamEncoderSeekStatus BoCA::FLACStreamEncoderSeekCallback(const FLAC__StreamEncoder *encoder, FLAC__uint64 absolute_byte_offset, void *client_data)
{
	EncoderFLAC	*filter = (EncoderFLAC *) client_data;

	filter->driver->Seek(absolute_byte_offset);

	return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
}

FLAC__StreamEncoderTellStatus BoCA::FLACStreamEncoderTellCallback(const FLAC__StreamEncoder *encoder, FLAC__uint64 *absolute_byte_offset, void *client_data)
{
	EncoderFLAC	*filter = (EncoderFLAC *) client_data;

	*absolute_byte_offset = filter->driver->GetPos();

	return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
}
