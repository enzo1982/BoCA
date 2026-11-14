 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

#include <stdint.h>

#include "opus.h"
#include "config.h"

using namespace smooth::IO;

static Bool osceSupported = False;
static Bool bweSupported  = False;

const String &BoCA::DecoderOpus::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (opusdll != NIL && AS::Registry::Get().ComponentExists("ogg-mux"))
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Opus Audio Decoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>opus-dec</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Opus Audio</name>						\
		      <extension>opus</extension>					\
		      <extension>ogg</extension>					\
		      <extension>oga</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_opus_get_version_string()).Replace("libopus ", NIL)));

		/* Check availability of Opus Speech Coding Enhancement (OSCE).
		 */
		const Array<String>	&version = String(ex_opus_get_version_string()).Replace("libopus ", NIL).Explode(".");

		if (version.GetNth(0).ToInt() > 1 || (version.GetNth(0).ToInt() == 1 && version.GetNth(1).ToInt() >= 5)) osceSupported = True;

		if (version.GetNth(0).ToInt() > 1 || (version.GetNth(0).ToInt() == 1 && version.GetNth(1).ToInt() >= 6))
		{
			int		 error	 = OPUS_OK;
			OpusDecoder	*decoder = ex_opus_decoder_create(48000, 2, &error);

			if (error == OPUS_OK)
			{
				if (ex_opus_decoder_ctl(decoder, OPUS_SET_OSCE_BWE(1)) == OPUS_OK) bweSupported = True;

				ex_opus_decoder_destroy(decoder);
			}
		}
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOpusDLL();
}

Void smooth::DetachDLL()
{
	FreeOpusDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 maxFrameSize	  = 5760;
	const Int	 minOutputSamples = 4096;

	/* Opus header definition.
	 */
	struct OpusHeader
	{
		char		 codec_id[8];	  /**< MUST be "OpusHead" */
		uint8_t		 version_id;	  /**< Version number */
		uint8_t		 nb_channels;	  /**< Number of channels */
		uint16_t	 preskip;	  /**< Pre-skip */
		uint32_t	 sample_rate;	  /**< Input sample rate; informational only */
		int16_t		 output_gain;	  /**< Output gain to apply when decoding */
		uint8_t		 channel_mapping; /**< Channel mapping family */

		uint8_t		 nb_streams;	  /**< Stream count */
		uint8_t		 nb_coupled;	  /**< Two-channel stream count */
		uint8_t		 stream_map[255]; /**< Channel mapping */
	};
};

Bool BoCA::DecoderOpus::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	/* Check if Opus stream.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer == NIL) return False;

	Bool			 result	  = False;
	Buffer<UnsignedByte>	 packet;

	Int64			 position = 0;
	Int			 flags	  = 0;

	if (muxer->ReadPacket(packet, position, flags) >= 19 && memcmp(packet, "OpusHead", 8) == 0)
	{
		/* Check Opus version and number of channels.
		 */
		OpusHeader	*setup = (OpusHeader *) (UnsignedByte *) packet;

		if (setup->version_id >> 4 == 0 && setup->channel_mapping <= 1 && setup->nb_channels <= 8) result = True;
	}

	CloseMuxer(muxer);

	return result;
}

