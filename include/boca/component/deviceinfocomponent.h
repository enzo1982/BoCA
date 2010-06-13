 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DEVICEINFOCOMPONENT
#define H_BOCA_DEVICEINFOCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/metadata/device.h"
#include "../common/metadata/mcdi.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT DeviceInfoComponent : public Component
		{
			protected:
				static Array<Device>		 devices;
			public:
								 DeviceInfoComponent();
				virtual				~DeviceInfoComponent();

				Int				 GetNumberOfDevices();
				const Device			&GetNthDeviceInfo(Int);

				virtual Bool			 OpenNthDeviceTray(Int);
				virtual Bool			 CloseNthDeviceTray(Int);

				virtual const Array<String>	&GetNthDeviceTrackList(Int) = 0;
				virtual const MCDI		&GetNthDeviceMCDI(Int);
		};
	};
};

#endif
