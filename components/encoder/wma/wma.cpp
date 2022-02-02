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

#include "wma.h"
#include "config.h"

const String &BoCA::EncoderWMA::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (wmvcoredll != NIL)
	{
		componentSpecs = "									\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>Windows Media Audio Encoder</name>						\
		    <version>1.0</version>								\
		    <id>wma-enc</id>									\
		    <type>encoder</type>								\
													\
		";

		Initialize();

		IWMProfileManager	*profileManager = NIL;
		IWMCodecInfo3		*codecInfo	= NIL;

		ex_WMCreateProfileManager(&profileManager);

		if (SUCCEEDED(profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo)))
		{
			DWORD	 numCodecs = 0;

			codecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);

			for (DWORD i = 0; i < numCodecs; i++)
			{
				DWORD	 nameLen = 0;

				codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, NIL, &nameLen);

				WCHAR	*name = new WCHAR [nameLen];

				codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, name, &nameLen);

				componentSpecs.Append("							\
													\
				    <format>								\
				      <name>").Append(name).Append("</name>				\
				      <extension>wma</extension>					\
				      <tag id=\"wma-tag\" mode=\"other\">WMA Metadata</tag>		\
				    </format>								\
													\
				");

				delete [] name;
			}

			codecInfo->Release();
		}

		profileManager->Release();

		Cleanup();

		componentSpecs.Append("									\
													\
		    <input bits=\"8\" signed=\"false\"/>						\
		    <input bits=\"16-32\"/>								\
		    <parameters>									\
		      <selection name=\"Target format\" argument=\"-f %VALUE\" default=\"standard\">	\
			<option alias=\"WMA Standard\">standard</option>				\
			<option alias=\"WMA Pro\">pro</option>						\
			<option alias=\"WMA Lossless\">lossless</option>				\
			<option alias=\"WMA Voice\">voice</option>					\
			<option alias=\"Uncompressed\">uncompressed</option>				\
		      </selection>									\
		      <range name=\"VBR quality\" argument=\"-q %VALUE\" default=\"90\">		\
			<min alias=\"min\">0</min>							\
			<max alias=\"max\">100</max>							\
		      </range>										\
		      <switch name=\"Use CBR mode\" argument=\"--cbr\"/>				\
		      <selection name=\"CBR bitrate\" argument=\"-b %VALUE\" default=\"128\">		\
			<option alias=\"32 kbps\">32</option>						\
			<option alias=\"48 kbps\">48</option>						\
			<option alias=\"64 kbps\">64</option>						\
			<option alias=\"80 kbps\">80</option>						\
			<option alias=\"96 kbps\">96</option>						\
			<option alias=\"128 kbps\">128</option>						\
			<option alias=\"160 kbps\">160</option>						\
			<option alias=\"192 kbps\">192</option>						\
		      </selection>									\
		    </parameters>									\
		  </component>										\
													\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWMVCoreDLL();

	/* Register initialization and cleanup handlers.
	 */
	if (wmvcoredll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Connect(&BoCA::EncoderWMA::Initialize);
		engine->onCleanup.Connect(&BoCA::EncoderWMA::Cleanup);
	}
}

Void smooth::DetachDLL()
{
	/* Unregister initialization and cleanup handlers.
	 */
	if (wmvcoredll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Disconnect(&BoCA::EncoderWMA::Initialize);
		engine->onCleanup.Disconnect(&BoCA::EncoderWMA::Cleanup);
	}

	FreeWMVCoreDLL();
}

