 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_CONFIG
#define H_BOCA_CONFIG

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Config
	{
		friend class ApplicationConfig;

		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Config			*instance;
			static Array<Config *, Void *>	 copies;

							 Config();
							 Config(const Config &);
			virtual				~Config();

			Array<String>			 persistentIntIDs;
			Array<Int *, Void *>		 persistentIntValues;

			Bool				 saveSettingsOnExit;

			Int				 FindPersistentIntValueIndex(const String &, const String &) const;
		protected:
			Configuration			*config;
		public:
			String				 configDir;
			String				 cacheDir;

			Int				 SetIntValue(const String &, const String &, Int);
			Int				 SetStringValue(const String &, const String &, const String &);

			Int				 GetIntValue(const String &, const String &, Int = 0) const;
			String				 GetStringValue(const String &, const String &, const String & = NIL) const;

			Int				&GetPersistentIntValue(const String &, const String &, Int = 0);

			Bool				 LoadSettings();
			Bool				 SaveSettings();

			Int				 AddConfiguration(const String &);
			Int				 RemoveConfiguration(const String &);

			Int				 GetNOfConfigurations() const;
			String				 GetNthConfigurationName(Int) const;

			Int				 SetActiveConfiguration(const String &);

			String				 GetConfigurationName() const;
			Int				 SetConfigurationName(const String &);

			Void				 SetSaveSettingsOnExit(Bool);

			/* Returns a new or existing instance of Config
			 */
			static Config			*Get();

			/* Returns a static copy of a Config instance
			 */
			static Config			*Copy(const Config * = NIL);

			/* Destroys an existing instance of Config
			 */
			static Void			 Free(Config * = NIL);
	};
};

#endif
