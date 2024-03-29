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
			GroupBox		*group_information;
			CheckBox		*check_read_tags;
			CheckBox		*check_prefer_cue;

			GroupBox		*group_alternatives;
			CheckBox		*check_alternatives;

			GroupBox		*group_errors;
			CheckBox		*check_ignore_errors;

			Bool			 readInfoTags;
			Bool			 preferCueSheets;

			Bool			 lookForAlternatives;

			Bool			 ignoreErrors;
		public:
			static const String	 ConfigID;

						 ConfigureCueSheet();
						~ConfigureCueSheet();

			Int			 SaveSettings();
		slots:
			Void			 ToggleReadTags();
	};
};

#endif
