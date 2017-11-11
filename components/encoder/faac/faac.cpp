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

#include "faac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (faacdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>FAAC MP4/AAC Encoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>faac-enc</id>							\
		    <type>encoder</type>						\
		    <replace>voaacenc-enc</replace>					\
											\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>MPEG-4 AAC Files</name>				\
			      <extension>m4a</extension>				\
			      <extension>m4b</extension>				\
			      <extension>m4r</extension>				\
			      <extension>mp4</extension>				\
			      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
			    </format>							\
											\
			");
		}

		componentSpecs.Append("							\
											\
		    <format>								\
		      <name>Raw AAC Files</name>					\
		      <extension>aac</extension>					\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>		\
		    </format>								\
		  </component>								\
											\
		");

		unsigned long	 samplesSize;
		unsigned long	 bufferSize;

		faacEncHandle	 faac = ex_faacEncOpen(44100, 2, &samplesSize, &bufferSize);

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_faacEncGetCurrentConfiguration(faac)->name));

		ex_faacEncClose(faac);
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFAACDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFAACDLL();
	FreeMP4v2DLL();
}

BoCA::EncoderFAAC::EncoderFAAC()
{
	configLayer  = NIL;

	mp4File	     = NIL;
	handle	     = NIL;

	mp4Track     = -1;
	sampleId     = 0;

	frameSize    = 0;

	totalSamples = 0;
}

BoCA::EncoderFAAC::~EncoderFAAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFAAC::Activate()
{
	const Format	&format = track.GetFormat();

	if (format.channels > 6)
	{
		errorString = "This encoder does not support more than 6 channels!";
		errorState  = True;

		return False;
	}

	if (GetSampleRateIndex(format.rate) == -1)
	{
		errorString = "Bad sampling rate! The selected sampling rate is not supported.";
		errorState  = True;

		return False;
	}

	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Bool	 mp4Container = config->GetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True);
	Int	 mpegVersion  = config->GetIntValue(ConfigureFAAC::ConfigID, "MPEGVersion", 0);
	Bool	 setQuality   = config->GetIntValue(ConfigureFAAC::ConfigID, "SetQuality", True);
	Int	 aacQuality   = config->GetIntValue(ConfigureFAAC::ConfigID, "AACQuality", 100);
	Int	 bitrate      = config->GetIntValue(ConfigureFAAC::ConfigID, "Bitrate", 96);
	Bool	 allowJS      = config->GetIntValue(ConfigureFAAC::ConfigID, "AllowJS", True);
	Bool	 useTNS	      = config->GetIntValue(ConfigureFAAC::ConfigID, "UseTNS", False);
	Int	 bandwidth    = config->GetIntValue(ConfigureFAAC::ConfigID, "BandWidth", 22050);

	/* Create FAAC encoder.
	 */
	unsigned long	 samplesSize = 0;
	unsigned long	 bufferSize  = 0;

	handle = ex_faacEncOpen(format.rate, format.channels, &samplesSize, &bufferSize);

	outBuffer.Resize(bufferSize);

	/* Set encoder parameters.
	 */
	faacEncConfigurationPtr	 fConfig = ex_faacEncGetCurrentConfiguration(handle);

	fConfig->mpegVersion	= mp4Container ? MPEG4 : mpegVersion;
	fConfig->aacObjectType	= LOW;
	fConfig->jointmode	= allowJS;
	fConfig->useTns		= useTNS;
	fConfig->bandWidth	= bandwidth;

	if (mp4Container) fConfig->outputFormat = RAW_STREAM;

	if (setQuality) fConfig->quantqual = aacQuality;
	else		fConfig->bitRate   = bitrate * 1000;

	if (format.bits ==  8) fConfig->inputFormat = FAAC_INPUT_16BIT;
	if (format.bits == 16) fConfig->inputFormat = FAAC_INPUT_16BIT;
	if (format.bits == 24) fConfig->inputFormat = FAAC_INPUT_32BIT;
	if (format.bits == 32) fConfig->inputFormat = FAAC_INPUT_32BIT;

	ex_faacEncSetConfiguration(handle, fConfig);

	frameSize = samplesSize / format.channels;

	/* Create MP4 container.
	 */
	if (mp4Container)
	{
		mp4File		= ex_MP4CreateEx(String(track.outfile).Append(".out").ConvertTo("UTF-8"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);

		unsigned char	*buffer = NIL;

		ex_faacEncGetDecoderSpecificInfo(handle, &buffer, &bufferSize);

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, (uint8_t *) buffer, bufferSize);

		totalSamples = 0;
	}

	/* Write ID3v2 tag if requested.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFAAC::ConfigID, "AllowID3v2", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
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
			}
		}
	}

	return True;
}

Bool BoCA::EncoderFAAC::Deactivate()
{
	const Config	*config = GetConfiguration();

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	ex_faacEncClose(handle);

	/* Finish MP4 writing.
	 */
	if (mp4File != NIL)
	{
		/* Write iTunes metadata with gapless information.
		 */
		MP4ItmfItem	*item  = ex_MP4ItmfItemAlloc("----", 1);
		String		 value = String().Append(" 00000000")
						 .Append(" ").Append(Number((Int64) frameSize).ToHexString(8).ToUpper())
						 .Append(" ").Append(Number((Int64) frameSize - totalSamples % frameSize).ToHexString(8).ToUpper())
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

			if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
			{
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

				if (tagger != NIL)
				{
					tagger->SetConfiguration(GetConfiguration());
					tagger->RenderStreamInfo(String(track.outfile).Append(".out"), track);

					boca.DeleteComponent(tagger);
				}
			}
		}
		else
		{
			/* Optimize file even when no tags are written.
			 */
			ex_MP4Optimize(String(track.outfile).Append(".out").ConvertTo("UTF-8"), NIL);
		}

		/* Stream contents of created MP4 file to output driver
		 */
		InStream		 in(STREAM_FILE, String(track.outfile).Append(".out"), IS_READ);
		Buffer<UnsignedByte>	 buffer(1024);
		Int64			 bytesLeft = in.Size();

		while (bytesLeft)
		{
			in.InputData(buffer, Math::Min(Int64(1024), bytesLeft));

			driver->WriteData(buffer, Math::Min(Int64(1024), bytesLeft));

			bytesLeft -= Math::Min(Int64(1024), bytesLeft);
		}

		in.Close();

		File(String(track.outfile).Append(".out")).Delete();
	}

	/* Write ID3v1 tag if requested.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo())
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
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFAAC::ConfigID, "AllowID3v2", False))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
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
	}

	return True;
}

Int BoCA::EncoderFAAC::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format	= track.GetFormat();

	/* Change to AAC channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::AAC_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::AAC_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::AAC_5_1);

	/* Convert samples to 16 or 24 bit.
	 */
	Int	 samples = data.Size() / format.channels / (format.bits / 8);
	Int	 offset	 = samplesBuffer.Size();

	if (format.bits <= 16) samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels / 2);
	else		       samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) ((short *) (int32_t *) samplesBuffer)[2 * offset + i] = (				 data [i] - 128) * 256;
		else if (format.bits == 16				) ((short *) (int32_t *) samplesBuffer)[2 * offset + i] = ((short *)   (unsigned char *) data)[i];
		else if (format.bits == 32				)			 samplesBuffer [    offset + i] = ((int32_t *) (unsigned char *) data)[i]	 / 256;

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[offset + i] = (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 256;
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[offset + i] = (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 256;
	}

	/* Output samples to encoder.
	 */
	return EncodeFrames(False);
}

