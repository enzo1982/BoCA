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

#include "wma_out.h"
#include "config.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::WMAOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (wmvcoredll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Windows Media Audio Encoder</name>	\
		    <version>1.0</version>			\
		    <id>wma-out</id>				\
		    <type>encoder</type>			\
		    <format>					\
		      <name>Windows Media Audio</name>		\
		      <extension>wma</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWMVCoreDLL();
}

Void smooth::DetachDLL()
{
	FreeWMVCoreDLL();
}

BoCA::WMAOut::WMAOut()
{
	configLayer = NIL;

	samplesWritten = 0;

	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

BoCA::WMAOut::~WMAOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);

	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

Bool BoCA::WMAOut::Activate()
{
	Config	*config = Config::Get();

	HRESULT	 hr = S_OK;

	hr = ex_WMCreateWriter(NIL, &m_pWriter);
	hr = m_pWriter->QueryInterface(IID_IWMWriterAdvanced, (void **) &m_pWriterAdvanced);

	hr = ex_WMCreateWriterFileSink(&m_pWriterFileSink);

	hr = m_pWriterFileSink->Open(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"));

	hr = m_pWriterAdvanced->AddSink(m_pWriterFileSink);

	hr = ex_WMCreateProfileManager(&m_pProfileManager);

	hr = m_pProfileManager->CreateEmptyProfile(WMT_VER_9_0, &m_pProfile);

	IWMCodecInfo3	*pCodecInfo = NIL;

	hr = m_pProfileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &pCodecInfo);

	if (config->GetIntValue("WMA", "Uncompressed", False))
	{
		hr = m_pProfile->CreateNewStream(WMMEDIATYPE_Audio, &m_pStreamConfig);
	}
	else if (!config->GetIntValue("WMA", "AutoSelectFormat", True))
	{
		BOOL	 falseValue = FALSE;
		BOOL	 trueValue = TRUE;
		DWORD	 oneValue = 1;
		DWORD	 twoValue = 2;

		if (config->GetIntValue("WMA", "EnableVBR", True))    hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, config->GetIntValue("WMA", "Codec", 0), g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue,  sizeof(BOOL));
		else						      hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, config->GetIntValue("WMA", "Codec", 0), g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));

		if (config->GetIntValue("WMA", "Enable2Pass", False)) hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, config->GetIntValue("WMA", "Codec", 0), g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));
		else						      hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, config->GetIntValue("WMA", "Codec", 0), g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

		hr = pCodecInfo->GetCodecFormat(WMMEDIATYPE_Audio, config->GetIntValue("WMA", "Codec", 0), config->GetIntValue("WMA", "CodecFormat", 0), &m_pStreamConfig);
	}
	else
	{
		m_pStreamConfig = GetBestCodecFormat(pCodecInfo, config->GetIntValue("WMA", "Codec", 0), track.GetFormat(), config->GetIntValue("WMA", "Bitrate", 128) * 1000);
	}

	hr = m_pStreamConfig->SetStreamNumber(1);
	hr = m_pStreamConfig->SetStreamName(String("Audio"));
	hr = m_pStreamConfig->SetConnectionName(String("Audio"));

	hr = m_pProfile->AddStream(m_pStreamConfig);

	hr = m_pWriter->SetProfile(m_pProfile);

	SetInputFormat(m_pWriter, track.GetFormat());

	pCodecInfo->Release();

	hr = m_pWriter->BeginWriting();

	return True;
}

