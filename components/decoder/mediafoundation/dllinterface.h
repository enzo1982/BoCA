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

#include <mfidl.h>
#include <mfreadwrite.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mfplatdll;
extern DynamicLoader	*mfreadwritedll;

Bool			 LoadMFPlatDLL();
Void			 FreeMFPlatDLL();

Bool			 LoadMFReadWriteDLL();
Void			 FreeMFReadWriteDLL();

typedef HRESULT		(WINAPI *MFSTARTUP)				(ULONG, DWORD);
typedef HRESULT		(WINAPI *MFSHUTDOWN)				();
typedef HRESULT		(WINAPI *MFCREATEASYNCRESULT)			(IUnknown *, IMFAsyncCallback *, IUnknown *, IMFAsyncResult **);
typedef HRESULT		(WINAPI *MFINVOKECALLBACK)			(IMFAsyncResult *);
typedef HRESULT		(WINAPI *MFCREATEMEDIATYPE)			(IMFMediaType **);
typedef HRESULT		(WINAPI *MFINITMEDIATYPEFROMWAVEFORMATEX)	(IMFMediaType *, const WAVEFORMATEX *, UINT32);

typedef HRESULT		(WINAPI *MFCREATESOURCEREADERFROMBYTESTREAM)	(IMFByteStream *, IMFAttributes *, IMFSourceReader **);

extern MFSTARTUP				 ex_MFStartup;
extern MFSHUTDOWN				 ex_MFShutdown;
extern MFCREATEASYNCRESULT			 ex_MFCreateAsyncResult;
extern MFINVOKECALLBACK				 ex_MFInvokeCallback;
extern MFCREATEMEDIATYPE			 ex_MFCreateMediaType;
extern MFINITMEDIATYPEFROMWAVEFORMATEX		 ex_MFInitMediaTypeFromWaveFormatEx;

extern MFCREATESOURCEREADERFROMBYTESTREAM	 ex_MFCreateSourceReaderFromByteStream;
