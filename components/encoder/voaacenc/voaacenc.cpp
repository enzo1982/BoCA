 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "voaacenc.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderVOAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (voaacencdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>VisualOn AAC Encoder</name>					\
		    <version>1.0</version>						\
		    <id>voaacenc-enc</id>						\
		    <type>encoder</type>						\
											\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>MP4 Audio Files</name>				\
			      <extension>m4a</extension>				\
			      <extension>m4b</extension>				\
			      <extension>m4r</extension>				\
			      <extension>mp4</extension>				\
			      <extension>3gp</extension>				\
			      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
			    </format>							\
											\
			");
		}

		componentSpecs.Append("							\
											\
		    <format>								\
		      <name>Advanced Audio Files</name>					\
		      <extension>aac</extension>					\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>		\
		    </format>								\
		  </component>								\
											\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadVOAACEncDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeVOAACEncDLL();
	FreeMP4v2DLL();
}

BoCA::EncoderVOAAC::EncoderVOAAC()
{
	configLayer    = NIL;

	mp4File	       = NIL;
	handle	       = NIL;

	mp4Track       = -1;
	sampleId       = 0;

	frameSize      = 0;

	totalSamples   = 0;
	delaySamples   = 0;

	memset(&memOperator, 0, sizeof(memOperator));
	memset(&userData, 0, sizeof(userData));

	ex_voGetAACEncAPI(&api);
}

