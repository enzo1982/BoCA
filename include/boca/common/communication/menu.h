 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_MENU
#define H_BOCA_MENU

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Menu
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Menu			*instance;

							 Menu();
							~Menu();
		public:
			/* Returns a new or existing instance of Menu
			 */
			static Menu			*Get();

			/* Destroys an existing instance of Menu
			 */
			static Void			 Free();
		signals:
			Signal1<Void, Menu *>		 doMenubarOverlay;	// called at application startup
			Signal1<Void, Menu *>		 doIconbarOverlay;	// called at application startup

			Signal1<Void, Menu *>		 doContextMenuOverlay;	// called before the context menu is displayed
	};
};

#endif
