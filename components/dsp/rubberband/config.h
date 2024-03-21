 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
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

			OptionBox		*option_semitones;
			Slider			*slider_semitones;
			Text			*text_semitones_value;

			OptionBox		*option_ratio;
			EditBox			*edit_ratio_num;
			Text			*text_ratio_num_hz;
			Text			*text_ratio_colon;
			EditBox			*edit_ratio_den;
			Text			*text_ratio_den_hz;
			Text			*text_ratio_equals;
			Text			*text_ratio_value;

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

			Int			 pitchOption;
			Int			 pitch;

			Bool			 smoothing;
		slots:
			Void			 OnChangeTempo(Int);

			Void			 OnChangePitchOption();
			Void			 OnEditRatio();
			Void			 OnChangeSemitones(Int);
		public:
			static const String	 ConfigID;

						 ConfigureRubberBand();
						~ConfigureRubberBand();

			Int			 SaveSettings();
	};
};

#endif
