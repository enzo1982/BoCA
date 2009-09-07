 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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

			Bool		 saveSettingsOnExit;

					 Config();
					~Config();

			Configuration	*config;
		public:
			String		 configDir;

			Bool		 firstStart;

			String		 language;
			Bool		 languageChanged;
 
			Bool		 writeToInputDir;
			Bool		 allowOverwrite;

			String		 enc_filePattern;
			Bool		 enc_onTheFly;
			Bool		 enc_keepWaves;
			Bool		 useUnicodeNames;

			Array<String>	 cdrip_drives;
			Int		 cdrip_numdrives;
			Int		 cdrip_activedrive;
			Int		 cdrip_debuglevel;
			Bool		 cdrip_paranoia;
			Bool		 cdrip_jitter;
			Int		 cdrip_paranoia_mode;
			Bool		 cdrip_swapchannels;
			Bool		 cdrip_autoRead_active;
			Bool		 cdrip_detectJitterErrors;
			Bool		 cdrip_detectC2Errors;
			Int		 cdrip_timeout;

			Int		 SetIntValue(const String &, const String &, Int);
			Int		 SetStringValue(const String &, const String &, const String &);

			Int		 GetIntValue(const String &, const String &, Int = 0);
			String		 GetStringValue(const String &, const String &, const String & = NIL);

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
