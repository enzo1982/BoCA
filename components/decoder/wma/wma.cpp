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

#include "wma.h"

using namespace smooth::IO;

const String &BoCA::DecoderWMA::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (wmvcoredll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Windows Media Audio Decoder</name>			\
		    <version>1.0</version>					\
		    <id>wma-dec</id>						\
		    <type>decoder</type>					\
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
	LoadWMVCoreDLL();
}

Void smooth::DetachDLL()
{
	FreeWMVCoreDLL();
}

Bool BoCA::DecoderWMA::CanOpenStream(const String &streamURI)
{
	InStream	*f_in  = new InStream(STREAM_FILE, streamURI, IS_READ);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1974609456);
}

Error BoCA::DecoderWMA::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	track.fileSize	= f_in->Size();

	delete f_in;

	readerCallback = new WMAReader();

	m_hAsyncEvent = readerCallback->GetAsyncEventHandle();

	HRESULT	 hr = ex_WMCreateReader(NIL, WMT_RIGHT_PLAYBACK, &m_pReader);

	if (!FAILED(hr)) hr = m_pReader->QueryInterface(IID_IWMReaderAdvanced2, (void **) &m_pReaderAdvanced);

	if (!FAILED(hr)) hr = m_pReader->Open(streamURI, readerCallback, NIL);

	/* Wait for the Open call to complete. The event is set in the
	 * OnStatus callback when the reader reports completion.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	if (!errorState)
	{
		DWORD		 cOutputs = 0;
		WAVEFORMATEX	*pWfx	  = NIL;

		/* Find out the output count
		 */
		m_pReader->GetOutputCount(&cOutputs);

		/* Find the first audio output.
		 */
		for (DWORD i = 0; i < cOutputs; i++)
		{
			/* Enable high definition output.
			 */
			BOOL	 enableDiscreteOutput = TRUE;
			DWORD	 speakerConfig	      = 0;

			m_pReaderAdvanced->SetOutputSetting(i, g_wszEnableDiscreteOutput, WMT_TYPE_BOOL, (BYTE *) &enableDiscreteOutput, sizeof(WMT_TYPE_BOOL));
			m_pReaderAdvanced->SetOutputSetting(i, g_wszSpeakerConfig, WMT_TYPE_DWORD, (BYTE *) &speakerConfig, sizeof(WMT_TYPE_DWORD));

			/* Set the first output format as it is
			 * the one with the highest quality.
			 */
			IWMOutputMediaProps	*pProps = NIL;

			if (!FAILED(m_pReader->GetOutputFormat(i, 0, &pProps)))
			{
				if (!FAILED(m_pReader->SetOutputProps(i, pProps)))
				{
					/* Find out the space needed for pMediaType
					 */
					ULONG	 cbType = 0;

					if (!FAILED(pProps->GetMediaType(NIL, &cbType)))
					{
						WM_MEDIA_TYPE	*pMediaType = (WM_MEDIA_TYPE *) new BYTE[cbType];

						/* Get the value for MediaType
						 */
						if (!FAILED(pProps->GetMediaType(pMediaType, &cbType)) && pMediaType->majortype == WMMEDIATYPE_Audio)
						{
							/* Store the wave format for this output
							 */
							pWfx = (WAVEFORMATEX *) new BYTE[pMediaType->cbFormat];

							CopyMemory(pWfx, pMediaType->pbFormat, pMediaType->cbFormat);
						}

						delete [] pMediaType;
					}
				}

				pProps->Release();
			}

			if (pWfx != NIL) break;
		}

		if (pWfx != NIL)
		{
			Format	 format = track.GetFormat();

			format.bits	= pWfx->wBitsPerSample;
			format.rate	= pWfx->nSamplesPerSec;
			format.channels	= pWfx->nChannels;

			track.SetFormat(format);

			delete [] pWfx;
		}

		if (pWfx == NIL) { errorState = True; errorString = "Unknown file type"; }
	}

	if (!errorState)
	{
		IWMHeaderInfo	*pHeaderInfo = NIL;
		BYTE		*pbValue = NIL;

		hr = m_pReader->QueryInterface(IID_IWMHeaderInfo, (void **) &pHeaderInfo);

		/* Get attribute "Duration"
		 */
		if (!FAILED(hr)) hr = GetHeaderAttribute(pHeaderInfo, g_wszWMDuration, &pbValue);

		if (!FAILED(hr) && pbValue != NIL)
		{
			/* Set approxLength, because WMA duration is not always 100% accurate.
			 */
			Format	 format = track.GetFormat();

			track.length = -1;
			track.approxLength = *(QWORD *) pbValue * format.rate / 10000000;

			/* Try to guess if this is a lossless file.
			 */
			if (((Float) track.fileSize) / track.approxLength / format.channels / (format.bits / 8) > 0.35) track.lossless = True;
			else												track.lossless = False;

			delete [] pbValue;
		}

		pHeaderInfo->Release();
	}

	hr = m_pReader->Close();

	/* Wait for the reader to deliver a WMT_CLOSED event to the
	 * OnStatus callback.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	m_pReaderAdvanced->Release();
	m_pReader->Release();
	readerCallback->Release();

	if (!errorState)
	{
		AS::Registry		&boca	= AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("wma-tag");

		if (tagger != NIL)
		{
			tagger->SetConfiguration(GetConfiguration());
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::DecoderWMA::DecoderWMA()
{
	m_pReader	  = NIL;
	m_pReaderAdvanced = NIL;

	m_hAsyncEvent	  = NIL;

	readerCallback	  = NIL;

	packageSize	  = 0;

	userProvidedClock = True;

	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

BoCA::DecoderWMA::~DecoderWMA()
{
	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

Bool BoCA::DecoderWMA::Activate()
{
	HRESULT	 hr = ex_WMCreateReader(NIL, WMT_RIGHT_PLAYBACK, &m_pReader);

	if (!FAILED(hr)) hr = m_pReader->QueryInterface(IID_IWMReaderAdvanced2, (void **) &m_pReaderAdvanced);

	readerCallback = new WMAReader();
	readerCallback->SetReaderAdvanced(m_pReaderAdvanced);
	readerCallback->SetSamplesBuffer(&samplesBuffer, &samplesBufferMutex);

	m_hAsyncEvent = readerCallback->GetAsyncEventHandle();

	hr = m_pReader->Open(track.origFilename, readerCallback, NIL);

	/* Wait for the Open call to complete. The event is set in the
	 * OnStatus callback when the reader reports completion.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	m_pReaderAdvanced->SetUserProvidedClock(userProvidedClock);

	DWORD	 cOutputs = 0;

	/* Find out the output count
	 */
	m_pReader->GetOutputCount(&cOutputs);

	/* Find the first audio output.
	 */
	for (DWORD i = 0; i < cOutputs; i++)
	{
		/* Enable high definition output.
		 */
		BOOL	 enableDiscreteOutput = TRUE;
		DWORD	 speakerConfig	      = 0;

		m_pReaderAdvanced->SetOutputSetting(i, g_wszEnableDiscreteOutput, WMT_TYPE_BOOL, (BYTE *) &enableDiscreteOutput, sizeof(WMT_TYPE_BOOL));
		m_pReaderAdvanced->SetOutputSetting(i, g_wszSpeakerConfig, WMT_TYPE_DWORD, (BYTE *) &speakerConfig, sizeof(WMT_TYPE_DWORD));

		/* Set the first output format as it is
		 * the one with the highest quality.
		 */
		IWMOutputMediaProps	*pProps = NIL;

		if (!FAILED(m_pReader->GetOutputFormat(i, 0, &pProps)))
		{
			if (!FAILED(m_pReader->SetOutputProps(i, pProps)))
			{
				/* Find out the space needed for pMediaType
				 */
				ULONG	 cbType = 0;

				if (!FAILED(pProps->GetMediaType(NIL, &cbType)))
				{
					WM_MEDIA_TYPE	*pMediaType = (WM_MEDIA_TYPE *) new BYTE[cbType];

					/* Get the value for MediaType
					 */
					if (!FAILED(pProps->GetMediaType(pMediaType, &cbType)) && pMediaType->majortype == WMMEDIATYPE_Audio)
					{
						readerCallback->SetAudioOutputNum(i);

						hr = m_pReader->Start(0, 0, 1.0, NIL);

						/* Wait for the Start call to complete.
						 */
						if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);
					}

					delete [] pMediaType;
				}
			}

			pProps->Release();
		}
	}

	/* Some streams do not work with a user provided clock.
	 * Detect this here and try without it in case of an error.
	 */
	while (readerCallback->IsActive() && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	if (!readerCallback->IsActive() && samplesBuffer.Size() <= 0)
	{
		if (!userProvidedClock) return False;

		Deactivate();

		userProvidedClock = False;
		errorState	  = False;

		return Activate();
	}

	return True;
}