Error BoCA::DecoderOpus::GetStreamInfo(const String &streamURI, Track &track)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Get configuration.
	 */
	const Config	*config	     = GetConfiguration();

	Bool		 decodeFloat = config->GetIntValue(ConfigureOpus::ConfigID, "DecodeFloat", False);
	Int		 complexity  = config->GetIntValue(ConfigureOpus::ConfigID, "Complexity", 10);
	Bool		 enableBwe   = config->GetIntValue(ConfigureOpus::ConfigID, "BandwidthExtension", False);

	/* Open file.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer == NIL) return Error();

	/* Set up track format.
	 */
	Format	 format;

	format.bits  = 16;
	format.rate  = 48000;

	if (decodeFloat)
	{
		format.bits = 32;
		format.fp   = True;
	}

	track.fileSize = ioDriver.GetSize();

	/* Get stream format.
	 */
	OpusHeader		 setup;
	Buffer<UnsignedByte>	 comments;
	Buffer<UnsignedByte>	 packet;

	memset(&setup, 0, sizeof(setup));

	for (Int packetNum = 0; packetNum <= 3; packetNum++)
	{
		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(packet, position, flags);

		if (packetSize < 0) break;

		/* Found header packet.
		 */
		if (packetNum == 0)
		{
			memcpy(&setup, packet, Math::Min(packetSize, (Int) sizeof(OpusHeader)));

			if (endianness != EndianLittle)
			{
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.preskip, sizeof(setup.preskip));
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.sample_rate, sizeof(setup.sample_rate));
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.output_gain, sizeof(setup.output_gain));
			}

			format.channels = setup.nb_channels;

			if (setup.sample_rate != 0 && (!bweSupported || complexity < 4 || !enableBwe))
			{
				if	(setup.sample_rate <=  8000) format.rate =  8000;
				else if	(setup.sample_rate <= 12000) format.rate = 12000;
				else if	(setup.sample_rate <= 16000) format.rate = 16000;
				else if	(setup.sample_rate <= 24000) format.rate = 24000;
				else				     format.rate = 48000;
			}

			preSkip = setup.preskip;

			track.length = -1;
		}

		/* Found Vorbis Comment packet.
		 */
		if (packetNum == 1)
		{
			if (packetSize >= 8 && memcmp(packet, "OpusTags", 8) == 0)
			{
				comments.Resize(packetSize - 8);

				memcpy(comments, packet + 8, packetSize - 8);
			}
		}

		/* Found audio packet.
		 */
		if (packetNum == 3)
		{
			int		 error	    = OPUS_OK;
			unsigned char	 mapping[2] = { 0, 1 };

			OpusMSDecoder	*decoder    = NIL;

			if (setup.channel_mapping == 0) decoder = ex_opus_multistream_decoder_create(format.rate, setup.nb_channels, 1,		       setup.nb_channels - 1, mapping,		&error);
			else				decoder = ex_opus_multistream_decoder_create(format.rate, setup.nb_channels, setup.nb_streams, setup.nb_coupled,      setup.stream_map, &error);

			if (error == OPUS_OK)
			{
				Buffer<opus_int16>	 samples(maxFrameSize * format.channels);

				track.approxLength = track.fileSize / (packetSize * 1.05) * ex_opus_multistream_decode(decoder, packet, packetSize, samples, maxFrameSize, 0);

				ex_opus_multistream_decoder_destroy(decoder);
			}
		}
	}

	track.SetFormat(format);

	/* Find real length of stream.
	 */
	Int64	 duration = muxer->GetDuration();

	if (duration >= 0) track.length = (duration - preSkip) / (48000 / format.rate);

	CloseMuxer(muxer);

	/* Read tags.
	 */
	if (comments.Size() > 0)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetConfiguration(GetConfiguration());
			tagger->ParseBuffer(comments, track);

			boca.DeleteComponent(tagger);
		}
	}

	return Success();
}

BoCA::DecoderOpus::DecoderOpus()
{
	configLayer = NIL;

	muxer	    = NIL;
	decoder	    = NIL;

	preSkip	    = 0;
	preSkipLeft = 0;

	skipSamples = 0;
}

BoCA::DecoderOpus::~DecoderOpus()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderOpus::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Get configuration.
	 */
	const Config	*config	    = GetConfiguration();

	Int		 complexity = config->GetIntValue(ConfigureOpus::ConfigID, "Complexity", 10);
	Bool		 enableBwe  = config->GetIntValue(ConfigureOpus::ConfigID, "BandwidthExtension", False);

	/* Open Ogg stream.
	 */
	muxer = OpenMuxer(driver);

	if (muxer == NIL) return False;

	/* Parse header packets and init decoder.
	 */
	for (Int packetNum = 0; packetNum <= 1; packetNum++)
	{
		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(packetBuffer, position, flags);

		if (packetSize < 0 || (packetNum == 0 && packetSize < 19))
		{
			CloseMuxer(muxer);

			muxer = NIL;

			return False;
		}

		/* Found header packet.
		 */
		if (packetNum == 0)
		{
			OpusHeader	 setup;

			memset(&setup, 0, sizeof(setup));
			memcpy(&setup, packetBuffer, Math::Min(packetSize, (Int) sizeof(OpusHeader)));

			if (endianness != EndianLittle)
			{
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.preskip, sizeof(setup.preskip));
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.sample_rate, sizeof(setup.sample_rate));
				BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.output_gain, sizeof(setup.output_gain));
			}

			const Format	&format	    = track.GetFormat();

			int		 error	    = OPUS_OK;
			unsigned char	 mapping[2] = { 0, 1 };

			if (setup.channel_mapping == 0) decoder = ex_opus_multistream_decoder_create(format.rate, setup.nb_channels, 1,		       setup.nb_channels - 1, mapping,		&error);
			else				decoder = ex_opus_multistream_decoder_create(format.rate, setup.nb_channels, setup.nb_streams, setup.nb_coupled,      setup.stream_map, &error);

			preSkip	    = setup.preskip / (48000 / format.rate);
			preSkipLeft = setup.preskip / (48000 / format.rate);

			if (osceSupported && error == OPUS_OK)
			{
				for (Int i = 0; i < (setup.channel_mapping == 0 ? 1 : setup.nb_streams); i++)
				{
					OpusDecoder	*streamDecoder = NIL;

					ex_opus_multistream_decoder_ctl(decoder, OPUS_MULTISTREAM_GET_DECODER_STATE(i, &streamDecoder));

					ex_opus_decoder_ctl(streamDecoder, OPUS_SET_COMPLEXITY(complexity));
					ex_opus_decoder_ctl(streamDecoder, OPUS_SET_OSCE_BWE(enableBwe));
				}
			}
		}
	}

	inBytes = driver->GetPos();

	return True;
}