Void BoCA::EncoderWMA::Initialize()
{
	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

Void BoCA::EncoderWMA::Cleanup()
{
	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

BoCA::EncoderWMA::EncoderWMA()
{
	configLayer    = NIL;
	config	       = NIL;

	writer	       = NIL;
	writerAdvanced = NIL;

	writerFileSink = NIL;

	profileManager = NIL;
	profile	       = NIL;

	streamConfig   = NIL;

	samplesWritten = 0;
}

BoCA::EncoderWMA::~EncoderWMA()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Int BoCA::EncoderWMA::GetNumberOfPasses() const
{
	/* 2-pass encoding is not supported, yet.
	 */
	return 1;
}

Bool BoCA::EncoderWMA::IsLossless() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureWMA::ConfigID, "Uncompressed", False)) return True;

	/* Create profile manager and get codec info.
	 */
	IWMProfileManager	*pProfileManager = NIL;
	IWMCodecInfo3		*pCodecInfo	 = NIL;

	if (FAILED(ex_WMCreateProfileManager(&pProfileManager))) return False;

	if (FAILED(pProfileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &pCodecInfo)))
	{
		pProfileManager->Release();

		return False;
	}

	Int	 defaultCodec = GetDefaultCodec(pCodecInfo);

	/* Get and check codec name.
	 */
	DWORD	 nameLen = 0;

	pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, config->GetIntValue(ConfigureWMA::ConfigID, "Codec", defaultCodec), NIL, &nameLen);

	WCHAR	*name = new WCHAR [nameLen];

	pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, config->GetIntValue(ConfigureWMA::ConfigID, "Codec", defaultCodec), name, &nameLen);

	Bool	 result = False;

	if (String(name).Contains("Lossless")) result = True;

	delete [] name;

	/* Release profile manager and codec info.
	 */
	pCodecInfo->Release();
	pProfileManager->Release();

	return result;
}

Bool BoCA::EncoderWMA::Activate()
{
	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	/* Close output file as it will be written directly by Windows Media.
	 */
	driver->Close();

	/* Create WMA writer object.
	 */
	if (FAILED(ex_WMCreateWriter(NIL, &writer))) return False;

	if (FAILED(writer->QueryInterface(IID_IWMWriterAdvanced, (void **) &writerAdvanced)))
	{
		writer->Release();

		return False;
	}

	/* A temporary file is needed if the path exceeds 255 characters.
	 */
	String	 outputFile = Directory::MakeExtendedPath(File(track.outputFile));

	if (track.outputFile.Length() > 255) outputFile = Utilities::GetNonUnicodeTempFileName(track.outputFile);

	/* Create and open file sink.
	 */
	ex_WMCreateWriterFileSink(&writerFileSink);

	writerFileSink->Open(outputFile);

	writerAdvanced->AddSink(writerFileSink);

	/* Create profile manager.
	 */
	ex_WMCreateProfileManager(&profileManager);

	profileManager->CreateEmptyProfile(WMT_VER_9_0, &profile);

	/* Query codec info.
	 */
	IWMCodecInfo3	*pCodecInfo = NIL;

	profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &pCodecInfo);

	Int	 codec		    = config->GetIntValue(ConfigureWMA::ConfigID, "Codec", GetDefaultCodec(pCodecInfo));
	Int	 codecFormat	    = config->GetIntValue(ConfigureWMA::ConfigID, "CodecFormat", 0);
	Bool	 uncompressedFormat = config->GetIntValue(ConfigureWMA::ConfigID, "Uncompressed", False);
	Bool	 autoSelectFormat   = config->GetIntValue(ConfigureWMA::ConfigID, "AutoSelectFormat", True);
	Bool	 enableVBR	    = config->GetIntValue(ConfigureWMA::ConfigID, "EnableVBR", True);
	Bool	 enable2Pass	    = config->GetIntValue(ConfigureWMA::ConfigID, "Enable2Pass", False);

	DWORD	 numCodecs = 0;

	pCodecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);
	
	if (codec >= (Int) numCodecs)
	{
		codec		 = GetDefaultCodec(pCodecInfo);
		autoSelectFormat = True;
	}

	if (codec == -1 || numCodecs == 0) uncompressedFormat = True;

	/* Create stream configuration.
	 */
	if (uncompressedFormat)
	{
		profile->CreateNewStream(WMMEDIATYPE_Audio, &streamConfig);
	}
	else if (!autoSelectFormat)
	{
		BOOL	 falseValue = FALSE;
		BOOL	 trueValue = TRUE;
		DWORD	 oneValue = 1;
		DWORD	 twoValue = 2;

		if (enableVBR)   pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codec, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue,  sizeof(BOOL));
		else		 pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codec, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));

		if (enable2Pass) pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codec, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));
		else		 pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codec, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

		pCodecInfo->GetCodecFormat(WMMEDIATYPE_Audio, codec, codecFormat, &streamConfig);
	}
	else
	{
		streamConfig = GetBestCodecFormat(pCodecInfo, codec, track.GetFormat());
	}

	/* Check stream configuration.
	 */
	if (!streamConfig)
	{
		errorState  = True;
		errorString = "Could not create codec configuration.";
	}

	/* Set up profile and writer.
	 */
	if (!errorState)
	{
		streamConfig->SetStreamNumber(1);
		streamConfig->SetStreamName(String("Audio"));
		streamConfig->SetConnectionName(String("Audio"));

		profile->AddStream(streamConfig);

		writer->SetProfile(profile);

		if (SetInputFormat(writer, track.GetFormat()) == False)
		{
			errorState  = True;
			errorString = "Incompatible sample format.";
		}
	}

	pCodecInfo->Release();

	/* Start writing output file.
	 */
	if (!errorState)
	{
		if (FAILED(writer->BeginWriting()))
		{
			errorState  = True;
			errorString = "Could not initialize encoder.";
		}
	}

	/* Release objects in case of an error.
	 */
	if (errorState)
	{
		if (streamConfig) streamConfig->Release();

		profile->Release();
		profileManager->Release();

		writerFileSink->Release();

		writerAdvanced->Release();
		writer->Release();
	}

	return !errorState;
}

