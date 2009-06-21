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

#include "lame_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::LAMEOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (lamedll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>LAME MP3 Encoder</name>		\
		    <version>1.0</version>			\
		    <id>lame-out</id>				\
		    <type>encoder</type>			\
		    <format>					\
		      <name>MPEG 1 Audio Layer 3</name>		\
		      <extension>mp3</extension>		\
		    </format>					\
		  </component>					\
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

BoCA::LAMEOut::LAMEOut()
{
	configLayer = NIL;
}

BoCA::LAMEOut::~LAMEOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::LAMEOut::Activate()
{
	Config	*config = Config::Get();

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
			if (config->GetIntValue("LAME", "SetBitrate", 1) && config->GetIntValue("LAME", "VBRMode", 0) == vbr_off && (config->GetIntValue("LAME", "Bitrate", 192) == 192 || config->GetIntValue("LAME", "Bitrate", 192) == 224 || config->GetIntValue("LAME", "Bitrate", 192) == 256 || config->GetIntValue("LAME", "Bitrate", 192) == 320))
			{
				Utilities::ErrorMessage("Bad bitrate! The selected bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMinVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && (config->GetIntValue("LAME", "MinVBRBitrate", 128) == 192 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 224 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 256 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 320))
			{
				Utilities::ErrorMessage("Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMaxVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && (config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 192 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 224 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 256 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 320))
			{
				Utilities::ErrorMessage("Bad maximum VBR bitrate! The selected maximum VBR bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}
			break;
		case 32000:
		case 44100:
		case 48000:
			if (config->GetIntValue("LAME", "SetBitrate", 1) && config->GetIntValue("LAME", "VBRMode", 0) == vbr_off && (config->GetIntValue("LAME", "Bitrate", 192) == 8 || config->GetIntValue("LAME", "Bitrate", 192) == 16 || config->GetIntValue("LAME", "Bitrate", 192) == 24 || config->GetIntValue("LAME", "Bitrate", 192) == 144))
			{
				Utilities::ErrorMessage("Bad bitrate! The selected bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMinVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && (config->GetIntValue("LAME", "MinVBRBitrate", 128) == 8 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 16 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 24 || config->GetIntValue("LAME", "MinVBRBitrate", 128) == 144))
			{
				Utilities::ErrorMessage("Bad minimum VBR bitrate! The selected minimum VBR bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}

			if (config->GetIntValue("LAME", "SetMaxVBRBitrate", 0) && config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && (config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 8 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 16 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 24 || config->GetIntValue("LAME", "MaxVBRBitrate", 128) == 144))
			{
				Utilities::ErrorMessage("Bad maximum VBR bitrate! The selected maximum VBR bitrate is not supported for this sampling rate.");

				errorState = True;

				return False;
			}
			break;
		default:
			Utilities::ErrorMessage("Bad sampling rate! The selected sampling rate is not supported.");

			errorState = True;

			return False;
	}

	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	outBuffer.Resize(131072);

	lameFlags = ex_lame_init();

	ex_lame_set_in_samplerate(lameFlags, format.rate);
	ex_lame_set_num_channels(lameFlags, format.channels);

	switch (config->GetIntValue("LAME", "Preset", 0))
	{
		case 0:
			ex_lame_set_copyright(lameFlags, config->GetIntValue("LAME", "Copyright", 0));
			ex_lame_set_original(lameFlags, config->GetIntValue("LAME", "Original", 1));
			ex_lame_set_extension(lameFlags, config->GetIntValue("LAME", "Private", 0));
			ex_lame_set_error_protection(lameFlags, config->GetIntValue("LAME", "CRC", 0));
			ex_lame_set_strict_ISO(lameFlags, config->GetIntValue("LAME", "StrictISO", 0));

			if (config->GetIntValue("LAME", "Resample", -1) == 0)	  ex_lame_set_out_samplerate(lameFlags, format.rate);
			else if (config->GetIntValue("LAME", "Resample", -1) > 0) ex_lame_set_out_samplerate(lameFlags, config->GetIntValue("LAME", "Resample", -1));

			if (config->GetIntValue("LAME", "VBRMode", 0) == vbr_off)
			{
				if (config->GetIntValue("LAME", "SetBitrate", 1)) ex_lame_set_brate(lameFlags, config->GetIntValue("LAME", "Bitrate", 192));
				else						  ex_lame_set_compression_ratio(lameFlags, ((double) config->GetIntValue("LAME", "Ratio", 1100)) / 100);
			}

			if (config->GetIntValue("LAME", "SetQuality", 0)) ex_lame_set_quality(lameFlags, config->GetIntValue("LAME", "Quality", 5));
			else						  ex_lame_set_quality(lameFlags, 5);

			if (config->GetIntValue("LAME", "DisableFiltering", 0))
			{
				ex_lame_set_lowpassfreq(lameFlags, -1);
				ex_lame_set_highpassfreq(lameFlags, -1);
			}
			else
			{
				if (config->GetIntValue("LAME", "SetLowpass", 0)) ex_lame_set_lowpassfreq(lameFlags, config->GetIntValue("LAME", "Lowpass", 0));
				if (config->GetIntValue("LAME", "SetHighpass", 0)) ex_lame_set_highpassfreq(lameFlags, config->GetIntValue("LAME", "Highpass", 0));

				if (config->GetIntValue("LAME", "SetLowpass", 0) && config->GetIntValue("LAME", "SetLowpassWidth", 0)) ex_lame_set_lowpasswidth(lameFlags, config->GetIntValue("LAME", "LowpassWidth", 0));
				if (config->GetIntValue("LAME", "SetHighpass", 0) && config->GetIntValue("LAME", "SetHighpassWidth", 0)) ex_lame_set_highpasswidth(lameFlags, config->GetIntValue("LAME", "HighpassWidth", 0));
			}

			if (format.channels == 2)
			{
				if	(config->GetIntValue("LAME", "StereoMode", 0) == 1)	ex_lame_set_mode(lameFlags, MONO);
				else if (config->GetIntValue("LAME", "StereoMode", 0) == 2)	ex_lame_set_mode(lameFlags, STEREO);
				else if (config->GetIntValue("LAME", "StereoMode", 0) == 3)	ex_lame_set_mode(lameFlags, JOINT_STEREO);
				else								ex_lame_set_mode(lameFlags, NOT_SET);

				if (config->GetIntValue("LAME", "StereoMode", 0) == 3)
				{
					if (config->GetIntValue("LAME", "ForceJS", 0))	ex_lame_set_force_ms(lameFlags, 1);
					else						ex_lame_set_force_ms(lameFlags, 0);
				}
			}
			else if (format.channels == 1)
			{
				ex_lame_set_mode(lameFlags, MONO);
			}

			switch (config->GetIntValue("LAME", "VBRMode", 0))
			{
				default:
				case vbr_off:
					break;
				case vbr_abr:
					ex_lame_set_VBR(lameFlags, vbr_abr);
					ex_lame_set_VBR_mean_bitrate_kbps(lameFlags, config->GetIntValue("LAME", "ABRBitrate", 192));
					break;
				case vbr_rh:
					ex_lame_set_VBR(lameFlags, vbr_rh);
					ex_lame_set_VBR_quality(lameFlags, config->GetIntValue("LAME", "VBRQuality", 50) / 10);
					break;
				case vbr_mtrh:
					ex_lame_set_VBR(lameFlags, vbr_mtrh);
					ex_lame_set_VBR_quality(lameFlags, config->GetIntValue("LAME", "VBRQuality", 50) / 10);
					break;
			}

			if (config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && config->GetIntValue("LAME", "SetMinVBRBitrate", 0)) ex_lame_set_VBR_min_bitrate_kbps(lameFlags, config->GetIntValue("LAME", "MinVBRBitrate", 128));
			if (config->GetIntValue("LAME", "VBRMode", 0) != vbr_off && config->GetIntValue("LAME", "SetMaxVBRBitrate", 0)) ex_lame_set_VBR_max_bitrate_kbps(lameFlags, config->GetIntValue("LAME", "MaxVBRBitrate", 128));

			if (config->GetIntValue("LAME", "EnableATH", 1))
			{
				if (config->GetIntValue("LAME", "ATHType", -1) != -1) ex_lame_set_ATHtype(lameFlags, config->GetIntValue("LAME", "ATHType", -1));
			}
			else
			{
				ex_lame_set_noATH(lameFlags, 1);
			}

			ex_lame_set_useTemporal(lameFlags, config->GetIntValue("LAME", "UseTNS", 1));

			break;
		case 1:
			ex_lame_set_preset(lameFlags, MEDIUM);
			break;
		case 2:
			ex_lame_set_preset(lameFlags, STANDARD);
			break;
		case 3:
			ex_lame_set_preset(lameFlags, EXTREME);
			break;
		case 4:
			ex_lame_set_preset(lameFlags, INSANE);
			break;
		case 5:
			ex_lame_set_preset(lameFlags, MEDIUM_FAST);
			break;
		case 6:
			ex_lame_set_preset(lameFlags, STANDARD_FAST);
			break;
		case 7:
			ex_lame_set_preset(lameFlags, EXTREME_FAST);
			break;
		case 8:
			ex_lame_set_preset(lameFlags, R3MIX);
			break;
		case 9:
			ex_lame_set_preset(lameFlags, config->GetIntValue("LAME", "ABRBitrate", 192));
			break;
	}

	if (ex_lame_init_params(lameFlags) < 0)
	{
		Utilities::ErrorMessage("Bad LAME encoder settings!\n\nPlease check your encoder settings in the\nconfiguration dialog.");

		return False;
	}

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True) && config->enable_id3)
	{
		Buffer<unsigned char>	 id3Buffer;
		Int			 size = TagID3v2().Render(track, id3Buffer);

		driver->WriteData(id3Buffer, size);
	}

	ex_lame_set_bWriteVbrTag(lameFlags, 1);

	return True;
}

Bool BoCA::LAMEOut::Deactivate()
{
	Config		*config = Config::Get();
	const Info	&info = track.GetInfo();

	unsigned long	 bytes = ex_lame_encode_flush(lameFlags, outBuffer, outBuffer.Size());

	driver->WriteData(outBuffer, bytes);

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v1", False) && config->enable_id3)
	{
		Buffer<unsigned char>	 id3Buffer;
		Int			 size = TagID3v1().Render(track, id3Buffer);

		driver->WriteData(id3Buffer, size);
	}

	if (ex_lame_get_VBR(lameFlags) != vbr_off)
	{
		String	 tempFile = S::System::System::GetTempDirectory().Append("xing.tmp");

		FILE	*f_out = fopen(tempFile, "w+b");

		if (f_out != NIL)
		{
			Buffer<unsigned char>	 buffer(driver->GetSize());

			driver->Seek(0);
			driver->ReadData(buffer, buffer.Size());

			fwrite(buffer, 1, buffer.Size(), f_out);

			ex_lame_mp3_tags_fid(lameFlags, f_out);

			buffer.Resize(ftell(f_out));

			fseek(f_out, 0, SEEK_SET);

			fread((void *) buffer, 1, buffer.Size(), f_out);

			driver->Seek(0);
			driver->WriteData(buffer, buffer.Size());

			fclose(f_out);

			File(tempFile).Delete();
		}
	}

	ex_lame_close(lameFlags);

	return True;
}

Int BoCA::LAMEOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	unsigned long	 bytes = 0;

	outBuffer.Resize(size + 7200);

	const Format	&format = track.GetFormat();

	if (format.bits != 16)
	{
		samplesBuffer.Resize(size / (format.bits / 8));

		for (int i = 0; i < size / (format.bits / 8); i++)
		{
			if (format.bits == 8)	samplesBuffer[i] = (data[i] - 128) * 256;
			if (format.bits == 24)	samplesBuffer[i] = (int) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
			if (format.bits == 32)	samplesBuffer[i] = (int) ((long *) (unsigned char *) data)[i] / 65536;
		}

		if (format.channels == 2)	bytes = ex_lame_encode_buffer_interleaved(lameFlags, samplesBuffer, size / (format.bits / 8) / format.channels, outBuffer, outBuffer.Size());
		else				bytes = ex_lame_encode_buffer(lameFlags, samplesBuffer, samplesBuffer, size / (format.bits / 8), outBuffer, outBuffer.Size());
	}
	else
	{
		if (format.channels == 2)	bytes = ex_lame_encode_buffer_interleaved(lameFlags, (signed short *) (unsigned char *) data, size / (format.bits / 8) / format.channels, outBuffer, outBuffer.Size());
		else				bytes = ex_lame_encode_buffer(lameFlags, (signed short *) (unsigned char *) data, (signed short *) (unsigned char *) data, size / (format.bits / 8), outBuffer, outBuffer.Size());
	}

	driver->WriteData(outBuffer, bytes);

	return bytes;
}

ConfigLayer *BoCA::LAMEOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureLAME();

	return configLayer;
}

Void BoCA::LAMEOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
