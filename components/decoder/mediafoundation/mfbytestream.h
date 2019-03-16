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

#ifndef H_BOCA_MFBYTESTREAM
#define H_BOCA_MFBYTESTREAM

#include <smooth.h>

#include "dllinterface.h"

using namespace smooth;

namespace BoCA
{
	class MFByteStream: public IMFByteStream
	{
		private:
			ULONG			 refCount;

			IO::Driver		*driver;

			ULONG			 bytesRead;
			ULONG			 bytesWritten;
		public:
						 MFByteStream(IO::Driver *);
			virtual			~MFByteStream();

			/* IUnknown methods.
			 */
			ULONG STDMETHODCALLTYPE	 AddRef();
			ULONG STDMETHODCALLTYPE	 Release();

			STDMETHOD		 (QueryInterface)(REFIID, void **);
				
			/* IMFByteStream methods.
			 */
			STDMETHOD		 (GetCapabilities)(DWORD *);
			
			STDMETHOD		 (Read)(BYTE *, ULONG, ULONG *);
			STDMETHOD		 (Write)(const BYTE *, ULONG, ULONG *);

			STDMETHOD		 (Seek)(MFBYTESTREAM_SEEK_ORIGIN, LONGLONG, DWORD, QWORD *);

			STDMETHOD		 (GetCurrentPosition)(QWORD *);
			STDMETHOD		 (SetCurrentPosition)(QWORD);

			STDMETHOD		 (GetLength)(QWORD *);
			STDMETHOD		 (SetLength)(QWORD);

			STDMETHOD		 (IsEndOfStream)(BOOL *);

			STDMETHOD		 (Flush)();
			STDMETHOD		 (Close)();

			STDMETHOD		 (BeginRead)(BYTE *, ULONG, IMFAsyncCallback *, IUnknown *);
			STDMETHOD		 (EndRead)(IMFAsyncResult * pResult, ULONG * pcbRead);

			STDMETHOD		 (BeginWrite)(const BYTE *, ULONG, IMFAsyncCallback *, IUnknown *);
			STDMETHOD		 (EndWrite)(IMFAsyncResult *, ULONG *);
	};
};

#endif