Bool BoCA::EncoderWMA::Deactivate()
{
	String	 outputFile = track.outputFile;

	if (track.outputFile.Length() > 255) outputFile = Utilities::GetNonUnicodeTempFileName(track.outputFile);

	/* Delete output file in case of an error.
	 */
	if (errorState)
	{
		File(outputFile).Delete();

		return True;
	}

	/* Finish writing output file.
	 */
	HRESULT	 hr = writer->EndWriting();

	writerAdvanced->RemoveSink(writerFileSink);

	streamConfig->Release();

	profile->Release();
	profileManager->Release();

	writerFileSink->Release();

	writerAdvanced->Release();
	writer->Release();

	if (FAILED(hr))
	{
		File(outputFile).Delete();

		return False;
	}

	/* Write metadata to file.
	 */
	if (config->GetIntValue("Tags", "EnableWMAMetadata", True))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("wma-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(config);
				tagger->RenderStreamInfo(outputFile, track);

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Move actual output file in place.
	 */
	if (outputFile != track.outputFile)
	{
		File(track.outputFile).Delete();
		File(outputFile).Move(track.outputFile);
	}

	return True;
}

Int BoCA::EncoderWMA::WriteData(Buffer<UnsignedByte> &data)
{
	INSSBuffer	*pSample = NIL;

	if (FAILED(writer->AllocateSample(data.Size(), &pSample))) return -1;

	BYTE	*buffer = NIL;

	if (!FAILED(pSample->GetBuffer(&buffer)))
	{
		memcpy(buffer, data, data.Size());

		pSample->SetLength(data.Size());

		const Format	&format	       = track.GetFormat();
		QWORD		 cnsSampleTime = samplesWritten * 10000000 / format.channels / format.rate;

		HRESULT	 hr = writer->WriteSample(0, cnsSampleTime, WM_SF_CLEANPOINT, pSample);

		pSample->Release();

		if (FAILED(hr)) return -1;

		samplesWritten += data.Size() / (format.bits / 8);
	}

	return data.Size();
}

Bool BoCA::EncoderWMA::NextPass()
{
	/* 2-pass encoding is not supported, yet.
	 */
	return False;
}

Bool BoCA::EncoderWMA::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigureWMA::ConfigID, "Uncompressed", False);
	config->SetIntValue(ConfigureWMA::ConfigID, "Codec", n);

	return True;
}

