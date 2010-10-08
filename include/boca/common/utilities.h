 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_UTILITIES
#define H_BOCA_UTILITIES

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Utilities
	{
		public:
			static Void	 WarningMessage(const String &, const String & = NIL, const String & = NIL);
			static Void	 ErrorMessage(const String &, const String & = NIL, const String & = NIL);

			static String	 GetNonUnicodeTempFileName(const String &);

			static String	 ReplaceIncompatibleCharacters(const String &);
	};
};

#endif
