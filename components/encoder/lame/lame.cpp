 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

#include "lame.h"
#include "config.h"

const String &BoCA::EncoderLAME::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (lamedll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>LAME MP3 Encoder %VERSION%</name>			\
		    <version>1.0</version>					\
		    <id>lame-enc</id>						\
		    <type>encoder</type>					\
		    <format>							\
		      <name>MPEG 1 Audio Layer 3</name>				\
		      <extension>mp3</extension>				\
		      <tag id=\"id3v1-tag\" mode=\"append\">ID3v1</tag>		\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		  </component>							\
										\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_get_lame_short_version()));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadLAMEDLL();
}

Void smooth::DetachDLL()
{
	FreeLAMEDLL();
}

BoCA::EncoderLAME::EncoderLAME()
{
	configLayer = NIL;

	context	    = NIL;

	dataOffset  = 0;
}

BoCA::EncoderLAME::~EncoderLAME()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderLAME::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Int	 preset		  = config->GetIntValue(ConfigureLAME::ConfigID, "Preset", 2);
	Int	 resample	  = config->GetIntValue(ConfigureLAME::ConfigID, "Resample", -1);
	Int	 vbrMode	  = config->GetIntValue(ConfigureLAME::ConfigID, "VBRMode", 4);
	Bool	 setBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "SetBitrate", 1);
	Int	 bitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "Bitrate", 192);
	Int	 ratio		  = config->GetIntValue(ConfigureLAME::ConfigID, "Ratio", 1100);
	Bool	 setQuality	  = config->GetIntValue(ConfigureLAME::ConfigID, "SetQuality", 0);
	Int	 quality	  = config->GetIntValue(ConfigureLAME::ConfigID, "Quality", 3);
	Int	 abrBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "ABRBitrate", 192);
	Int	 vbrQuality	  = config->GetIntValue(ConfigureLAME::ConfigID, "VBRQuality", 50);
	Bool	 setMinVBRBitrate = config->GetIntValue(ConfigureLAME::ConfigID, "SetMinVBRBitrate", 0);
	Bool	 setMaxVBRBitrate = config->GetIntValue(ConfigureLAME::ConfigID, "SetMaxVBRBitrate", 0);
	Int	 minVBRBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "MinVBRBitrate", 128);
	Int	 maxVBRBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "MaxVBRBitrate", 256);
	Bool	 copyrightBit	  = config->GetIntValue(ConfigureLAME::ConfigID, "Copyright", 0);
	Bool	 originalBit	  = config->GetIntValue(ConfigureLAME::ConfigID, "Original", 1);
	Bool	 privateBit	  = config->GetIntValue(ConfigureLAME::ConfigID, "Private", 0);
	Bool	 crc		  = config->GetIntValue(ConfigureLAME::ConfigID, "CRC", 0);
	Bool	 strictISO	  = config->GetIntValue(ConfigureLAME::ConfigID, "StrictISO", 0);
	Int	 stereoMode	  = config->GetIntValue(ConfigureLAME::ConfigID, "StereoMode", 0);
	Bool	 forceJS	  = config->GetIntValue(ConfigureLAME::ConfigID, "ForceJS", 0);
	Bool	 enableATH	  = config->GetIntValue(ConfigureLAME::ConfigID, "EnableATH", 1);
	Int	 athType	  = config->GetIntValue(ConfigureLAME::ConfigID, "ATHType", -1);
	Bool	 useTNS		  = config->GetIntValue(ConfigureLAME::ConfigID, "UseTNS", 1);
	Bool	 disableFiltering = config->GetIntValue(ConfigureLAME::ConfigID, "DisableFiltering", 0);
	Bool	 setLowpass	  = config->GetIntValue(ConfigureLAME::ConfigID, "SetLowpass", 0);
	Int	 lowpass	  = config->GetIntValue(ConfigureLAME::ConfigID, "Lowpass", 0);
	Bool	 setHighpass	  = config->GetIntValue(ConfigureLAME::ConfigID, "SetHighpass", 0);
	Int	 highpass	  = config->GetIntValue(ConfigureLAME::ConfigID, "Highpass", 0);
	Bool	 setLowpassWidth  = config->GetIntValue(ConfigureLAME::ConfigID, "SetLowpassWidth", 0);
	Int	 lowpassWidth	  = config->GetIntValue(ConfigureLAME::ConfigID, "LowpassWidth", 0);
	Bool	 setHighpassWidth = config->GetIntValue(ConfigureLAME::ConfigID, "SetHighpassWidth", 0);
	Int	 highpassWidth	  = config->GetIntValue(ConfigureLAME::ConfigID, "HighpassWidth", 0);

	/* Check settings.
	 */
	const Format	&format	  = track.GetFormat();
	const Info	&info	  = track.GetInfo();

	if (resample >= 0)
	{
		Int	 effectiveRate = (resample != 0 ? resample : format.rate);

		switch (effectiveRate)
		{
			case 8000:
			case 11025:
			case 12000:
			case 16000:
			case 22050:
			case 24000:
				if (setBitrate && vbrMode == vbr_off && (bitrate == 192 || bitrate == 224 || bitrate == 256 || bitrate == 320))
				{
					errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}

				if (setMinVBRBitrate && vbrMode != vbr_off && (minVBRBitrate == 192 || minVBRBitrate == 224 || minVBRBitrate == 256 || minVBRBitrate == 320))
				{
					errorString = "Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}

				if (setMaxVBRBitrate && vbrMode != vbr_off && (maxVBRBitrate == 192 || maxVBRBitrate == 224 || maxVBRBitrate == 256 || maxVBRBitrate == 320))
				{
					errorString = "Bad maximum VBR bitrate! The selected maximum VBR bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}
				break;
			case 32000:
			case 44100:
			case 48000:
				if (setBitrate && vbrMode == vbr_off && (bitrate == 8 || bitrate == 16 || bitrate == 24 || bitrate == 144))
				{
					errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}

				if (setMinVBRBitrate && vbrMode != vbr_off && (minVBRBitrate == 8 || minVBRBitrate == 16 || minVBRBitrate == 24 || minVBRBitrate == 144))
				{
					errorString = "Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}

				if (setMaxVBRBitrate && vbrMode != vbr_off && (maxVBRBitrate == 8 || maxVBRBitrate == 16 || maxVBRBitrate == 24 || maxVBRBitrate == 144))
				{
					errorString = "Bad maximum VBR bitrate! The selected maximum VBR bitrate is not supported for this sampling rate.";
					errorState  = True;

					return False;
				}
				break;
			default:
				errorString = "Bad sampling rate! The selected sampling rate is not supported.";
				errorState  = True;

				return False;
		}
	}

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	outBuffer.Resize(131072);

	/* Create and configure LAME encoder.
	 */
	context = ex_lame_init();

	ex_lame_set_in_samplerate(context, format.rate);
	ex_lame_set_num_channels(context, format.channels);

	switch (preset)
	{
		case 0:
			ex_lame_set_copyright(context, copyrightBit);
			ex_lame_set_original(context, originalBit);
			ex_lame_set_extension(context, privateBit);
			ex_lame_set_error_protection(context, crc);
			ex_lame_set_strict_ISO(context, strictISO);

			/* Set resampling.
			 */
			if	(resample == 0) ex_lame_set_out_samplerate(context, format.rate);
			else if (resample >  0) ex_lame_set_out_samplerate(context, resample);

			/* Set bitrate.
			 */
			if (vbrMode == vbr_off)
			{
				if (setBitrate) ex_lame_set_brate(context, bitrate);
				else		ex_lame_set_compression_ratio(context, ratio / 100.0);
			}

			/* Set quality.
			 */
			if (setQuality) ex_lame_set_quality(context, quality);

			/* Set audio filtering.
			 */
			if (disableFiltering)
			{
				ex_lame_set_lowpassfreq(context, -1);
				ex_lame_set_highpassfreq(context, -1);
			}
			else
			{
				if (setLowpass)  ex_lame_set_lowpassfreq(context, lowpass);
				if (setHighpass) ex_lame_set_highpassfreq(context, highpass);

				if (setLowpass  && setLowpassWidth)  ex_lame_set_lowpasswidth(context, lowpassWidth);
				if (setHighpass && setHighpassWidth) ex_lame_set_highpasswidth(context, highpassWidth);
			}

			/* Set Stereo mode.
			 */
			if	(stereoMode == 1) ex_lame_set_mode(context, MONO);
			else if (stereoMode == 2) ex_lame_set_mode(context, STEREO);
			else if (stereoMode == 3) ex_lame_set_mode(context, JOINT_STEREO);
			else			  ex_lame_set_mode(context, NOT_SET);

			if (stereoMode == 3)
			{
				if (forceJS) ex_lame_set_force_ms(context, 1);
				else	     ex_lame_set_force_ms(context, 0);
			}

			/* Set VBR mode.
			 */
			switch (vbrMode)
			{
				default:
				case vbr_off:
					break;
				case vbr_abr:
					ex_lame_set_VBR(context, vbr_abr);
					ex_lame_set_VBR_mean_bitrate_kbps(context, abrBitrate);
					break;
				case vbr_rh:
					ex_lame_set_VBR(context, vbr_rh);
					ex_lame_set_VBR_quality(context, vbrQuality / 10.0);
					break;
				case vbr_mtrh:
					ex_lame_set_VBR(context, vbr_mtrh);
					ex_lame_set_VBR_quality(context, vbrQuality / 10.0);
					break;
			}

			if (vbrMode != vbr_off && setMinVBRBitrate) ex_lame_set_VBR_min_bitrate_kbps(context, minVBRBitrate);
			if (vbrMode != vbr_off && setMaxVBRBitrate) ex_lame_set_VBR_max_bitrate_kbps(context, maxVBRBitrate);

			/* Set ATH.
			 */
			if (enableATH)
			{
				if (athType != -1) ex_lame_set_ATHtype(context, athType);
			}
			else
			{
				ex_lame_set_noATH(context, 1);
			}

			/* Set TNS.
			 */
			ex_lame_set_useTemporal(context, useTNS);

			break;
		case 1:
			ex_lame_set_preset(context, MEDIUM_FAST);
			break;
		case 2:
			ex_lame_set_preset(context, STANDARD_FAST);
			break;
		case 3:
			ex_lame_set_preset(context, EXTREME_FAST);
			break;
		case 4:
			ex_lame_set_preset(context, abrBitrate);
			break;
	}

	if (ex_lame_init_params(context) < 0)
	{
		errorString = "Bad LAME encoder settings!\n\nPlease check your encoder settings in the\nconfiguration dialog.";
		errorState  = True;

		return False;
	}

	dataOffset = 0;

	/* Write ID3v2 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v2", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);

			dataOffset = id3Buffer.Size();
		}
	}

	ex_lame_set_bWriteVbrTag(context, 1);

	return True;
}

Bool BoCA::EncoderLAME::Deactivate()
{
	const Config	*config = GetConfiguration();
	const Info	&info = track.GetInfo();

	/* Flush buffers and write remaining data.
	 */
	while (true)
	{
		unsigned long	 bytes = ex_lame_encode_flush(context, outBuffer, outBuffer.Size());

		if (bytes == 0) break;

		driver->WriteData(outBuffer, bytes);
	}

	/* Write ID3v1 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v1", False) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(id3Buffer, track);

			driver->Seek(0);
			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Write Xing or Info header.
	 */
	Buffer<unsigned char>	 buffer(2880);	// Maximum frame size
	Int			 size = ex_lame_get_lametag_frame(context, buffer, buffer.Size());

	driver->Seek(dataOffset);
	driver->WriteData(buffer, size);

	ex_lame_close(context);

	return True;
}

Int BoCA::EncoderLAME::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	outBuffer.Resize(data.Size() + 7200);

	/* Convert samples to 16 bit.
	 */
	const Format	&format = track.GetFormat();
	unsigned long	 bytes	= 0;

	if (format.bits != 16)
	{
		samplesBuffer.Resize(data.Size() / (format.bits / 8));

		for (Int i = 0; i < data.Size() / (format.bits / 8); i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	/ 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
		}

		if (format.channels == 2) bytes = ex_lame_encode_buffer_interleaved(context, samplesBuffer, data.Size() / (format.bits / 8) / format.channels, outBuffer, outBuffer.Size());
		else			  bytes = ex_lame_encode_buffer(context, samplesBuffer, samplesBuffer, data.Size() / (format.bits / 8), outBuffer, outBuffer.Size());
	}
	else
	{
		if (format.channels == 2) bytes = ex_lame_encode_buffer_interleaved(context, (signed short *) (unsigned char *) data, data.Size() / (format.bits / 8) / format.channels, outBuffer, outBuffer.Size());
		else			  bytes = ex_lame_encode_buffer(context, (signed short *) (unsigned char *) data, (signed short *) (unsigned char *) data, data.Size() / (format.bits / 8), outBuffer, outBuffer.Size());
	}

	driver->WriteData(outBuffer, bytes);

	return bytes;
}

ConfigLayer *BoCA::EncoderLAME::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureLAME();

	return configLayer;
}
