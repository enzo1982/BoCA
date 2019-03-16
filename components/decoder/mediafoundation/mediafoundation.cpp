 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include <smooth/io/drivers/driver_posix.h> 

#include <mfapi.h>
#include <propvarutil.h>

#include "mediafoundation.h"
#include "mfbytestream.h"

using namespace smooth::IO;

const String &BoCA::DecoderMediaFoundation::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (mfplatdll != NIL && mfreadwritedll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Media Foundation Decoder</name>			\
		    <version>1.0</version>					\
		    <id>mediafoundation-dec</id>				\
		    <type>decoder</type>					\
		    <precede>wma-dec</precede>					\
		    <format>							\
		      <name>Windows Media Audio Files</name>			\
		      <extension>wma</extension>				\
		      <tag id=\"wma-tag\" mode=\"other\">WMA Metadata</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMFPlatDLL();
	LoadMFReadWriteDLL();

	/* Register initialization and cleanup handlers.
	 */
	if (mfplatdll != NIL && mfreadwritedll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Connect(&BoCA::DecoderMediaFoundation::Initialize);
		engine->onCleanup.Connect(&BoCA::DecoderMediaFoundation::Cleanup);
	}
}

Void smooth::DetachDLL()
{
	/* Unregister initialization and cleanup handlers.
	 */
	if (mfplatdll != NIL && mfreadwritedll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Disconnect(&BoCA::DecoderMediaFoundation::Initialize);
		engine->onCleanup.Disconnect(&BoCA::DecoderMediaFoundation::Cleanup);
	}

	FreeMFPlatDLL();
	FreeMFReadWriteDLL();
}

Void BoCA::DecoderMediaFoundation::Initialize()
{
	/* Init COM and Media Foundation.
	 */
	CoInitialize(NIL);

	ex_MFStartup(MF_VERSION, MFSTARTUP_FULL);
}

Void BoCA::DecoderMediaFoundation::Cleanup()
{
	/* Uninit Media Foundation and COM.
	 */
	ex_MFShutdown();

	CoUninitialize();
}

Bool BoCA::DecoderMediaFoundation::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);
	UnsignedInt64	 magic = in.InputNumber(8);

	return (magic == 0x11CF668E75B22630);
}

Error BoCA::DecoderMediaFoundation::GetStreamInfo(const String &streamURI, Track &track)
{
	DriverPOSIX	 driver(streamURI, IS_READ);

	/* Wrap stream in an IMFByteStream interface and create source reader.
	 */
	IMFByteStream	*byteStream = new MFByteStream(&driver);
	HRESULT		 hr	    = ex_MFCreateSourceReaderFromByteStream(byteStream, NULL, &reader);

	byteStream->Release();

	if (FAILED(hr)) return Error();

	/* Select first audio stream and get input media type.
	 */
	IMFMediaType	*inputType  = NULL;

	reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
	reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &inputType);

	/* Fill format descriptor with input type.
	 */
	WAVEFORMATEX	 wfx	    = { };

	wfx.wFormatTag	    = WAVE_FORMAT_PCM;
	wfx.nChannels	    = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_NUM_CHANNELS, 0);
	wfx.nSamplesPerSec  = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
	wfx.wBitsPerSample  = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
	wfx.nBlockAlign	    = wfx.nChannels * (wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	inputType->Release();

	/* Create output media type.
	 */
	IMFMediaType	*outputType = NULL;

	ex_MFCreateMediaType(&outputType);
	ex_MFInitMediaTypeFromWaveFormatEx(outputType, &wfx, sizeof(wfx));

	/* Set output type and get resulting media type.
	 */
	reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, outputType);
	reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &mediaType);

	outputType->Release();

	/* Fill format from media type.
	 */
	Format	 format = track.GetFormat();

	format.rate	= MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
	format.bits	= MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
	format.channels	= MFGetAttributeUINT32(mediaType, MF_MT_AUDIO_NUM_CHANNELS, 0);

	track.SetFormat(format);

	mediaType->Release();

	/* Get track duration.
	 */
	PROPVARIANT	 duration;

	if (!FAILED(reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &duration)))
	{
		/* Set approxLength, because WMA duration is not always 100% accurate.
		 */
		track.length	   = -1;
		track.approxLength = Math::Round(Float(duration.hVal.QuadPart) / 1e7 * format.rate);
		track.fileSize	   = driver.GetSize();

		/* Try to guess if this is a lossless file.
		 */
		if (Float(track.fileSize) / track.approxLength / format.channels / (format.bits / 8) > 0.35) track.lossless = True;
		else											     track.lossless = False;
	}

	driver.Close();

	/* Read tags.
	 */
	AS::Registry		&boca	= AS::Registry::Get();
	AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("wma-tag");

	if (tagger != NIL)
	{
		tagger->SetConfiguration(GetConfiguration());
		tagger->ParseStreamInfo(streamURI, track);

		boca.DeleteComponent(tagger);
	}

	return Success();
}

