 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BONKCONFIG
#define H_BONKCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureBonk : public ConfigLayer
	{
		private:
			GroupBox	*group_quant;
			Slider		*slider_quant;
			Text		*text_quant;

			GroupBox	*group_stereo;
			CheckBox	*check_joint;

			GroupBox	*group_mode;
			CheckBox	*check_lossless;

			GroupBox	*group_downsampling;
			Slider		*slider_downsampling;
			Text		*text_downsampling;

			GroupBox	*group_predictor;
			Slider		*slider_predictor;
			Text		*text_predictor;

			Int		 quant;
			Int		 predictor;
			Int		 downsampling;
			Bool		 jstereo;
			Bool		 lossless;
		slots:
			Void		 SetQuantization();
			Void		 SetPredictorSize();
			Void		 SetDownsamplingRatio();
			Void		 SetEncoderMode();
		public:
					 ConfigureBonk();
					~ConfigureBonk();

			Int		 SaveSettings();
	};
};

#endif