Bool BoCA::EncoderWMA::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "wma-enc";

	/* Create codec info object.
	 */
	IWMProfileManager	*pProfileManager = NIL;
	IWMCodecInfo3		*pCodecInfo	 = NIL;

	if (FAILED(ex_WMCreateProfileManager(&pProfileManager))) return False;

	if (FAILED(pProfileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &pCodecInfo)))
	{
		pProfileManager->Release();

		return False;
	}

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureWMA::ConfigID, "Uncompressed", False);
		config->SetIntValue(ConfigureWMA::ConfigID, "AutoSelectFormat", True);

		config->SetIntValue(ConfigureWMA::ConfigID, "EnableVBR", True);

		config->SetIntValue(ConfigureWMA::ConfigID, "Bitrate", 128);
		config->SetIntValue(ConfigureWMA::ConfigID, "Quality", 90);

		config->SetIntValue(ConfigureWMA::ConfigID, "Codec", GetDefaultCodec(pCodecInfo));
	}

	/* Get number of codecs.
	 */
	DWORD	 numCodecs = 0;

	pCodecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);

	/* Get command line settings.
	 */
	Bool	 useCBR	    = config->GetIntValue(encoderID, "Use CBR mode", !config->GetIntValue(ConfigureWMA::ConfigID, "EnableVBR", True));

	Int	 bitrate    = config->GetIntValue(ConfigureWMA::ConfigID, "Bitrate", 128);
	Int	 quality    = config->GetIntValue(ConfigureWMA::ConfigID, "Quality", 90);

	Int	 format	    = config->GetIntValue(ConfigureWMA::ConfigID, "Codec", GetDefaultCodec(pCodecInfo));
	String	 formatName = config->GetIntValue(ConfigureWMA::ConfigID, "Uncompressed", False) ? "uncompressed" : "standard";

	if (formatName != "uncompressed")
	{
		DWORD	 nameLen = 0;

		pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, format, NIL, &nameLen);

		WCHAR	*name = new WCHAR [nameLen];

		pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, format, name, &nameLen);

		if	(String(name).Contains("Pro"))	    formatName = "pro";
		else if (String(name).Contains("Lossless")) formatName = "lossless";
		else if (String(name).Contains("Voice"))    formatName = "voice";
	}

	if (config->GetIntValue(encoderID, "Set CBR bitrate", False))	bitrate    = config->GetIntValue(encoderID, "CBR bitrate", bitrate);
	if (config->GetIntValue(encoderID, "Set VBR quality", False))	quality    = config->GetIntValue(encoderID, "VBR quality", quality);
	if (config->GetIntValue(encoderID, "Set Target format", False))	formatName = config->GetStringValue(encoderID, "Target format", formatName).ToLower();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureWMA::ConfigID, "Uncompressed", formatName == "uncompressed");

	config->SetIntValue(ConfigureWMA::ConfigID, "EnableVBR", !useCBR);

	config->SetIntValue(ConfigureWMA::ConfigID, "Bitrate", Math::Max(32, Math::Min(192, bitrate)));
	config->SetIntValue(ConfigureWMA::ConfigID, "Quality", Math::Max(0, Math::Min(100, quality)));

	for (DWORD i = 0; i < numCodecs; i++)
	{
		DWORD	 nameLen = 0;

		pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, i, NIL, &nameLen);

		WCHAR	*name = new WCHAR [nameLen];

		pCodecInfo->GetCodecName(WMMEDIATYPE_Audio, i, name, &nameLen);

		if	(formatName == "pro"      && String(name).Contains("Pro"))	format = i;
		else if (formatName == "lossless" && String(name).Contains("Lossless")) format = i;
		else if (formatName == "voice"    && String(name).Contains("Voice"))    format = i;

		delete [] name;
	}

	config->SetIntValue(ConfigureWMA::ConfigID, "Codec", format);

	/* Release codec info object.
	 */
	pCodecInfo->Release();
	pProfileManager->Release();

	return True;
}

ConfigLayer *BoCA::EncoderWMA::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWMA();

	return configLayer;
}

/* Select default codec to be used when no codec is set.
 */
