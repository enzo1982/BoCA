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

#include "flac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFLAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (flacdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>FLAC Audio Encoder %VERSION%</name>							\
		    <version>1.0</version>									\
		    <id>flac-enc</id>										\
		    <type>encoder</type>									\
		    <format>											\
		      <name>FLAC Files</name>									\
		      <lossless>true</lossless>									\
		      <extension>flac</extension>								\
		      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>					\
		    </format>											\
														\
		";

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
		{
			componentSpecs.Append("									\
														\
			    <format>										\
			      <name>Ogg FLAC Files</name>							\
			      <lossless>true</lossless>								\
			      <extension>oga</extension>							\
			      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>			\
			    </format>										\
														\
			");
		}

		componentSpecs.Append("										\
														\
		    <input bits=\"8-24\" channels=\"1-8\" rate=\"1-655350\"/>					\
		    <parameters>										\
														\
		");

		if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
		{
			componentSpecs.Append("									\
														\
		      <selection name=\"Format\" argument=\"-f %VALUE\" default=\"flac\">			\
			<option alias=\"FLAC\">flac</option>							\
			<option alias=\"Ogg FLAC\">ogg</option>							\
		      </selection>										\
														\
			");
		}

		componentSpecs.Append("										\
														\
		      <range name=\"Compression level\" argument=\"-c %VALUE\">					\
			<min alias=\"fastest\">0</min>								\
			<max alias=\"best\">8</max>								\
		      </range>											\
		      <range name=\"Block size\" argument=\"-b %VALUE\" default=\"4096\">			\
			<min>192</min>										\
			<max>32768</max>									\
		      </range>											\
		      <switch name=\"Use mid-side stereo\" argument=\"-m\"/>					\
		      <switch name=\"Do exhaustive model search\" argument=\"-e\"/>				\
		      <range name=\"Max LPC order\" argument=\"-l %VALUE\" default=\"8\">			\
			<min alias=\"disabled\">0</min>								\
			<max>").Append(String::FromInt(FLAC__MAX_LPC_ORDER)).Append("</max>			\
		      </range>											\
		      <switch name=\"Do exhaustive QLP coefficient search\" argument=\"-p\"/>			\
		      <range name=\"QLP coefficient precision\" argument=\"-q %VALUE\" default=\"0 (auto)\">	\
			<min>").Append(String::FromInt(FLAC__MIN_QLP_COEFF_PRECISION)).Append("</min>		\
			<max>").Append(String::FromInt(FLAC__MAX_QLP_COEFF_PRECISION)).Append("</max>		\
		      </range>											\
		      <range name=\"Maximum rice partition order\" argument=\"-r %VALUE\" default=\"5\">	\
			<min alias=\"auto\">0</min>								\
			<max>").Append(String::FromInt(FLAC__MAX_RICE_PARTITION_ORDER)).Append("</max>		\
		      </range>											\
		    </parameters>										\
		  </component>											\
														\
		");

		componentSpecs.Replace("%VERSION%", String("v").Append(*ex_FLAC__VERSION_STRING));
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
	FLAC__StreamEncoderReadStatus	 FLACStreamEncoderReadCallback(const FLAC__StreamEncoder *, FLAC__byte[], size_t *, void *);
	FLAC__StreamEncoderWriteStatus	 FLACStreamEncoderWriteCallback(const FLAC__StreamEncoder *, const FLAC__byte[], size_t, uint32_t, uint32_t, void *);
	FLAC__StreamEncoderSeekStatus	 FLACStreamEncoderSeekCallback(const FLAC__StreamEncoder *, FLAC__uint64, void *);
	FLAC__StreamEncoderTellStatus	 FLACStreamEncoderTellCallback(const FLAC__StreamEncoder *, FLAC__uint64 *, void *);

	/* Use 512kB IO buffer for writing FLAC files.
	 */
	static const Int		 flacStreamEncoderBufferSize = 512 * 1024;
};

BoCA::EncoderFLAC::EncoderFLAC()
{
	configLayer  = NIL;
	config	     = NIL;

	encoder	     = NIL;

	bytesWritten = 0;
}

