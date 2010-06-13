 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/component/deviceinfocomponent.h>

Array<BoCA::Device>	 BoCA::CS::DeviceInfoComponent::devices;

BoCA::CS::DeviceInfoComponent::DeviceInfoComponent()
{
}

BoCA::CS::DeviceInfoComponent::~DeviceInfoComponent()
{
}

Int BoCA::CS::DeviceInfoComponent::GetNumberOfDevices()
{
	/* Device list must be filled by component.
	 */
	return devices.Length();
}

const BoCA::Device &BoCA::CS::DeviceInfoComponent::GetNthDeviceInfo(Int n)
{
	/* Device list must be filled by component.
	 */
	return devices.GetNth(n);
}

Bool BoCA::CS::DeviceInfoComponent::OpenNthDeviceTray(Int n)
{
	return False;
}

Bool BoCA::CS::DeviceInfoComponent::CloseNthDeviceTray(Int n)
{
	return False;
}

const BoCA::MCDI &BoCA::CS::DeviceInfoComponent::GetNthDeviceMCDI(Int n)
{
	static MCDI	 defaultMCDI = MCDI(Buffer<UnsignedByte>());

	/* Return an empty MCDI by default.
	 */
	return defaultMCDI;
}
