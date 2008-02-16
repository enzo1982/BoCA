 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_WINAMPCONFIG_
#define _H_WINAMPCONFIG_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureWinampOut : public ConfigLayer
	{
		private:
			ListBox		*list_output;
			Button		*button_output;
			Button		*button_output_about;
		slots:
			Void		 SelectOutputPlugin();
			Void		 ConfigureOutputPlugin();
			Void		 AboutOutputPlugin();
		public:
					 ConfigureWinampOut();
					~ConfigureWinampOut();

			Int		 SaveSettings();
	};
};

#endif
