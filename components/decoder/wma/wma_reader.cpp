 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "wma_reader.h"

BoCA::WMAReader::WMAReader()
{
	m_cRef = 1;

	m_fEOF = false;
	m_qwTime = 0;
	m_dwAudioOutputNum = 0;

	m_hAsyncEvent = CreateEvent(NIL, FALSE, FALSE, NIL);

	m_pReaderAdvanced = NIL;

	active = False;
	error = False;

	samplesBuffer = NIL;
	samplesBufferMutex = NIL;
}

BoCA::WMAReader::~WMAReader()
{
	CloseHandle(m_hAsyncEvent);
}

HRESULT BoCA::WMAReader::QueryInterface(REFIID riid, void **ppvObject) 
{
	if (ppvObject == NIL) return E_INVALIDARG;

	if (riid == IID_IWMReaderCallback)
	{
		*ppvObject = static_cast<IWMReaderCallback *>(this);
	}
	else if (riid == IID_IWMReaderCallbackAdvanced)
	{
		*ppvObject = static_cast<IWMReaderCallbackAdvanced *>(this);
	}
	else if (riid == IID_IUnknown)
	{
		*ppvObject = static_cast<IWMReaderCallback *>(this);
	}
	else
	{
		*ppvObject = NIL;

		return E_NOINTERFACE;
	}

	AddRef();

	return S_OK;
}

ULONG BoCA::WMAReader::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

ULONG BoCA::WMAReader::Release()
{
	if (InterlockedDecrement(&m_cRef) == 0)
	{
		delete this;

		return 0;
	}

	return m_cRef;
}

HRESULT BoCA::WMAReader::OnStatus(WMT_STATUS status, HRESULT hr, WMT_ATTR_DATATYPE dwType, BYTE __RPC_FAR *pValue, void __RPC_FAR *pvContext)
{
	/* This switch checks for the important messages sent by the reader object.
	 */
	switch (status)
	{
		/* The reader is finished opening a file.
		 */
		case WMT_OPENED:
			SetAsyncEvent(hr);

			break;

		/* The reader is finished closing a file.
		 */
		case WMT_CLOSED:
			SetAsyncEvent(hr);

			break;

		/* Playback of the opened file has begun.
		 */
		case WMT_STARTED:
			m_qwTime = 10000000;

			m_pReaderAdvanced->DeliverTime(m_qwTime);

			active = True;

			SetAsyncEvent(hr);

			break;

		/* The previously playing reader has stopped.
		 */
		case WMT_STOPPED:
			active = False;

			SetAsyncEvent(hr);

			break;

		case WMT_EOF:
			m_fEOF = true;

			break;

		/* This class reacts to any errors by changing its state to stopped.
		 */
		case WMT_ERROR:
			active = False;
			error = True;

			break;

		case WMT_MISSING_CODEC:
			error = True;
			errorString = "Missing appropriate codec";

			break;

		case WMT_NO_RIGHTS:
		case WMT_NO_RIGHTS_EX:
			error = True;
			errorString = "Cannot open protected files";

			break;

		/* Do nothing on other events.
		 */
		default:
			break;
	}

	return S_OK;
}

HRESULT BoCA::WMAReader::OnSample(DWORD dwOutputNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer *pSample, void *pvContext)
{
	/* Check the output number of the sample against the stored output number.
	 * Because only the first audio output is stored, all other outputs,
	 * regardless of type, will be ignored.
	 */
	if (dwOutputNum != m_dwAudioOutputNum)
	{
		return S_OK;
	}

	BYTE	*pData = NIL;
	DWORD	 cbData = 0;
	HRESULT	 hr = S_OK;

	/* Get the sample from the buffer object.
	 */
	hr = pSample->GetBufferAndLength( &pData, &cbData );

	while (IsActive() && samplesBuffer->Size() >= 131072) S::System::System::Sleep(0);

	/* Copy the sample to the sample buffer.
	 */
	if (IsActive())
	{
		samplesBufferMutex->Lock();

		samplesBuffer->Resize(samplesBuffer->Size() + cbData);

		memcpy((UnsignedByte *) *samplesBuffer + samplesBuffer->Size() - cbData, pData, cbData);

		samplesBufferMutex->Release();
	}

	return S_OK;
}

HRESULT BoCA::WMAReader::OnTime(QWORD cnsCurrentTime, void *pvContext)
{
	HRESULT	 hr = S_OK;

	/*  Keep asking for the specific duration of the stream till EOF
	 */
	if (m_fEOF == false)
	{
		m_qwTime += 10000000; // 1 second

		hr = m_pReaderAdvanced->DeliverTime(m_qwTime);
	}

	return hr;
}

Void BoCA::WMAReader::SetAsyncEvent(HRESULT hrAsync)
{
	SetEvent(m_hAsyncEvent);
}

Bool BoCA::WMAReader::IsActive()
{
	return active;
}

Void BoCA::WMAReader::SetActive(Bool nActive)
{
	active = nActive;
}

Bool BoCA::WMAReader::IsError()
{
	return error;
}

const String &BoCA::WMAReader::GetErrorString()
{
	return errorString;
}

HANDLE BoCA::WMAReader::GetAsyncEventHandle() const
{
	return m_hAsyncEvent;
}

Void BoCA::WMAReader::SetReaderAdvanced(IWMReaderAdvanced *pReaderAdvanced)
{
	m_pReaderAdvanced = pReaderAdvanced;
}

Void BoCA::WMAReader::SetAudioOutputNum(DWORD dwAudioOutputNum)
{
	m_dwAudioOutputNum = dwAudioOutputNum;
}

Void BoCA::WMAReader::SetSamplesBuffer(Buffer<UnsignedByte> *buffer, Mutex *mutex)
{
	samplesBuffer = buffer;
	samplesBufferMutex = mutex;

	/* Allocate 128kB for the sample buffer.
	 */
	samplesBuffer->Resize(131072);
	samplesBuffer->Resize(0);
}
