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

#ifndef H_BOCA_AS_DEVICEINFOCOMPONENT
#define H_BOCA_AS_DEVICEINFOCOMPONENT

#include "component.h"
#include "../common/metadata/device.h"
#include "../common/metadata/mcdi.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT DeviceInfoComponent : public Component
		{
			public:
								 DeviceInfoComponent(ComponentSpecs *);
				virtual				~DeviceInfoComponent();

				virtual Int			 GetNumberOfDevices();
				virtual const Device		&GetNthDeviceInfo(Int);

				virtual Bool			 OpenNthDeviceTray(Int);
				virtual Bool			 CloseNthDeviceTray(Int);

				virtual const Array<String>	&GetNthDeviceTrackList(Int);
				virtual const MCDI		&GetNthDeviceMCDI(Int);
		};
	};
};

#endif