Bool BoCA::WMAOut::Deactivate()
{
	Config	*config = Config::Get();

	HRESULT	 hr = S_OK;

	hr = m_pWriter->Flush();

	hr = m_pWriter->EndWriting();

	hr = m_pWriterAdvanced->RemoveSink(m_pWriterFileSink);

	m_pStreamConfig->Release();

	m_pProfile->Release();
	m_pProfileManager->Release();

	m_pWriterFileSink->Release();

	m_pWriterAdvanced->Release();
	m_pWriter->Release();

	/* Write metadata to file
	 */
	if (config->GetIntValue("Tags", "EnableWMAMetadata", True))
	{
		const Info	&info = track.GetInfo();

		if (info.artist != NIL || info.title != NIL) TagWMA().Render(track, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"));
	}

	/* Stream contents of created WMA file to output driver
	 */
	InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READONLY);
	Buffer<UnsignedByte>	 buffer(1024);
	Int			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		driver->WriteData((UnsignedByte *) in.InputData(buffer, Math::Min(1024, bytesLeft)), Math::Min(1024, bytesLeft));

		bytesLeft -= Math::Min(1024, bytesLeft);
	}

	in.Close();

	File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();

	return True;
}

Int BoCA::WMAOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	HRESULT	 hr;

	INSSBuffer	*pSample = NIL;

	hr = m_pWriter->AllocateSample(size, &pSample);

	BYTE	*buffer = NIL;

	hr = pSample->GetBuffer(&buffer);

	memcpy(buffer, data, size);

	pSample->SetLength(size);

	const Format	&format = track.GetFormat();
	QWORD		 cnsSampleTime = samplesWritten * 10000000 / format.channels / format.rate;

	samplesWritten += size / (format.bits / 8);

        hr = m_pWriter->WriteSample(0, cnsSampleTime, WM_SF_CLEANPOINT, pSample);

	pSample->Release();

	return size;
}

ConfigLayer *BoCA::WMAOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWMAEnc();

	return configLayer;
}

Void BoCA::WMAOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}

/* This method will return the format best matching
 * our requirements for a specified codec.
 */
IWMStreamConfig *BoCA::WMAOut::GetBestCodecFormat(IWMCodecInfo3 *pCodecInfo, DWORD codecIndex, const Format &format, Int bitrate)
{
	Config	*config = Config::Get();

	HRESULT		 hr = S_OK;

	BOOL		 falseValue = FALSE;
	BOOL		 trueValue = TRUE;
	DWORD		 oneValue = 1;
	DWORD		 twoValue = 2;

	Bool		 supportCBR	= False;
	Bool		 supportVBR	= False;
	Bool		 support1Pass	= True;
	Bool		 support2Pass	= False;

	/* Check if CBR is supported.
	 */
	{
		hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));
		hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

		DWORD	 numFormats = 0;

		hr = pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

		if (hr == S_OK && numFormats > 0) supportCBR = True;
	}

	/* Check if VBR is supported.
	 */
	{
		WMT_ATTR_DATATYPE	 valueType = WMT_TYPE_BOOL;
		DWORD			 valueSize = sizeof(BOOL);
		BOOL			 isVBRSupported = FALSE;

		hr = pCodecInfo->GetCodecProp(WMMEDIATYPE_Audio, codecIndex, g_wszIsVBRSupported, &valueType, (BYTE *) &isVBRSupported, &valueSize);

		if (isVBRSupported)
		{
			hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue, sizeof(BOOL));
			hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

			if (hr == S_OK)
			{
				DWORD	 numFormats = 0;

				hr = pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

				if (hr == S_OK && numFormats > 0) supportVBR = True;
			}
		}
	}

	if (supportVBR && (!supportCBR || config->GetIntValue("WMA", "EnableVBR", True)))	 hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue,  sizeof(BOOL));
	else											 hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));

	if (support2Pass && (!support1Pass || config->GetIntValue("WMA", "Enable2Pass", False))) hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));
	else											 hr = pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

	IWMStreamConfig	*result = NIL;

	DWORD		 bestMatchBitrate	= 100000000;
	DWORD		 bestMatchSampleRate	= 100000000;
	DWORD		 bestMatchBits		= 100000000;
	DWORD		 bestMatchChannels	= 100000000;

	DWORD		 numFormats = 0;

	hr = pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

	for (DWORD i = 0; i < numFormats; i++)
	{
		IWMStreamConfig	*pStreamConfig = NIL;
		IWMMediaProps	*pMediaProps = NIL;

		hr = pCodecInfo->GetCodecFormat(WMMEDIATYPE_Audio, codecIndex, i, &pStreamConfig);
		hr = pStreamConfig->QueryInterface(IID_IWMMediaProps, (void **) &pMediaProps);

		DWORD		 formatBitrate = 0;

		hr = pStreamConfig->GetBitrate(&formatBitrate);

		DWORD		 mediaTypeSize = 0;

		hr = pMediaProps->GetMediaType(NIL, &mediaTypeSize);

		WM_MEDIA_TYPE	*mediaType = (WM_MEDIA_TYPE *) new BYTE [mediaTypeSize];

		hr = pMediaProps->GetMediaType(mediaType, &mediaTypeSize);

		if (mediaType->majortype  == WMMEDIATYPE_Audio &&
		    mediaType->formattype == WMFORMAT_WaveFormatEx)
		{
			Bool		 newBestMatch = False;
			WAVEFORMATEX	*waveFormat = (WAVEFORMATEX *) mediaType->pbFormat;

			if	(Math::Abs(Int(waveFormat->nChannels	  - format.channels))	      < Math::Abs(Int(bestMatchChannels	  - format.channels))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate))  < Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate)) == Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate)) &&
				 Math::Abs(Int(waveFormat->wBitsPerSample - format.bits))	      < Math::Abs(Int(bestMatchBits	  - format.bits))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate)) == Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate)) &&
				 Math::Abs(Int(waveFormat->wBitsPerSample - format.bits))	     == Math::Abs(Int(bestMatchBits	  - format.bits)) &&
				 Math::Abs(Int(formatBitrate		  - bitrate))		      < Math::Abs(Int(bestMatchBitrate	  - bitrate))) newBestMatch = True;

			if (newBestMatch)
			{
				if (Math::Abs(Int(formatBitrate - bitrate)) < Math::Abs(Int(bestMatchBitrate - bitrate)))
				{
					if (result != NIL) result->Release();

					result = pStreamConfig;
					result->AddRef();

					bestMatchChannels	= waveFormat->nChannels;
					bestMatchBits		= waveFormat->wBitsPerSample;
					bestMatchSampleRate	= waveFormat->nSamplesPerSec;
					bestMatchBitrate	= formatBitrate;
				}
			}
		}

		delete [] mediaType;

		pMediaProps->Release();
		pStreamConfig->Release();		
	}

	return result;
}