Bool BoCA::DecoderWMA::Deactivate()
{
	readerCallback->SetActive(False);

	HRESULT	 hr = m_pReader->Stop();

	/* Wait for the reader to stop.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	hr = m_pReader->Close();

	/* Wait for the reader to deliver a WMT_CLOSED event to the
	 * OnStatus callback.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	m_pReaderAdvanced->Release();
	m_pReader->Release();
	readerCallback->Release();

	return True;
}

Bool BoCA::DecoderWMA::Seek(Int64 samplePosition)
{
	/* Stop reader before seeking.
	 */
	readerCallback->SetActive(False);

	HRESULT	 hr = m_pReader->Stop();

	/* Wait for the reader to stop.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	/* Clear samples buffer.
	 */
	samplesBuffer.Resize(0);

	/* Restart reader at new position.
	 */
	QWORD	 position = samplePosition * 10000000 / track.GetFormat().rate;

	hr = m_pReader->Start(position, 0, 1.0, &position);

	/* Wait for the Start call to complete.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	return True;
}

Int BoCA::DecoderWMA::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (!readerCallback->IsActive() && samplesBuffer.Size() <= 0) return -1;

	while (readerCallback->IsActive() && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	/* Copy any data from the sample buffer.
	 */
	samplesBufferMutex.Lock();

	data.Resize(samplesBuffer.Size());

	memcpy(data, samplesBuffer, samplesBuffer.Size());

	samplesBuffer.Resize(0);

	samplesBufferMutex.Release();

	/* Update inBytes to indicate progress.
	 */
	inBytes += track.fileSize * data.Size() / (track.approxLength * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return data.Size();
}

HRESULT BoCA::DecoderWMA::GetHeaderAttribute(IWMHeaderInfo *pHeaderInfo, LPCWSTR pwszName, BYTE **ppbValue)
{
	WMT_ATTR_DATATYPE	 wmtType;
	WORD			 wStreamNum = 0;
	WORD			 cbLength = 0;

	*ppbValue = NIL;

	/* Get the number of bytes to be allocated for pbValue
	 */
	HRESULT	 hr = pHeaderInfo->GetAttributeByName(&wStreamNum, pwszName, &wmtType, NIL, &cbLength);

	/* No such an attribute, so return
	 */
	if (hr == ASF_E_NOTFOUND) return S_OK;

	if (FAILED(hr)) return hr;

	/* Allocate space and get the actual value
	 */
	*ppbValue = new BYTE[cbLength];

	pHeaderInfo->GetAttributeByName(&wStreamNum, pwszName, &wmtType, *ppbValue, &cbLength);

	return S_OK;
}

Void BoCA::DecoderWMA::WaitForEvent(HANDLE hEvent, DWORD msMaxWaitTime)
{
	for (DWORD i = 0; i < msMaxWaitTime; i += 10)
	{
		MSG	 msg;

		if (PeekMessage(&msg, (HWND) NIL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (WaitForSingleObject(hEvent, 10) != WAIT_TIMEOUT) break;
	}

	if (readerCallback->IsError())
	{
		errorState = True;
		errorString = readerCallback->GetErrorString();
	}
}
