 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_CDPARANOIACONFIG
#define H_CDPARANOIACONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureCDParanoia : public ConfigLayer
	{
		private:
			GroupBox	*group_drive;
			ComboBox	*combo_drive;
			CheckBox	*check_speed;
			ComboBox	*combo_speed;

			GroupBox	*group_ripping;
			CheckBox	*check_paranoia;
			ComboBox	*combo_paranoia_mode;
			CheckBox	*check_jitter;
			CheckBox	*check_swapchannels;

			GroupBox	*group_automatization;
			CheckBox	*check_autoRead;
			CheckBox	*check_autoRip;

			Bool		 setspeed;
			Bool		 cdparanoia;
			Bool		 jitter;
			Bool		 swapchannels;
			Bool		 autoRead;
			Bool		 autoRip;

			Array<Int>	 driveSpeeds;
		slots:
			Void		 SelectDrive();

			Void		 ToggleSetSpeed();
			Void		 SelectSpeed();

			Void		 ToggleParanoia();

			Void		 ToggleAutoRead();
		public:
					 ConfigureCDParanoia();
					~ConfigureCDParanoia();

			Int		 SaveSettings();
	};
};

#endif
