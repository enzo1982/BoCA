 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_CUESHEET_CONFIG
#define H_CUESHEET_CONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureCueSheet : public ConfigLayer
	{
		private:
			GroupBox	*group_alternatives;
			CheckBox	*check_alternatives;

			Bool		 lookForAlternatives;
		public:
					 ConfigureCueSheet();
					~ConfigureCueSheet();

			Int		 SaveSettings();
	};
};

#endif
