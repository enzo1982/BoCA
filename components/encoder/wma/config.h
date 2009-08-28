 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_WMACONFIG
#define H_WMACONFIG

#include <smooth.h>
#include <boca.h>

#include "dllinterface.h"

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureWMAEnc : public ConfigLayer
	{
		private:
			IWMProfileManager	*profileManager;

			GroupBox		*group_codec;
			OptionBox		*option_uncompressed;
			OptionBox		*option_codec;
			ComboBox		*combo_codec;

			GroupBox		*group_format;
			OptionBox		*option_autoselect;
			OptionBox		*option_format;
			CheckBox		*check_vbr;
			CheckBox		*check_2pass;
			ComboBox		*combo_format;

			GroupBox		*group_settings;
			CheckBox		*check_vbr_setting;
			CheckBox		*check_2pass_setting;
			Text			*text_quality;
			Slider			*slider_quality;
			Text			*text_quality_value;
			Text			*text_bitrate;
			ComboBox		*combo_bitrate;
			Text			*text_bitrate_kbps;

			Bool			 supportCBR1Pass;
			Bool			 supportVBR1Pass;
			Bool			 supportCBR2Pass;
			Bool			 supportVBR2Pass;

			Int			 uncompressed;
			Int			 autoselect;

			Bool			 useVBR;
			Bool			 use2Pass;

			Bool			 useVBRSetting;
			Bool			 use2PassSetting;

			Int			 quality;

			Void			 FillCodecComboBox();
			Void			 FillFormatComboBox();
		public:
						 ConfigureWMAEnc();
						~ConfigureWMAEnc();

			Int			 SaveSettings();
		slots:
			Void			 OnToggleCodec();
			Void			 OnSelectCodec();

			Void			 OnToggleFormat();

			Void			 OnToggleVBR();
			Void			 OnToggle2Pass();

			Void			 OnToggleVBRSetting();
			Void			 OnToggle2PassSetting();

			Void			 OnSetQuality();
	};
};

#endif
