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

#include <wmsdk/wmsdk.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*wmvcoredll;

Bool			 LoadWMVCoreDLL();
Void			 FreeWMVCoreDLL();

typedef HRESULT		(STDMETHODCALLTYPE *WMCREATEEDITOR)	(IWMMetadataEditor **);

extern WMCREATEEDITOR	 ex_WMCreateEditor;
