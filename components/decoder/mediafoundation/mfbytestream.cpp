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

#include "mfbytestream.h"

using namespace smooth::IO;

BoCA::MFByteStream::MFByteStream(Driver *driver)
{
	refCount     = 1;

	this->driver = driver;

	bytesRead    = 0;
	bytesWritten = 0;
}

BoCA::MFByteStream::~MFByteStream()
{
}

ULONG STDMETHODCALLTYPE BoCA::MFByteStream::AddRef()
{
	return ++refCount;
}

ULONG STDMETHODCALLTYPE BoCA::MFByteStream::Release()
{
	if (--refCount == 0) delete this;

	return refCount;
}

STDMETHODIMP BoCA::MFByteStream::QueryInterface(REFIID riid, void **ppvObject)
{
	if	(riid == IID_IUnknown)	    *ppvObject = (IUnknown *) this;
	else if	(riid == IID_IMFByteStream) *ppvObject = (IMFByteStream *) this;
	else				    return E_NOINTERFACE;

	AddRef();

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::GetCapabilities(DWORD *pdwCapabilities)
{
	*pdwCapabilities = MFBYTESTREAM_IS_READABLE | MFBYTESTREAM_IS_WRITABLE | MFBYTESTREAM_IS_SEEKABLE;

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::Read(BYTE *pb, ULONG cb, ULONG *pcbRead)
{
	*pcbRead = driver->ReadData(pb, cb);

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::Write(const BYTE *pb, ULONG cb, ULONG *pcbWritten)
{
	*pcbWritten = driver->WriteData(pb, cb);

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::Seek(MFBYTESTREAM_SEEK_ORIGIN SeekOrigin, LONGLONG qwSeekOffset, DWORD dwSeekFlags, QWORD *pqwCurrentPosition)
{
	if (SeekOrigin == msoBegin) *pqwCurrentPosition = driver->Seek(qwSeekOffset);
	else			    *pqwCurrentPosition = driver->Seek(driver->GetPos() + qwSeekOffset);

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::GetCurrentPosition(QWORD *pqwPosition)
{
	*pqwPosition = driver->GetPos();

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::SetCurrentPosition(QWORD qwPosition)
{
	driver->Seek(qwPosition);

	return S_OK;
}
 
STDMETHODIMP BoCA::MFByteStream::GetLength(QWORD *pqwLength)
{
	*pqwLength = driver->GetSize();

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::SetLength(QWORD qwLength)
{
	if (!driver->Truncate(qwLength)) return E_FAIL;

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::IsEndOfStream(BOOL *pfEndOfStream)
{
	Int64	 position = driver->GetPos();
	Int64	 size	  = driver->GetSize();

	*pfEndOfStream = (position == size);

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::Flush()
{
	if (!driver->Flush()) return E_FAIL;

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::Close()
{
	if (!driver->Close()) return E_FAIL;

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::BeginRead(BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	bytesRead = driver->ReadData(pb, cb);

	IMFAsyncResult	*asyncResult = NULL;

	ex_MFCreateAsyncResult(NULL, pCallback, punkState, &asyncResult);
	ex_MFInvokeCallback(asyncResult);

	asyncResult->Release();

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::EndRead(IMFAsyncResult *pResult, ULONG *pcbRead)
{
	*pcbRead = bytesRead;

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::BeginWrite(const BYTE *pb, ULONG cb, IMFAsyncCallback *pCallback, IUnknown *punkState)
{
	bytesWritten = driver->WriteData(pb, cb);

	IMFAsyncResult	*asyncResult = NULL;

	ex_MFCreateAsyncResult(NULL, pCallback, punkState, &asyncResult);
	ex_MFInvokeCallback(asyncResult);

	asyncResult->Release();

	return S_OK;
}

STDMETHODIMP BoCA::MFByteStream::EndWrite(IMFAsyncResult *pResult, ULONG *pcbWritten)
{
	*pcbWritten = bytesWritten;

	return S_OK;
}
