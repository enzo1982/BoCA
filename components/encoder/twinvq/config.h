 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_TWINVQCONFIG
#define H_TWINVQCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureTwinVQ : public ConfigLayer
	{
		private:
			GroupBox	*group_bitrate;
			ComboBox	*combo_bitrate;
			Text		*text_bitrate;
			Text		*text_bitrate_kbps;

			GroupBox	*group_precand;
			ComboBox	*combo_precand;
			Text		*text_precand;
		public:
					 ConfigureTwinVQ();
					~ConfigureTwinVQ();

			Int		 SaveSettings();
	};
};

#endif
