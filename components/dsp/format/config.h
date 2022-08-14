 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_FORMATCONFIG
#define H_FORMATCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureFormat : public ConfigLayer
	{
		private:
			GroupBox		*group_format;

			Text			*text_numbers;
			ComboBox		*combo_numbers;

			Text			*text_resolution;
			ComboBox		*combo_resolution;
			Text			*text_bit;

			CheckBox		*check_unsigned;

			GroupBox		*group_dither;

			CheckBox		*check_dither;

			Text			*text_dither_type;
			ComboBox		*combo_dither_type;

			Bool			 unsignedSamples;
			Bool			 applyDither;
		public:
			static const String	 ConfigID;

						 ConfigureFormat();
						~ConfigureFormat();

			Int			 SaveSettings();
		slots:
			Void			 OnChangeNumberFormat();
			Void			 OnChangeResolution();

			Void			 OnToggleDither();
	};
};

#endif
