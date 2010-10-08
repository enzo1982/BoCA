 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_CONFIG
#define H_BOCA_CONFIG

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;
using namespace smooth::GUI;

namespace BoCA
{
	class BOCA_DLL_EXPORT Config
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Config	*instance;

					 Config();
					~Config();

			Configuration	*config;

			Array<String>	 persistentIntIDs;
			Array<Int *>	 persistentIntValues;

			Bool		 saveSettingsOnExit;

			Int		 FindPersistentIntValueIndex(const String &, const String &);
		public:
			String		 configDir;

			Bool		 languageChanged;

			Bool		 enable_console;

			Int		 cdrip_numdrives;
			Int		 cdrip_timeout;
			Bool		 cdrip_autoRead_active;

			Int		 SetIntValue(const String &, const String &, Int);
			Int		 SetStringValue(const String &, const String &, const String &);

			Int		 GetIntValue(const String &, const String &, Int = 0);
			String		 GetStringValue(const String &, const String &, const String & = NIL);

			Int		&GetPersistentIntValue(const String &, const String &, Int = 0);

			Bool		 LoadSettings();
			Bool		 SaveSettings();

			Void		 SetSaveSettingsOnExit(Bool);

			/* Returns a new or existing instance of Config
			 */
			static Config	*Get();

			/* Destroys an existing instance of Config
			 */
			static Void	 Free();
	};
};

#endif
