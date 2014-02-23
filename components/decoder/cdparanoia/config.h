 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
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
			CheckBox	*check_offset;
			EditBox		*edit_offset;
			Text		*text_offset_samples;

			GroupBox	*group_ripping;
			CheckBox	*check_paranoia;
			ComboBox	*combo_paranoia_mode;
			CheckBox	*check_jitter;
			CheckBox	*check_swapchannels;

			GroupBox	*group_automatization;
			CheckBox	*check_autoRead;
			CheckBox	*check_autoRip;

			Bool		 useoffset;
			Bool		 setspeed;
			Bool		 cdparanoia;
			Bool		 jitter;
			Bool		 swapchannels;
			Bool		 autoRead;
			Bool		 autoRip;

			Array<Bool>	 driveOffsetUsed;
			Array<Int>	 driveOffsets;
			Array<Int>	 driveSpeeds;
		slots:
			Void		 SelectDrive();

			Void		 ToggleUseOffset();
			Void		 ChangeOffset();

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
