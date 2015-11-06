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

#ifndef H_BOCA_I18N
#define H_BOCA_I18N

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT I18n : public smooth::I18n::Translator
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static I18n	*instance;

					 I18n();
					~I18n();
		public:
			String		 AddColon(const String &);
			String		 AddEllipsis(const String &);
			String		 AddBrackets(const String &, const String &);

			/* Returns a new or existing instance of I18n
			 */
			static I18n	*Get();

			/* Destroys an existing instance of I18n
			 */
			static Void	 Free();
	};
};

#endif