BoCA::EncoderFLAC::~EncoderFLAC()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFLAC::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	/* Create FLAC encoder.
	 */
	Math::RandomSeed();

	encoder = ex_FLAC__stream_encoder_new();

	/* Add Vorbis Comment metadata object.
	 */
	Buffer<unsigned char>	 vcBuffer;

	if (config->GetIntValue("Tags", "EnableFLACMetadata", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
	{
		FLAC__StreamMetadata	*vorbiscomment = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

		metadata.Add(vorbiscomment);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			/* Disable writing cover art to Vorbis Comment tags for FLAC files.
			 */
			Config	*taggerConfig = Config::Copy(config);

			taggerConfig->SetIntValue("Tags", "CoverArtWriteToVorbisComment", False);
			taggerConfig->SetIntValue("Tags", "TrackPrependZeroVorbisComment", config->GetIntValue("Tags", "TrackPrependZeroFLACMetadata", True));

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

	/* Add seektable metadata object.
	 */
	Int64	 numSamples = track.length >= 0 ? track.length : track.approxLength;

	if (numSamples >= 0)
	{
		FLAC__StreamMetadata	*seektable = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_SEEKTABLE);

		/* Set number of seek points and limit it to 230 for Ogg FLAC to fit in one Ogg page.
		 */
		Int	 numSeekPoints = Math::Min(1000, (Int) Math::Ceil(Float(numSamples) / format.rate / 10.0));

		if (config->GetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0) == 1 && *ex_FLAC_API_SUPPORTS_OGG_FLAC == 1) numSeekPoints = Math::Min(230, numSeekPoints);

		ex_FLAC__metadata_object_seektable_template_append_spaced_points(seektable, numSeekPoints, numSamples);
		ex_FLAC__metadata_object_seektable_template_sort(seektable, true);

		metadata.Add(seektable);
	}

	/* Add picture metadata object.
	 */
	if (config->GetIntValue("Tags", "EnableFLACMetadata", True) && config->GetIntValue("Tags", "CoverArtWriteToTags", True) && config->GetIntValue("Tags", "CoverArtWriteToFLACMetadata", True))
	{
		for (Int i = 0; i < track.pictures.Length(); i++)
		{
			FLAC__StreamMetadata	*picture = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PICTURE);
			const Picture		&picInfo = track.pictures.GetNth(i);

			metadata.Add(picture);

			if (picInfo.mime != NIL)	ex_FLAC__metadata_object_picture_set_mime_type(picture, picInfo.mime, true);
			if (picInfo.description != NIL) ex_FLAC__metadata_object_picture_set_description(picture, (FLAC__byte *) picInfo.description.Trim().ConvertTo("UTF-8"), true);

			ex_FLAC__metadata_object_picture_set_data(picture, const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data), picInfo.data.Size(), true);

			picture->data.picture.type = (FLAC__StreamMetadata_Picture_Type) picInfo.type;
		}
	}

	/* Add padding metadata object.
	 */
	FLAC__StreamMetadata	*padding = ex_FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING);

	padding->length = 8192;

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
	ex_FLAC__stream_encoder_set_bits_per_sample(encoder, format.bits);

	if (config->GetIntValue(ConfigureFLAC::ConfigID, "Preset", 5) < 0)
	{
		const String	&windowFunctions = config->GetStringValue(ConfigureFLAC::ConfigID, "Apodization", "tukey(0.5)");

		ex_FLAC__stream_encoder_set_streamable_subset(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "StreamableSubset", 1));
		ex_FLAC__stream_encoder_set_do_mid_side_stereo(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "DoMidSideStereo", 1));
		ex_FLAC__stream_encoder_set_loose_mid_side_stereo(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "LooseMidSideStereo", 0));
		ex_FLAC__stream_encoder_set_blocksize(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "Blocksize", 4096));
		ex_FLAC__stream_encoder_set_max_lpc_order(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "MaxLPCOrder", 8));
		ex_FLAC__stream_encoder_set_qlp_coeff_precision(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "QLPCoeffPrecision", 0));
		ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "DoQLPCoeffPrecSearch", 0));
		ex_FLAC__stream_encoder_set_do_exhaustive_model_search(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "DoExhaustiveModelSearch", 0));
		ex_FLAC__stream_encoder_set_min_residual_partition_order(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "MinResidualPartitionOrder", 0));
		ex_FLAC__stream_encoder_set_max_residual_partition_order(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "MaxResidualPartitionOrder", 5));

		if (windowFunctions != NIL) ex_FLAC__stream_encoder_set_apodization(encoder, windowFunctions);
	}
	else
	{
		ex_FLAC__stream_encoder_set_streamable_subset(encoder, true);
		ex_FLAC__stream_encoder_set_compression_level(encoder, config->GetIntValue(ConfigureFLAC::ConfigID, "Preset", 5));

		if (config->GetIntValue(ConfigureFLAC::ConfigID, "Preset", 5) < 3) ex_FLAC__stream_encoder_set_blocksize(encoder, 1152);
		else								   ex_FLAC__stream_encoder_set_blocksize(encoder, 4096);
	}

	bytesWritten = 0;

	/* Set larger write buffer size.
	 */
	if (driver->IsBuffered()) driver->SetBufferSize(flacStreamEncoderBufferSize);

	/* Init encoder and check status.
	 */
	FLAC__StreamEncoderInitStatus	 status = FLAC__STREAM_ENCODER_INIT_STATUS_OK;

	if (config->GetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0) == 0 || *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0)
	{
		status = ex_FLAC__stream_encoder_init_stream(encoder, &FLACStreamEncoderWriteCallback, &FLACStreamEncoderSeekCallback, &FLACStreamEncoderTellCallback, NIL, this);
	}
	else
	{
		ex_FLAC__stream_encoder_set_ogg_serial_number(encoder, Math::Random());

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
	/* Finish and free encoder.
	 */
	ex_FLAC__stream_encoder_finish(encoder);
	ex_FLAC__stream_encoder_delete(encoder);

	for (Int i = 0; i < metadata.Length(); i++) ex_FLAC__metadata_object_delete(metadata.GetNth(i));

	/* Fix chapter marks in Vorbis Comments.
	 */
	if (config->GetIntValue("Tags", "EnableFLACMetadata", True) && track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
	{
		driver->Close();

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = NIL;

		if (config->GetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0) == 0 || *ex_FLAC_API_SUPPORTS_OGG_FLAC == 0) tagger = (AS::TaggerComponent *) boca.CreateComponentByID("flac-tag");
		else													       tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->UpdateStreamInfo(track.outputFile, track);

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderFLAC::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	bytesWritten = 0;

	/* Convert samples to encoder input format.
	 */
	const Format	&format = track.GetFormat();

	buffer.Resize(data.Size() / (format.bits / 8));

	for (Int i = 0; i < data.Size() / (format.bits / 8); i++)
	{
		if	(format.bits ==  8				) buffer[i] = ((int8_t	*) (UnsignedByte *) data)[i];
		else if (format.bits == 16				) buffer[i] = ((int16_t *) (UnsignedByte *) data)[i];

		else if (format.bits == 24 && endianness == EndianLittle) buffer[i] = (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 256;
		else if (format.bits == 24 && endianness == EndianBig	) buffer[i] = (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 256;
	}

	ex_FLAC__stream_encoder_process_interleaved(encoder, buffer, data.Size() / (format.bits / 8) / format.channels);

	return bytesWritten;
}

Bool BoCA::EncoderFLAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigureFLAC::ConfigID, "FileFormat", n);

	return True;
}

String BoCA::EncoderFLAC::GetOutputFileExtension() const
{
	String	 extension = "flac";

	if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 1)
	{
		Config	*config = Config::Copy(GetConfiguration());

		ConvertArguments(config);

		if (config->GetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0) == 1) extension = "oga";

		Config::Free(config);
	}

	return extension;
}