Int BoCA::EncoderWMA::GetDefaultCodec(IWMCodecInfo3 *codecInfo)
{
	Int	 index	   = -1;

	DWORD	 numCodecs = 0;
	HRESULT	 hr	   = codecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);

	for (DWORD i = 0; i < numCodecs; i++)
	{
		DWORD	 nameLen = 0;

		codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, NIL, &nameLen);

		WCHAR	*name = new WCHAR [nameLen];

		codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, name, &nameLen);

		if ( String(name).Contains("Windows Media Audio") &&
		    !String(name).Contains("Voice")		  &&
		    !String(name).Contains("Lossless")		  &&
		    !String(name).Contains("Pro")) index = i;

		delete [] name;
	}

	if (hr != S_OK) index = -1;

	return index;
}

/* This method will return the format best matching
 * our requirements for a specified codec.
 */
IWMStreamConfig *BoCA::EncoderWMA::GetBestCodecFormat(IWMCodecInfo3 *pCodecInfo, DWORD codecIndex, const Format &format) const
{
	BOOL	 falseValue   = FALSE;
	BOOL	 trueValue    = TRUE;
	DWORD	 oneValue     = 1;
	DWORD	 twoValue     = 2;

	Bool	 supportCBR   = False;
	Bool	 supportVBR   = False;
	Bool	 support1Pass = True;
	Bool	 support2Pass = False;

	/* Check if CBR is supported.
	 */
	{
		pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));
		pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

		DWORD	 numFormats = 0;
		HRESULT	 hr	    = pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

		if (!FAILED(hr) && numFormats > 0) supportCBR = True;
	}

	/* Check if VBR is supported.
	 */
	{
		WMT_ATTR_DATATYPE	 valueType = WMT_TYPE_BOOL;
		DWORD			 valueSize = sizeof(BOOL);
		BOOL			 isVBRSupported = FALSE;

		pCodecInfo->GetCodecProp(WMMEDIATYPE_Audio, codecIndex, g_wszIsVBRSupported, &valueType, (BYTE *) &isVBRSupported, &valueSize);

		if (isVBRSupported)
		{
			pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue, sizeof(BOOL));
			pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

			DWORD	 numFormats = 0;
			HRESULT	 hr	    = pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

			if (!FAILED(hr) && numFormats > 0) supportVBR = True;
		}
	}

	if (supportVBR && (!supportCBR || config->GetIntValue(ConfigureWMA::ConfigID, "EnableVBR", True)))	  pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue,  sizeof(BOOL));
	else													  pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));

	if (support2Pass && (!support1Pass || config->GetIntValue(ConfigureWMA::ConfigID, "Enable2Pass", False))) pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));
	else													  pCodecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

	IWMStreamConfig	*result		     = NIL;

	Int		 targetBitrate	     = config->GetIntValue(ConfigureWMA::ConfigID, "Bitrate", 128) * 1000;
	Int		 targetQuality	     = config->GetIntValue(ConfigureWMA::ConfigID, "Quality", 90);

	Bool		 useVBR		     = (supportVBR && config->GetIntValue(ConfigureWMA::ConfigID, "EnableVBR", True));

	DWORD		 bestMatchBitrate    = 100000000;
	DWORD		 bestMatchQuality    = 100000000;
	DWORD		 bestMatchSampleRate = 100000000;
	DWORD		 bestMatchBits	     = 100000000;
	DWORD		 bestMatchChannels   = 100000000;

	DWORD		 numFormats	     = 0;

	pCodecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

	for (DWORD i = 0; i < numFormats; i++)
	{
		IWMStreamConfig	*pStreamConfig = NIL;
		IWMMediaProps	*pMediaProps   = NIL;

		pCodecInfo->GetCodecFormat(WMMEDIATYPE_Audio, codecIndex, i, &pStreamConfig);
		pStreamConfig->QueryInterface(IID_IWMMediaProps, (void **) &pMediaProps);

		DWORD		 formatBitrate = 0;
		DWORD		 formatQuality = 0;

		pStreamConfig->GetBitrate(&formatBitrate);

		DWORD		 mediaTypeSize = 0;

		pMediaProps->GetMediaType(NIL, &mediaTypeSize);

		WM_MEDIA_TYPE	*mediaType = (WM_MEDIA_TYPE *) new BYTE [mediaTypeSize];

		pMediaProps->GetMediaType(mediaType, &mediaTypeSize);

		if (mediaType->majortype  == WMMEDIATYPE_Audio &&
		    mediaType->formattype == WMFORMAT_WaveFormatEx)
		{
			Bool		 newBestMatch = False;
			WAVEFORMATEX	*waveFormat   = (WAVEFORMATEX *) mediaType->pbFormat;

			formatQuality = waveFormat->nAvgBytesPerSec & 255;

			if	(Math::Abs(Int(waveFormat->nChannels	  - format.channels))	      < Math::Abs(Int(bestMatchChannels	  - format.channels))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate))  < Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate)) == Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate)) &&
				 Math::Abs(Int(waveFormat->wBitsPerSample - format.bits))	      < Math::Abs(Int(bestMatchBits	  - format.bits))) newBestMatch = True;
			else if (Math::Abs(Int(waveFormat->nChannels	  - format.channels))	     == Math::Abs(Int(bestMatchChannels	  - format.channels)) &&
				 Math::Abs(Int(waveFormat->nSamplesPerSec - (Unsigned) format.rate)) == Math::Abs(Int(bestMatchSampleRate - (Unsigned) format.rate)) &&
				 Math::Abs(Int(waveFormat->wBitsPerSample - format.bits))	     == Math::Abs(Int(bestMatchBits	  - format.bits)) &&
				 ((!useVBR && Math::Abs(Int(formatBitrate - targetBitrate))	      < Math::Abs(Int(bestMatchBitrate	  - targetBitrate))) ||
				  ( useVBR && Math::Abs(Int(formatQuality - targetQuality))	      < Math::Abs(Int(bestMatchQuality	  - targetQuality))))) newBestMatch = True;

			if (newBestMatch)
			{
				if (result != NIL) result->Release();

				result = pStreamConfig;
				result->AddRef();

				bestMatchChannels   = waveFormat->nChannels;
				bestMatchBits	    = waveFormat->wBitsPerSample;
				bestMatchSampleRate = waveFormat->nSamplesPerSec;
				bestMatchBitrate    = formatBitrate;
				bestMatchQuality    = formatQuality;
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
Bool BoCA::EncoderWMA::SetInputFormat(IWMWriter *pWriter, const Format &format)
{
	WM_MEDIA_TYPE	 mediaType;

	mediaType.majortype		= WMMEDIATYPE_Audio;
	mediaType.subtype		= WMMEDIASUBTYPE_PCM;
	mediaType.bFixedSizeSamples	= True;
	mediaType.bTemporalCompression	= False;
	mediaType.lSampleSize		= (format.bits / 8) * format.channels;
	mediaType.formattype		= WMFORMAT_WaveFormatEx;
	mediaType.pUnk			= NIL;

	WAVEFORMATEX	 waveFormat;

	waveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	waveFormat.nChannels		= format.channels;
	waveFormat.nSamplesPerSec	= format.rate;
	waveFormat.nAvgBytesPerSec	= format.rate * (format.bits / 8) * format.channels;
	waveFormat.nBlockAlign		= (format.bits / 8) * format.channels;
	waveFormat.wBitsPerSample	= format.bits;
	waveFormat.cbSize		= 0;

	mediaType.cbFormat		= sizeof(waveFormat);
	mediaType.pbFormat		= (BYTE *) &waveFormat;

	IWMInputMediaProps	*pInputProps = NIL;

	if (!FAILED(pWriter->GetInputProps(0, &pInputProps)))
	{
		pInputProps->SetMediaType(&mediaType);

		HRESULT	 hr = writer->SetInputProps(0, pInputProps);

		pInputProps->Release();

		return !FAILED(hr);
	}

	return False;
}
