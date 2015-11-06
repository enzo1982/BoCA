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

#ifndef H_RESAMPLECONFIG
#define H_RESAMPLECONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureResample : public ConfigLayer
	{
		private:
			GroupBox	*group_converter;
			Text		*text_converter;
			ComboBox	*combo_converter;
			Text		*text_description;

			GroupBox	*group_samplerate;
			Text		*text_samplerate;
			EditBox		*edit_samplerate;
			List		*list_samplerate;
		slots:
			Void		 SetConverter();
		public:
					 ConfigureResample();
					~ConfigureResample();

			Int		 SaveSettings();
	};
};

#endif
