 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DEVICE
#define H_BOCA_DEVICE

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	enum DeviceType
	{
		DEVICE_NONE = 0,
		DEVICE_CDROM,

		NUM_DEVICETYPES
	};

	class BOCA_DLL_EXPORT Device
	{
		public:
			/* Device information:
			 */
			DeviceType	 type;

			String		 vendor;
			String		 model;
			String		 revision;

			String		 path;

			Bool		 canOpenTray;

			/* Class constructor / destructor:
			 */
					 Device();
					~Device();
	};
};

#endif
