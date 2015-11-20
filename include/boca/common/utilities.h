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

#ifndef H_BOCA_UTILITIES
#define H_BOCA_UTILITIES

#include "metadata/track.h"
#include "metadata/channels.h"

using namespace smooth;
using namespace smooth::System;

namespace BoCA
{
	class BOCA_DLL_EXPORT Utilities
	{
		public:
			static Void		 InfoMessage(const String &, const String & = NIL, const String & = NIL);
			static Void		 WarningMessage(const String &, const String & = NIL, const String & = NIL);
			static Void		 ErrorMessage(const String &, const String & = NIL, const String & = NIL);

			static String		 GetBoCADirectory();

			static DynamicLoader	*LoadCodecDLL(const String &);
			static Bool		 FreeCodecDLL(DynamicLoader *);

			static Bool		 SwitchByteOrder(UnsignedByte *, Int);
			static Bool		 SwitchBufferByteOrder(Buffer<UnsignedByte> &, Int);

			static Bool		 ChangeChannelOrder(Buffer<UnsignedByte> &, const Format &, const Channel::Layout, const Channel::Layout);

			static String		 GetNonUnicodeTempFileName(const String &);

			static String		 ReplaceIncompatibleCharacters(const String &);
			static String		 CreateDirectoryForFile(const String &);

			static String		 GetRelativeFileName(const String &, const String &);

			static String		 GetCDTrackFileName(const Track &);
	};
};

#endif
