 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_SETTINGS
#define H_BOCA_SETTINGS

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Settings
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Settings			*instance;

							 Settings();
							~Settings();
		public:
			/* Returns a new or existing instance of Settings
			 */
			static Settings			*Get();

			/* Destroys an existing instance of Settings
			 */
			static Void			 Free();
		signals:
			Signal0<Void>			 onChangeConfigurationSettings;	// called when configuration settings are changed
			Signal0<Void>			 onChangeLanguageSettings;	// called when language settings are changed
	};
};

#endif