Bool BoCA::DecoderOpus::Deactivate()
{
	CloseMuxer(muxer);

	muxer = NIL;

	ex_opus_multistream_decoder_destroy(decoder);

	return True;
}

Bool BoCA::DecoderOpus::Seek(Int64 samplePosition)
{
	const Format	&format = track.GetFormat();

	/* Seek to the page containing the requested sample position.
	 */
	Int	 scale	  = 48000 / format.rate;
	Int64	 position = muxer->SeekPacket((samplePosition + preSkip + 1) * scale - 1);

	inBytes = driver->GetPos();

	if (position < 0) return False;

	skipSamples  = preSkip + samplePosition - position / scale;
	preSkipLeft += skipSamples;

	ex_opus_multistream_decoder_ctl(decoder, OPUS_RESET_STATE);

	return True;
}

Int BoCA::DecoderOpus::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	Int	 bytesPerSample = format.channels * (format.bits / 8);
	Int	 size		= 0;

	/* Decode packets until we have enough samples.
	 */
	while (size < minOutputSamples * bytesPerSample)
	{
		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(packetBuffer, position, flags);

		if (packetSize < 0) break;

		data.Resize(size + maxFrameSize * bytesPerSample);

		Int	 frameSize = 0;

		if (format.bits == 16) frameSize = ex_opus_multistream_decode(decoder, packetBuffer, packetSize, (opus_int16 *) (UnsignedByte *) (data + size), maxFrameSize, 0);
		else		       frameSize = ex_opus_multistream_decode_float(decoder, packetBuffer, packetSize, (float *) (UnsignedByte *) (data + size), maxFrameSize, 0);

		if (frameSize > preSkipLeft)
		{
			if (preSkipLeft) memmove((unsigned char *) data + size, (unsigned char *) data + size + preSkipLeft * bytesPerSample, (frameSize - preSkipLeft) * bytesPerSample);

			size += Math::Max(0, (frameSize - preSkipLeft) * bytesPerSample);
		}

		preSkipLeft = Math::Max(0, preSkipLeft - frameSize);
	}

	inBytes = driver->GetPos();

	/* Bail out if no more data available.
	 */
	if (size == 0) return -1;

	data.Resize(size);

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_7_1, Channel::Default_7_1);

	return size;
}

BoCA::AS::MuxerComponent *BoCA::DecoderOpus::OpenMuxer(Driver *ioDriver) const
{
	/* Create Ogg muxer and open stream.
	 */
	AS::Registry		&boca  = AS::Registry::Get();
	AS::MuxerComponent	*muxer = (AS::MuxerComponent *) boca.CreateComponentByID("ogg-mux");

	if (muxer == NIL) return NIL;

	muxer->SetConfiguration(GetConfiguration());
	muxer->SetDriver(ioDriver);

	if (!muxer->Open(MUXER_MODE_READ))
	{
		boca.DeleteComponent(muxer);

		return NIL;
	}

	return muxer;
}

Void BoCA::DecoderOpus::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}

ConfigLayer *BoCA::DecoderOpus::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureOpus(osceSupported, bweSupported);

	return configLayer;
}