BoCA::DecoderMediaFoundation::DecoderMediaFoundation()
{
	reader	  = NIL;
	mediaType = NIL;
}

BoCA::DecoderMediaFoundation::~DecoderMediaFoundation()
{
}

Bool BoCA::DecoderMediaFoundation::Activate()
{
	/* Wrap stream in an IMFByteStream interface and create source reader.
	 */
	IMFByteStream	*byteStream = new MFByteStream(driver);
	HRESULT		 hr	    = ex_MFCreateSourceReaderFromByteStream(byteStream, NULL, &reader);

	byteStream->Release();

	if (FAILED(hr)) return False;

	/* Select first audio stream and get input media type.
	 */
	IMFMediaType	*inputType  = NULL;

	reader->SetStreamSelection(MF_SOURCE_READER_FIRST_AUDIO_STREAM, TRUE);
	reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &inputType);

	/* Fill format descriptor with input type.
	 */
	WAVEFORMATEX	 wfx	    = { };

	wfx.wFormatTag	    = WAVE_FORMAT_PCM;
	wfx.nChannels	    = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_NUM_CHANNELS, 0);
	wfx.nSamplesPerSec  = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
	wfx.wBitsPerSample  = MFGetAttributeUINT32(inputType, MF_MT_AUDIO_BITS_PER_SAMPLE, 16);
	wfx.nBlockAlign	    = wfx.nChannels * (wfx.wBitsPerSample / 8);
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;

	inputType->Release();

	/* Create output media type.
	 */
	IMFMediaType	*outputType = NULL;

	ex_MFCreateMediaType(&outputType);
	ex_MFInitMediaTypeFromWaveFormatEx(outputType, &wfx, sizeof(wfx));

	/* Set output type and get resulting media type.
	 */
	reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, NULL, outputType);
	reader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, &mediaType);

	outputType->Release();

	return True;
}

Bool BoCA::DecoderMediaFoundation::Deactivate()
{
	mediaType->Release();
	reader->Release();

	return True;
}

Bool BoCA::DecoderMediaFoundation::Seek(Int64 samplePosition)
{
	/* Seek to the requested position.
	 */
	PROPVARIANT	 position;

	InitPropVariantFromInt64(Float(samplePosition) * 1e7 / track.GetFormat().rate, &position);

	HRESULT	 hr = reader->SetCurrentPosition(GUID_NULL, position);

	if (FAILED(hr))	return False;

	return True;
}

Int BoCA::DecoderMediaFoundation::ReadData(Buffer<UnsignedByte> &data)
{
	/* Read next sample.
	 */
	IMFSample	*sample	     = NULL;
	DWORD		 streamFlags = 0;
	HRESULT		 hr	     = reader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, NULL, &streamFlags, NULL, &sample);

	if (FAILED(hr)) return -1;

	if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) return 0;

	/* Copy sample to data buffer.
	 */
	IMFMediaBuffer	*buffer = NULL;

	sample->ConvertToContiguousBuffer(&buffer);

	BYTE		*bytes	= NULL;
	DWORD		 length = 0;

	buffer->Lock(&bytes, NULL, &length);

	data.Resize(length);

	memcpy(data, bytes, length);

	buffer->Unlock();
	buffer->Release();

	sample->Release();

	/* Update inBytes to indicate progress.
	 */
	if (track.approxLength > 0) inBytes += track.fileSize * data.Size() / (track.approxLength * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return data.Size();
}
