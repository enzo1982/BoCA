 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_BLADECONFIG_
#define _H_BLADECONFIG_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureBladeEnc : public ConfigLayer
	{
		private:
			GroupBox	*group_bit;
			Slider		*slider_bit;
			Text		*text_bit;

			GroupBox	*group_crc;
			CheckBox	*check_crc;

			GroupBox	*group_copyright;
			CheckBox	*check_copyright;

			GroupBox	*group_original;
			CheckBox	*check_original;

			GroupBox	*group_private;
			CheckBox	*check_private;

			GroupBox	*group_dualchannel;
			CheckBox	*check_dualchannel;

			Int		 bitrate;
			Bool		 crc;
			Bool		 copyright;
			Bool		 original;
			Bool		 priv;
			Bool		 dualchannel;
		slots:
			Void		 SetBitrate();
			Int		 GetBitrate();
			Int		 GetSliderValue();
		public:
					 ConfigureBladeEnc();
					~ConfigureBladeEnc();

			Int		 SaveSettings();
	};
};

#endif
