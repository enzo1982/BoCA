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

			Point		 wndPos;
			Size		 wndSize;
			Bool		 maximized;

			String		 language;
			Bool		 languageChanged;
 
			Bool		 showTitleInfo;
			Bool		 showTooltips;

			String		 encoderID;
			String		 enc_outdir;
			String		 enc_filePattern;
			Bool		 enc_onTheFly;
			Bool		 enc_keepWaves;

			Bool		 encodeToSingleFile;
			Bool		 useUnicodeNames;

			Bool		 writeToInputDir;
			Bool		 allowOverwrite;

			Bool		 checkUpdatesAtStartup;

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

			String		 freedb_dir;
			String		 freedb_server;
			Int		 freedb_mode;
			Int		 freedb_cddbp_port;
			Int		 freedb_http_port;
			String		 freedb_query_path;
			String		 freedb_submit_path;
			String		 freedb_email;
			Int		 freedb_proxy_mode;
			String		 freedb_proxy;
			Int		 freedb_proxy_port;
			String		 freedb_proxy_user;
			String		 freedb_proxy_password;
			Bool		 enable_local_cddb;
			Bool		 enable_remote_cddb;
			Bool		 enable_auto_cddb;
			Bool		 enable_overwrite_cdtext;
			Bool		 enable_cddb_cache;
			Bool		 update_joblist;

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