Int BoCA::EncoderFAAC::EncodeFrames(Bool flush)
{
	const Format	&format = track.GetFormat();

	/* Pad end of stream with empty samples.
	 */
	if (flush && samplesBuffer.Size() > 0)
	{
		Int	 nullSamples = frameSize - (samplesBuffer.Size() / format.channels * (format.bits <= 16 ? 2 : 1)) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * format.channels / (format.bits <= 16 ? 2 : 1));

		memset(((int32_t *) samplesBuffer) + samplesBuffer.Size() - nullSamples * format.channels / (format.bits <= 16 ? 2 : 1), 0, sizeof(int32_t) * nullSamples * format.channels / (format.bits <= 16 ? 2 : 1));

		totalSamples += samplesBuffer.Size() / format.channels * (format.bits <= 16 ? 2 : 1) - nullSamples;
	}

	/* Encode samples.
	 */
	Int	 dataLength	 = 0;
	Int	 framesProcessed = 0;

	Int	 samplesPerFrame = frameSize * format.channels / (format.bits <= 16 ? 2 : 1);

	while (flush || samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame)
	{
		unsigned long	 bytes = 0;

		if (samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame) bytes = ex_faacEncEncode(handle, samplesBuffer + framesProcessed * samplesPerFrame, frameSize * format.channels, outBuffer, outBuffer.Size());
		else										 bytes = ex_faacEncEncode(handle, NULL, 0, outBuffer, outBuffer.Size());

		if (flush && bytes == 0) break;

		if (!flush) totalSamples += frameSize;

		if (bytes > 0)
		{
			dataLength += bytes;

			if (mp4File != NIL) ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, bytes, frameSize, 0, true);
			else		    driver->WriteData(outBuffer, bytes);
		}

		if (samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame) framesProcessed++;
	}

	memmove((int32_t *) samplesBuffer, ((int32_t *) samplesBuffer) + framesProcessed * samplesPerFrame, sizeof(int32_t) * (samplesBuffer.Size() - framesProcessed * samplesPerFrame));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * samplesPerFrame);

	return dataLength;
}

Int BoCA::EncoderFAAC::GetSampleRateIndex(Int sampleRate) const
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

Bool BoCA::EncoderFAAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n == 0 && mp4v2dll != NIL)
	{
		config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True);
		config->SetIntValue(ConfigureFAAC::ConfigID, "MPEGVersion", 0);
	}
	else
	{
		config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", False);
	}

	return True;
}

String BoCA::EncoderFAAC::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureFAAC::ConfigID, "MP4FileExtension", 0))
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

ConfigLayer *BoCA::EncoderFAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFAAC();

	return configLayer;
}
