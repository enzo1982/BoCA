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
		    <name>LAME MP3 Encoder</name>				\
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
	const Config	*config = GetConfiguration();

	int	 effrate;

	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (config->GetIntValue("LAME", "Resample", -1) > 0)	effrate = config->GetIntValue("LAME", "Resample", -1);
	else							effrate = format.rate;

	switch (effrate)
	{
		case 8000:
		case 11025:
		case 12000:
		case 16000:
		case 22050:
		case 24000:
			if (config->GetIntValue("LAME", "SetBitrate", 1) && config->GetIntValue("LAME", "VBRMode", 4) == vbr_off && (config->GetIntValue("LAME", "Bitrate", 192) == 192 || config->GetIntValue("LAME", "Bitrate", 192) == 224 || config->GetIntValue("LAME", "Bitrate", 192) == 256 || config->GetIntValue("LAME", "Bitrate", 192) == 320))
			{
				errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMinVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && (config->GetIntValue("LAME", "MinVBRBitrate", 128) == 192 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 224 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 256 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 320))
			{
				errorString = "Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMaxVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && (config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 192 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 224 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 256 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 320))
			{
				errorString = "Bad maximum VBR bitrate! The selected maximum VBR bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}
			break;
		case 32000:
		case 44100:
		case 48000:
			if (config->GetIntValue("LAME", "SetBitrate", 1) && config->GetIntValue("LAME", "VBRMode", 4) == vbr_off && (config->GetIntValue("LAME", "Bitrate", 192) == 8 || config->GetIntValue("LAME", "Bitrate", 192) == 16 || config->GetIntValue("LAME", "Bitrate", 192) == 24 || config->GetIntValue("LAME", "Bitrate", 192) == 144))
			{
				errorString = "Bad bitrate! The selected bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMinVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && (config->GetIntValue("LAME", "MinVBRBitrate", 128) == 8 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 16 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 24 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 144))
			{
				errorString = "Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.";
				errorState  = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMaxVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && (config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 8 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 16 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 24 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 144))
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

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	outBuffer.Resize(131072);

	context = ex_lame_init();

	ex_lame_set_in_samplerate(context, format.rate);
	ex_lame_set_num_channels(context, format.channels);

	switch (config->GetIntValue("LAME", "Preset", 2))
	{
		case 0:
			ex_lame_set_copyright(context, config->GetIntValue("LAME", "Copyright", 0));
			ex_lame_set_original(context, config->GetIntValue("LAME", "Original", 1));
			ex_lame_set_extension(context, config->GetIntValue("LAME", "Private", 0));
			ex_lame_set_error_protection(context, config->GetIntValue("LAME", "CRC", 0));
			ex_lame_set_strict_ISO(context, config->GetIntValue("LAME", "StrictISO", 0));

			/* Set resampling.
			 */
			if	(config->GetIntValue("LAME", "Resample", -1) == 0) ex_lame_set_out_samplerate(context, format.rate);
			else if (config->GetIntValue("LAME", "Resample", -1) >  0) ex_lame_set_out_samplerate(context, config->GetIntValue("LAME", "Resample", -1));

			/* Set bitrate.
			 */
			if (config->GetIntValue("LAME", "VBRMode", 4) == vbr_off)
			{
				if (config->GetIntValue("LAME", "SetBitrate", 1)) ex_lame_set_brate(context, config->GetIntValue("LAME", "Bitrate", 192));
				else						  ex_lame_set_compression_ratio(context, ((double) config->GetIntValue("LAME", "Ratio", 1100)) / 100);
			}

			/* Set quality.
			 */
			if (config->GetIntValue("LAME", "SetQuality", 0)) ex_lame_set_quality(context, config->GetIntValue("LAME", "Quality", 3));

			/* Set audio filtering.
			 */
			if (config->GetIntValue("LAME", "DisableFiltering", 0))
			{
				ex_lame_set_lowpassfreq(context, -1);
				ex_lame_set_highpassfreq(context, -1);
			}
			else
			{
				if (config->GetIntValue("LAME", "SetLowpass", 0)) ex_lame_set_lowpassfreq(context, config->GetIntValue("LAME", "Lowpass", 0));
				if (config->GetIntValue("LAME", "SetHighpass", 0)) ex_lame_set_highpassfreq(context, config->GetIntValue("LAME", "Highpass", 0));

				if (config->GetIntValue("LAME", "SetLowpass", 0) && config->GetIntValue("LAME", "SetLowpassWidth", 0)) ex_lame_set_lowpasswidth(context, config->GetIntValue("LAME", "LowpassWidth", 0));
				if (config->GetIntValue("LAME", "SetHighpass", 0) && config->GetIntValue("LAME", "SetHighpassWidth", 0)) ex_lame_set_highpasswidth(context, config->GetIntValue("LAME", "HighpassWidth", 0));
			}

			/* Set Stereo mode.
			 */
			if	(config->GetIntValue("LAME", "StereoMode", 0) == 1)	ex_lame_set_mode(context, MONO);
			else if (config->GetIntValue("LAME", "StereoMode", 0) == 2)	ex_lame_set_mode(context, STEREO);
			else if (config->GetIntValue("LAME", "StereoMode", 0) == 3)	ex_lame_set_mode(context, JOINT_STEREO);
			else								ex_lame_set_mode(context, NOT_SET);

			if (config->GetIntValue("LAME", "StereoMode", 0) == 3)
			{
				if (config->GetIntValue("LAME", "ForceJS", 0))	ex_lame_set_force_ms(context, 1);
				else						ex_lame_set_force_ms(context, 0);
			}

			/* Set VBR mode.
			 */
			switch (config->GetIntValue("LAME", "VBRMode", 4))
			{
				default:
				case vbr_off:
					break;
				case vbr_abr:
					ex_lame_set_VBR(context, vbr_abr);
					ex_lame_set_VBR_mean_bitrate_kbps(context, config->GetIntValue("LAME", "ABRBitrate", 192));
					break;
				case vbr_rh:
					ex_lame_set_VBR(context, vbr_rh);
					ex_lame_set_VBR_quality(context, config->GetIntValue("LAME", "VBRQuality", 50) / 10);
					break;
				case vbr_mtrh:
					ex_lame_set_VBR(context, vbr_mtrh);
					ex_lame_set_VBR_quality(context, config->GetIntValue("LAME", "VBRQuality", 50) / 10);
					break;
			}

			if (config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && config->GetIntValue("LAME", "SetMinVBRBitrate", 0)) ex_lame_set_VBR_min_bitrate_kbps(context, config->GetIntValue("LAME", "MinVBRBitrate", 128));
			if (config->GetIntValue("LAME", "VBRMode", 4) != vbr_off && config->GetIntValue("LAME", "SetMaxVBRBitrate", 0)) ex_lame_set_VBR_max_bitrate_kbps(context, config->GetIntValue("LAME", "MaxVBRBitrate", 128));

			/* Set ATH.
			 */
			if (config->GetIntValue("LAME", "EnableATH", 1))
			{
				if (config->GetIntValue("LAME", "ATHType", -1) != -1) ex_lame_set_ATHtype(context, config->GetIntValue("LAME", "ATHType", -1));
			}
			else
			{
				ex_lame_set_noATH(context, 1);
			}

			/* Set TNS.
			 */
			ex_lame_set_useTemporal(context, config->GetIntValue("LAME", "UseTNS", 1));

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
			ex_lame_set_preset(context, config->GetIntValue("LAME", "ABRBitrate", 192));
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
	if (((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
	     (info.artist != NIL || info.title != NIL)) && config->GetIntValue("Tags", "EnableID3v2", True))
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
	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v1", False))
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
