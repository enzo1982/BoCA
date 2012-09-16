 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "wma_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::WMAIn::GetComponentSpecs()
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
		    <id>wma-in</id>						\
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

Bool BoCA::WMAIn::CanOpenStream(const String &streamURI)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READ);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1974609456);
}

Error BoCA::WMAIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READ);

	track.fileSize	= f_in->Size();

	delete f_in;

	HRESULT	 hr = NIL;

	readerCallback = new WMAReader();

	m_hAsyncEvent = readerCallback->GetAsyncEventHandle();

	hr = ex_WMCreateReader(NIL, WMT_RIGHT_PLAYBACK, &m_pReader);

	hr = m_pReader->Open(streamURI, readerCallback, NIL);

	/* Wait for the Open call to complete. The event is set in the
	 * OnStatus callback when the reader reports completion.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	if (!errorState)
	{
		DWORD		 cOutputs = 0;
		WAVEFORMATEX	*pWfx = NIL;

		/* Find out the output count
		 */
		hr = m_pReader->GetOutputCount(&cOutputs);

		/* Find the first audio output.
		 */
		for (DWORD i = 0; i < cOutputs; i++)
		{
			IWMOutputMediaProps	*pProps = NIL;
			WM_MEDIA_TYPE		*pMediaType = NIL;
			ULONG			 cbType = 0;

			/* Set the first output format as it is
			 * the one with the highest quality.
			 */
			hr = m_pReader->GetOutputFormat(i, 0, &pProps);
			hr = m_pReader->SetOutputProps(i, pProps);

			/* Find out the space needed for pMediaType
			 */
			hr = pProps->GetMediaType(NIL, &cbType);

			pMediaType = (WM_MEDIA_TYPE *) new BYTE[cbType];

			/* Get the value for MediaType
			 */
			hr = pProps->GetMediaType(pMediaType, &cbType);

			if (pMediaType->majortype == WMMEDIATYPE_Audio)
			{
				/* Store the wave format for this output
				 */
				pWfx = (WAVEFORMATEX *) new BYTE[pMediaType->cbFormat];

				CopyMemory(pWfx, pMediaType->pbFormat, pMediaType->cbFormat);
			}

			pProps->Release();

			delete [] pMediaType;

			if (pWfx != NIL) break;
		}

		if (pWfx != NIL)
		{
			Format	 format = track.GetFormat();

			format.order	= BYTE_INTEL;
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
		hr = GetHeaderAttribute(pHeaderInfo, g_wszWMDuration, &pbValue);

		if (!FAILED(hr) && pbValue != NIL)
		{
			/* Set approxLength, because WMA duration is not always 100% accurate.
			 */
			track.length = -1;
			track.approxLength = *(QWORD *) pbValue * track.GetFormat().rate / 10000000;

			delete [] pbValue;
		}

		pHeaderInfo->Release();
	}

	hr = m_pReader->Close();

	/* Wait for the reader to deliver a WMT_CLOSED event to the
	 * OnStatus callback.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	m_pReader->Release();
	readerCallback->Release();

	if (!errorState)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("wma-tag");

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	if (errorState)	return Error();
	else		return Success();
}

BoCA::WMAIn::WMAIn()
{
	m_pReader = NIL;

	packageSize = 0;

	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

BoCA::WMAIn::~WMAIn()
{
	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

Bool BoCA::WMAIn::Activate()
{
	HRESULT	 hr = NIL;

	hr = ex_WMCreateReader(NIL, WMT_RIGHT_PLAYBACK, &m_pReader);

	hr = m_pReader->QueryInterface(IID_IWMReaderAdvanced, (void **) &m_pReaderAdvanced);

	readerCallback = new WMAReader();
	readerCallback->SetReaderAdvanced(m_pReaderAdvanced);
	readerCallback->SetSamplesBuffer(&samplesBuffer, &samplesBufferMutex);

	m_hAsyncEvent = readerCallback->GetAsyncEventHandle();

	hr = m_pReader->Open(track.origFilename, readerCallback, NIL);

	/* Wait for the Open call to complete. The event is set in the
	 * OnStatus callback when the reader reports completion.
	 */
	if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);

	hr = m_pReaderAdvanced->SetUserProvidedClock(true);

	DWORD	 cOutputs = 0;

	/* Find out the output count
	 */
	hr = m_pReader->GetOutputCount(&cOutputs);

	/* Find the first audio output.
	 */
	for (DWORD i = 0; i < cOutputs; i++)
	{
		IWMOutputMediaProps	*pProps = NIL;
		WM_MEDIA_TYPE		*pMediaType = NIL;
		ULONG			 cbType = 0;

		/* Set the first output format as it is
		 * the one with the highest quality.
		 */
		hr = m_pReader->GetOutputFormat(i, 0, &pProps);
		hr = m_pReader->SetOutputProps(i, pProps);

		/* Find out the space needed for pMediaType
		 */
		hr = pProps->GetMediaType(NIL, &cbType);

		pMediaType = (WM_MEDIA_TYPE *) new BYTE[cbType];

		/* Get the value for MediaType
		 */
		hr = pProps->GetMediaType(pMediaType, &cbType);

		if (pMediaType->majortype == WMMEDIATYPE_Audio)
		{
			readerCallback->SetAudioOutputNum(i);

			hr = m_pReader->Start(0, 0, 1.0, NIL);

			/* Wait for the Start call to complete.
			 */
			if (!FAILED(hr)) WaitForEvent(m_hAsyncEvent);
		}

		pProps->Release();

		delete [] pMediaType;
	}

	return True;
}

Bool BoCA::WMAIn::Deactivate()
{
	readerCallback->SetActive(False);

	HRESULT	 hr = NIL;

	hr = m_pReader->Stop();

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

Int BoCA::WMAIn::ReadData(Buffer<UnsignedByte> &data, Int size)
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

HRESULT BoCA::WMAIn::GetHeaderAttribute(IWMHeaderInfo *pHeaderInfo, LPCWSTR pwszName, BYTE **ppbValue)
{
	HRESULT			 hr = S_OK;
	WMT_ATTR_DATATYPE	 wmtType;
	WORD			 wStreamNum = 0;
	WORD			 cbLength = 0;

	*ppbValue = NIL;

	/* Get the number of bytes to be allocated for pbValue
	 */
	hr = pHeaderInfo->GetAttributeByName(&wStreamNum, pwszName, &wmtType, NIL, &cbLength);

	/* No such an attribute, so return
	 */
	if (hr == ASF_E_NOTFOUND) return S_OK;

	if (FAILED(hr)) return hr;

	/* Allocate space and get the actual value
	 */
	*ppbValue = new BYTE[cbLength];

	hr = pHeaderInfo->GetAttributeByName(&wStreamNum, pwszName, &wmtType, *ppbValue, &cbLength);

	return S_OK;
}

Void BoCA::WMAIn::WaitForEvent(HANDLE hEvent, DWORD msMaxWaitTime)
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
