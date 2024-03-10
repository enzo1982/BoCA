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

#ifndef H_OPUSCONFIG
#define H_OPUSCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureOpus : public ConfigLayer
	{
		private:
			GroupBox		*group_quality;
			Text			*text_complexity;
			Slider			*slider_complexity;
			Text			*text_complexity_value;

			Int			 complexity;
		slots:
			Void			 SetComplexity();
		public:
			static const String	 ConfigID;

						 ConfigureOpus();
						~ConfigureOpus();

			Int			 SaveSettings();
	};
};

#endif