/* This method will set the input format to the
 * specified format.
 */
Bool BoCA::WMAOut::SetInputFormat(IWMWriter *pWriter, const Format &format)
{
	HRESULT			 hr = S_OK;

	IWMInputMediaProps	*pInputProps = NIL;

	hr = pWriter->GetInputProps(0, &pInputProps);

	WM_MEDIA_TYPE		 mediaType;

	mediaType.majortype		= WMMEDIATYPE_Audio;
	mediaType.subtype		= WMMEDIASUBTYPE_PCM;
	mediaType.bFixedSizeSamples	= True;
	mediaType.bTemporalCompression	= False;
	mediaType.lSampleSize		= (format.bits / 8) * format.channels;
	mediaType.formattype		= WMFORMAT_WaveFormatEx;
	mediaType.pUnk			= NIL;

	WAVEFORMATEX		 waveFormat;

	waveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	waveFormat.nChannels		= format.channels;
	waveFormat.nSamplesPerSec	= format.rate;
	waveFormat.nAvgBytesPerSec	= format.rate * (format.bits / 8) * format.channels;
	waveFormat.nBlockAlign		= (format.bits / 8) * format.channels;
	waveFormat.wBitsPerSample	= format.bits;
	waveFormat.cbSize		= 0;

	mediaType.cbFormat		= sizeof(waveFormat);
	mediaType.pbFormat		= (BYTE *) &waveFormat;

	hr = pInputProps->SetMediaType(&mediaType);

	hr = m_pWriter->SetInputProps(0, pInputProps);

	pInputProps->Release();

	return (hr == S_OK);
}
