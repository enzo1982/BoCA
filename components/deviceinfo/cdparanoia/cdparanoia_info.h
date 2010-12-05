 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(CDParanoiaInfo)

namespace BoCA
{
	class CDParanoiaInfo : public CS::DeviceInfoComponent
	{
		private:
			static Bool		 initialized;

			Void			 CollectDriveInfo();
		public:
			static const String	&GetComponentSpecs();

						 CDParanoiaInfo();
						~CDParanoiaInfo();

			Bool			 OpenNthDeviceTray(Int);
			Bool			 CloseNthDeviceTray(Int);

			const Array<String>	&GetNthDeviceTrackList(Int);
			const MCDI		&GetNthDeviceMCDI(Int);
	};
};

BoCA_DEFINE_DEVICEINFO_COMPONENT(CDParanoiaInfo)

BoCA_END_COMPONENT(CDParanoiaInfo)
