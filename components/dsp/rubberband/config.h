 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_RUBBERBANDCONFIG
#define H_RUBBERBANDCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureRubberBand : public ConfigLayer
	{
		private:
			GroupBox		*group_basic;

			Text			*text_tempo;
			Slider			*slider_tempo;
			Text			*text_tempo_value;

			Text			*text_pitch;
			Slider			*slider_pitch;
			Text			*text_pitch_value;

			GroupBox		*group_tuning;

			Text			*text_detector;
			ComboBox		*combo_detector;

			Text			*text_transients;
			ComboBox		*combo_transients;

			Text			*text_window;
			ComboBox		*combo_window;
			CheckBox		*check_smoothing;

			Text			*text_phase;
			ComboBox		*combo_phase;

			Text			*text_formant;
			ComboBox		*combo_formant;

			Text			*text_pitchmode;
			ComboBox		*combo_pitchmode;

			Text			*text_channels;
			ComboBox		*combo_channels;

			Int			 tempo;
			Int			 pitch;

			Bool			 smoothing;
		slots:
			Void			 OnChangeTempo(Int);
			Void			 OnChangePitch(Int);
		public:
			static const String	 ConfigID;

						 ConfigureRubberBand();
						~ConfigureRubberBand();

			Int			 SaveSettings();
	};
};

#endif