Bool BoCA::EncoderFLAC::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "flac-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0);
		config->SetIntValue(ConfigureFLAC::ConfigID, "Preset", -1);

		config->SetIntValue(ConfigureFLAC::ConfigID, "DoMidSideStereo", False);
		config->SetIntValue(ConfigureFLAC::ConfigID, "DoExhaustiveModelSearch", False);
		config->SetIntValue(ConfigureFLAC::ConfigID, "DoQLPCoeffPrecSearch", False);

		config->SetIntValue(ConfigureFLAC::ConfigID, "Blocksize", 4096);
		config->SetIntValue(ConfigureFLAC::ConfigID, "MaxLPCOrder", 8);
		config->SetIntValue(ConfigureFLAC::ConfigID, "QLPCoeffPrecision", 0);
		config->SetIntValue(ConfigureFLAC::ConfigID, "MinResidualPartitionOrder", 0);
		config->SetIntValue(ConfigureFLAC::ConfigID, "MaxResidualPartitionOrder", 5);
	}

	/* Get command line settings.
	 */
	Bool	 midSideStereo	  = config->GetIntValue(encoderID, "Use mid-side stereo", config->GetIntValue(ConfigureFLAC::ConfigID, "DoMidSideStereo", False));
	Bool	 exhaustiveSearch = config->GetIntValue(encoderID, "Do exhaustive model search", config->GetIntValue(ConfigureFLAC::ConfigID, "DoExhaustiveModelSearch", False));
	Bool	 qlpCoeffSearch   = config->GetIntValue(encoderID, "Do exhaustive QLP coefficient search", config->GetIntValue(ConfigureFLAC::ConfigID, "DoQLPCoeffPrecSearch", False));

	String	 format		  = config->GetIntValue(ConfigureFLAC::ConfigID, "FileFormat", 0) == 0 ? "flac" : "ogg";
	Int	 preset		  = config->GetIntValue(ConfigureFLAC::ConfigID, "Preset", -1);
	Int	 blocksize	  = config->GetIntValue(ConfigureFLAC::ConfigID, "Blocksize", 4096);
	Int	 lpc		  = config->GetIntValue(ConfigureFLAC::ConfigID, "MaxLPCOrder", 8);
	Int	 qlp		  = config->GetIntValue(ConfigureFLAC::ConfigID, "QLPCoeffPrecision", 0);
	Int	 rice		  = config->GetIntValue(ConfigureFLAC::ConfigID, "MaxResidualPartitionOrder", 5);

	if (config->GetIntValue(encoderID, "Set Format", False))		       format	 = config->GetStringValue(encoderID, "Format", format).ToLower();
	if (config->GetIntValue(encoderID, "Set Compression level", False))	       preset	 = config->GetIntValue(encoderID, "Compression level", preset);
	if (config->GetIntValue(encoderID, "Set Block size", False))		       blocksize = config->GetIntValue(encoderID, "Block size", blocksize);
	if (config->GetIntValue(encoderID, "Set Max LPC order", False))		       lpc	 = config->GetIntValue(encoderID, "Max LPC order", lpc);
	if (config->GetIntValue(encoderID, "Set QLP coefficient precision", False))    qlp	 = config->GetIntValue(encoderID, "QLP coefficient precision", qlp);
	if (config->GetIntValue(encoderID, "Set Maximum rice partition order", False)) rice	 = config->GetIntValue(encoderID, "Maximum rice partition order", rice);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureFLAC::ConfigID, "FileFormat", format == "ogg");
	config->SetIntValue(ConfigureFLAC::ConfigID, "Preset", preset);

	config->SetIntValue(ConfigureFLAC::ConfigID, "DoMidSideStereo", midSideStereo);
	config->SetIntValue(ConfigureFLAC::ConfigID, "DoExhaustiveModelSearch", exhaustiveSearch);
	config->SetIntValue(ConfigureFLAC::ConfigID, "DoQLPCoeffPrecSearch", qlpCoeffSearch);

	config->SetIntValue(ConfigureFLAC::ConfigID, "Blocksize", Math::Max(192, Math::Min(32768, blocksize)));
	config->SetIntValue(ConfigureFLAC::ConfigID, "MaxLPCOrder", Math::Max(0, Math::Min(FLAC__MAX_LPC_ORDER, lpc)));
	config->SetIntValue(ConfigureFLAC::ConfigID, "QLPCoeffPrecision", qlp == 0 ? 0 : Math::Max(FLAC__MIN_QLP_COEFF_PRECISION, Math::Min(FLAC__MAX_QLP_COEFF_PRECISION, qlp)));
	config->SetIntValue(ConfigureFLAC::ConfigID, "MaxResidualPartitionOrder", Math::Max(0, Math::Min(FLAC__MAX_RICE_PARTITION_ORDER, rice)));

	return True;
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

FLAC__StreamEncoderWriteStatus BoCA::FLACStreamEncoderWriteCallback(const FLAC__StreamEncoder *encoder, const FLAC__byte buffer[], size_t bytes, uint32_t samples, uint32_t current_frame, void *client_data)
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
