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

#ifndef H_SURROUNDCONFIG
#define H_SURROUNDCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureSurround : public ConfigLayer
	{
		private:
			GroupBox		*group_output;

			Text			*text_channels;
			ComboBox		*combo_channels;

			CheckBox		*check_lfe;

			Bool			 redirectBass;
		public:
			static const String	 ConfigID;

						 ConfigureSurround();
						~ConfigureSurround();

			Int			 SaveSettings();
	};
};

#endif
