 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_WMA_READER
#define H_BOCA_WMA_READER

#include <boca.h>
#include "dllinterface.h"

using namespace smooth::Threads;

namespace BoCA
{
	class WMAReader : public IWMReaderCallback,
			  public IWMReaderCallbackAdvanced
	{
		private:
			LONG			 m_cRef;

			BOOL			 m_fEOF;
			QWORD			 m_qwTime;
			DWORD			 m_dwAudioOutputNum;

			HANDLE			 m_hAsyncEvent;

			IWMReaderAdvanced	*m_pReaderAdvanced;

			Bool			 active;
			Bool			 error;

			String			 errorString;

			Buffer<UnsignedByte>	*samplesBuffer;
			Mutex			*samplesBufferMutex;
		public:
						 WMAReader();
			virtual			~WMAReader();

			Void			 SetAsyncEvent(HRESULT hrAsync);
		accessors:
			Bool			 IsActive();
			Void			 SetActive(Bool);

			Bool			 IsError();
			const String		&GetErrorString();

			HANDLE			 GetAsyncEventHandle() const;

			Void			 SetReaderAdvanced(IWMReaderAdvanced *);
			Void			 SetAudioOutputNum(DWORD);

			Void			 SetSamplesBuffer(Buffer<UnsignedByte> *, Mutex *);
		public:
			/* IUnknown methods
			 */
			STDMETHOD(QueryInterface)(REFIID, void __RPC_FAR * __RPC_FAR *);
			STDMETHOD_(ULONG, AddRef)();
			STDMETHOD_(ULONG, Release)();

			/* IWMStatusCallback methods
			 */
			STDMETHOD(OnStatus)(WMT_STATUS, HRESULT, WMT_ATTR_DATATYPE, BYTE *, void *);

			/* IWMReaderCallback methods
			 */
			STDMETHOD(OnSample)(DWORD, QWORD, QWORD, DWORD, INSSBuffer *, void *);

			/* IWMReaderCallbackAdvanced methods
			 */
			STDMETHOD(OnTime)(QWORD, void *);

			STDMETHOD(OnStreamSample)(WORD wStreamNum, QWORD cnsSampleTime, QWORD cnsSampleDuration, DWORD dwFlags, INSSBuffer *pSample, void *pvContext)	{ return S_OK; }
			STDMETHOD(OnStreamSelection)(WORD wStreamCount, WORD *pStreamNumbers, WMT_STREAM_SELECTION *pSelections, void *pvContext)			{ return S_OK; }

			STDMETHOD(OnOutputPropsChanged)(DWORD dwOutputNum, WM_MEDIA_TYPE *pMediaType, void *pvContext)							{ return S_OK; }

			STDMETHOD(AllocateForStream)(WORD wStreamNum, DWORD cbBuffer, INSSBuffer **ppBuffer, void *pvContext)						{ return E_NOTIMPL; }
			STDMETHOD(AllocateForOutput)(DWORD dwOutputNum, DWORD cbBuffer, INSSBuffer **ppBuffer, void *pvContext)						{ return E_NOTIMPL; }
	};
};

#endif
