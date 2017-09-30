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

#include <boca.h>

BoCA_BEGIN_COMPONENT(DeviceInfoCDParanoia)

namespace BoCA
{
	class DeviceInfoCDParanoia : public CS::DeviceInfoComponent
	{
		private:
			Void				 CollectDriveInfo();
		public:
			static const String		&GetComponentSpecs();

			static const Array<String>	&FindDrives();

							 DeviceInfoCDParanoia();
							~DeviceInfoCDParanoia();

			Bool				 IsNthDeviceTrayOpen(Int);

			Bool				 OpenNthDeviceTray(Int);
			Bool				 CloseNthDeviceTray(Int);

			const Array<String>		&GetNthDeviceTrackList(Int);
			const MCDI			&GetNthDeviceMCDI(Int);
	};
};

BoCA_DEFINE_DEVICEINFO_COMPONENT(DeviceInfoCDParanoia)

BoCA_END_COMPONENT(DeviceInfoCDParanoia)
