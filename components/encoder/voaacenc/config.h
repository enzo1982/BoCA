 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_VOAACCONFIG
#define H_VOAACCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureVOAAC : public ConfigLayer
	{
		private:
			GroupBox	*group_bitrate;
			Text		*text_bitrate;
			Slider		*slider_bitrate;
			EditBox		*edit_bitrate;
			Text		*text_bitrate_kbps;

			GroupBox	*group_id3v2;
			CheckBox	*check_id3v2;
			Text		*text_note;
			Text		*text_id3v2;

			GroupBox	*group_mp4;
			OptionBox	*option_mp4;
			OptionBox	*option_aac;

			GroupBox	*group_extension;
			OptionBox	*option_extension_m4a;
			OptionBox	*option_extension_m4b;
			OptionBox	*option_extension_m4r;
			OptionBox	*option_extension_mp4;

			Int		 bitrate;
			Bool		 allowID3;
			Int		 fileFormat;
			Int		 fileExtension;
		slots:
			Void		 SetBitrate();
			Void		 SetBitrateByEditBox();
			Void		 SetFileFormat();
		public:
					 ConfigureVOAAC();
					~ConfigureVOAAC();

			Int		 SaveSettings();
	};
};

#endif