BoCA::EncoderVOAAC::~EncoderVOAAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderVOAAC::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	if (GetSampleRateIndex(format.rate) == -1)
	{
		errorString = "Bad sampling rate! The selected sampling rate is not supported.";
		errorState  = True;

		return False;
	}

	Config	*config = Config::Get();

	unsigned long	 samplesSize = 1024 * format.channels;
	unsigned long	 bufferSize  = samplesSize * 4;

	outBuffer.Resize(bufferSize);

	memOperator.Alloc = ex_cmnMemAlloc;
	memOperator.Copy  = ex_cmnMemCopy;
	memOperator.Free  = ex_cmnMemFree;
	memOperator.Set	  = ex_cmnMemSet;
	memOperator.Check = ex_cmnMemCheck;

	userData.memflag  = VO_IMF_USERMEMOPERATOR;
	userData.memData  = &memOperator;

	api.Init(&handle, VO_AUDIO_CodingAAC, &userData);

	AACENC_PARAM	 params;

	params.sampleRate = format.rate;
	params.nChannels  = format.channels;
	params.bitRate	  = config->GetIntValue("VOAACEnc", "Bitrate", 128) * 1000;
	params.adtsUsed	  = !config->GetIntValue("VOAACEnc", "MP4Container", 1);

	api.SetParam(handle, VO_PID_AAC_ENCPARAM, &params);

	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		mp4File		= ex_MP4CreateEx(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);

		int	 sampleRateIndex = GetSampleRateIndex(format.rate);
		uint8_t	 esConfig[2]	 = { uint8_t(			 2 << 3 | sampleRateIndex >> 1),
					     uint8_t((sampleRateIndex & 1) << 7 | format.channels << 3) };

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, esConfig, 2);

		totalSamples = 0;
	}

	frameSize    = samplesSize / format.channels;
	delaySamples = frameSize + 576;

	/* Write ID3v2 tag if requested.
	 */
	if (!config->GetIntValue("VOAACEnc", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("VOAACEnc", "AllowID3v2", 0))
	{
		if ((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
		    (info.artist != NIL || info.title != NIL))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Bool BoCA::EncoderVOAAC::Deactivate()
{
	Config	*config = Config::Get();

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(samplesBuffer, outBuffer, True);

	api.Uninit(handle);

	/* Finish MP4 writing.
	 */
	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		/* Write iTunes metadata with gapless information.
		 */
		MP4ItmfItem	*item  = ex_MP4ItmfItemAlloc("----", 1);
		String		 value = String().Append(" 00000000")
						 .Append(" ").Append(Number((Int64) delaySamples).ToHexString(8).ToUpper())
						 .Append(" ").Append(Number((Int64) frameSize - (delaySamples + totalSamples) % frameSize).ToHexString(8).ToUpper())
						 .Append(" ").Append(Number((Int64) totalSamples).ToHexString(16).ToUpper())
						 .Append(" 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000");

		item->mean = (char *) "com.apple.iTunes";
		item->name = (char *) "iTunSMPB";

		item->dataList.elements[0].typeCode  = MP4_ITMF_BT_UTF8;
		item->dataList.elements[0].value     = (uint8_t *) value.ConvertTo("UTF-8");
		item->dataList.elements[0].valueSize = value.Length();

		ex_MP4ItmfAddItem(mp4File, item);

		item->mean = NIL;
		item->name = NIL;

		item->dataList.elements[0].typeCode  = MP4_ITMF_BT_IMPLICIT;
		item->dataList.elements[0].value     = NIL;
		item->dataList.elements[0].valueSize = 0;

		ex_MP4ItmfItemFree(item);

		ex_MP4Close(mp4File, 0);

		/* Write metadata to file
		 */
		if (config->GetIntValue("Tags", "EnableMP4Metadata", True))
		{
			const Info	&info = track.GetInfo();

			if ((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
			    (info.artist != NIL || info.title != NIL))
			{
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

				if (tagger != NIL)
				{
					tagger->RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), track);

					boca.DeleteComponent(tagger);
				}
			}
		}

		/* Stream contents of created MP4 file to output driver
		 */
		InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READ);
		Buffer<UnsignedByte>	 buffer(1024);
		Int64			 bytesLeft = in.Size();

		while (bytesLeft)
		{
			in.InputData(buffer, Math::Min(Int64(1024), bytesLeft));

			driver->WriteData(buffer, Math::Min(Int64(1024), bytesLeft));

			bytesLeft -= Math::Min(Int64(1024), bytesLeft);
		}

		in.Close();

		File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();
	}

	/* Write ID3v1 tag if requested.
	 */
	if (!config->GetIntValue("VOAACEnc", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.artist != NIL || info.title != NIL)
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (!config->GetIntValue("VOAACEnc", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("VOAACEnc", "AllowID3v2", 0))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->Seek(0);
				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Int BoCA::EncoderVOAAC::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = size / format.channels / (format.bits / 8);
	Int		 offset	 = samplesBuffer.Size();

	samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[offset + i] =	    (				    data [i] - 128) * 256;
		else if	(format.bits == 16				) samplesBuffer[offset + i] = (int)  ((int16_t *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[offset + i] = (int) (((int32_t *) (unsigned char *) data)[i]	    / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[offset + i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[offset + i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Output samples to encoder.
	 */
	return EncodeFrames(samplesBuffer, outBuffer, False);
}

Int BoCA::EncoderVOAAC::EncodeFrames(Buffer<int16_t> &samplesBuffer, Buffer<unsigned char> &outBuffer, Bool flush)
{
	Config		*config = Config::Get();
	const Format	&format = track.GetFormat();

	/* Pad end of stream with empty samples.
	 */
	if (flush)
	{
		Int	 nullSamples = delaySamples;

		if ((samplesBuffer.Size() / format.channels + delaySamples) % frameSize > 0) nullSamples += frameSize - (samplesBuffer.Size() / format.channels + delaySamples) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * format.channels);

		memset(((int16_t *) samplesBuffer) + samplesBuffer.Size() - nullSamples * format.channels, 0, sizeof(int16_t) * nullSamples * format.channels);

		totalSamples += samplesBuffer.Size() / format.channels - nullSamples;
	}

	/* Encode samples.
	 */
	Int	 dataLength	 = 0;
	Int	 framesProcessed = 0;

	while (samplesBuffer.Size() - framesProcessed * frameSize * format.channels >= frameSize * format.channels)
	{
		/* Prepare buffer information.
		 */
		VO_CODECBUFFER		 input	     = { 0 };
		VO_CODECBUFFER		 output	     = { 0 };
		VO_AUDIO_OUTPUTINFO	 outputInfo  = { 0 };

		input.Buffer = (uint8_t *) ((int16_t *) samplesBuffer + framesProcessed * frameSize * format.channels);
		input.Length = frameSize * format.channels * sizeof(int16_t);

		/* Hand input data to encoder and retrieve output.
		 */
		api.SetInputData(handle, &input);

		output.Buffer = outBuffer;
		output.Length = outBuffer.Size();

		if (api.GetOutputData(handle, &output, &outputInfo) == VO_ERR_NONE)
		{
			if (!flush) totalSamples += frameSize;

			dataLength += output.Length;

			if (config->GetIntValue("VOAACEnc", "MP4Container", 1)) ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, output.Length, frameSize, 0, true);
			else							driver->WriteData(outBuffer, output.Length);
		}

		framesProcessed++;
	}

	memmove((int16_t *) samplesBuffer, ((int16_t *) samplesBuffer) + framesProcessed * frameSize * format.channels, sizeof(int16_t) * (samplesBuffer.Size() - framesProcessed * frameSize * format.channels));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * frameSize * format.channels);

	return dataLength;
}

Int BoCA::EncoderVOAAC::GetSampleRateIndex(Int sampleRate)
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

String BoCA::EncoderVOAAC::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		switch (config->GetIntValue("VOAACEnc", "MP4FileExtension", 0))
		{
			default:
			case  0: return "m4a";
			case  1: return "m4b";
			case  2: return "m4r";
			case  3: return "mp4";
		}
	}

	return "aac";
}

ConfigLayer *BoCA::EncoderVOAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVOAAC();

	return configLayer;
}
