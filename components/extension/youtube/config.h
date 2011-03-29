 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_CONFIG
#define H_YOUTUBE_CONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureYouTube : public ConfigLayer
	{
		private:
			GroupBox		*group_auto;
			CheckBox		*check_auto_download;

			GroupBox		*group_downloads;
			Text			*text_max_downloads;
			EditBox			*edit_max_downloads;
			Arrows			*arrows_max_downloads;

			GroupBox		*group_files;
			CheckBox		*check_keep;

			EditBox			*edit_dir;
			Button			*button_browse;

			Int			 maxDownloads;

			Bool			 autoDownload;
			Bool			 keepVideoFiles;
		public:
			static const String	&GetDefaultVideoOutputDirectory();

						 ConfigureYouTube();
						~ConfigureYouTube();

			Int			 SaveSettings();
		slots:
			Void			 EditMaxDownloads();
			Void			 ArrowsMaxDownloads();

			Void			 ToggleKeepFiles();

			Void			 SelectOutputDir();
	};
};

#endif
