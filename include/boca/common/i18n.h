 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
			/* Returns a new or existing instance of I18n
			 */
			static I18n	*Get();

			/* Destroys an existing instance of I18n
			 */
			static Void	 Free();
	};
};

#endif
