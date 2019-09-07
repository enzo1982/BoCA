 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_RNNOISECONFIG
#define H_RNNOISECONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureRNNoise : public ConfigLayer
	{
		private:
			GroupBox		*group_signal;

			Text			*text_signal;
			ComboBox		*combo_signal;

			Text			*text_noise;
			ComboBox		*combo_noise;
		public:
			static const String	 ConfigID;

						 ConfigureRNNoise();
						~ConfigureRNNoise();

			Int			 SaveSettings();
	};
};

#endif
