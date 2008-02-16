 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_VORBISCONFIG_
#define _H_VORBISCONFIG_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureVorbis : public ConfigLayer
	{
		private:
			GroupBox	*group_mode;
			GroupBox	*group_mode2;
			OptionBox	*option_mode_vbr;
			OptionBox	*option_mode_abr;
			Slider		*slider_quality;
			Text		*text_quality;
			Text		*text_quality_value;
			Slider		*slider_abr;
			Text		*text_abr;
			EditBox		*edit_abr;
			Text		*text_abr_kbps;

			Layer		*layer_vbr;
			Layer		*layer_abr;

			Int		 quality;
			Int		 abr;
			Int		 mode;
		slots:
			Void		 SetMode();
			Void		 SetQuality();
			Void		 SetBitrate();
			Void		 SetBitrateByEditBox();
		public:
					 ConfigureVorbis();
					~ConfigureVorbis();

			Int		 SaveSettings();
	};
};

#endif
