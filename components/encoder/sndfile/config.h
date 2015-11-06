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

#ifndef H_SNDFILECONFIG
#define H_SNDFILECONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureSndFile : public ConfigLayer
	{
		private:
			GroupBox	*group_format;
			Text		*text_format;
			ComboBox	*combo_format;
			Text		*text_subformat;
			ComboBox	*combo_subformat;

			Array<Int>	 formats;
			Array<Int>	 subformats;

			Void		 FillFormats();
		slots:
			Void		 SelectFormat();
		public:
					 ConfigureSndFile();
					~ConfigureSndFile();

			Int		 SaveSettings();
	};
};

#endif
