 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(DeviceInfoCDRip)

namespace BoCA
{
	class DeviceInfoCDRip : public CS::DeviceInfoComponent
	{
		private:
			static Bool		 initialized;

			Void			 CollectDriveInfo();
		public:
			static const String	&GetComponentSpecs();

						 DeviceInfoCDRip();
						~DeviceInfoCDRip();

			Bool			 OpenNthDeviceTray(Int);
			Bool			 CloseNthDeviceTray(Int);

			const Array<String>	&GetNthDeviceTrackList(Int);
			const MCDI		&GetNthDeviceMCDI(Int);
	};
};

BoCA_DEFINE_DEVICEINFO_COMPONENT(DeviceInfoCDRip)

BoCA_END_COMPONENT(DeviceInfoCDRip)
