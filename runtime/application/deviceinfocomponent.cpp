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

#include <boca/application/deviceinfocomponent.h>

BoCA::AS::DeviceInfoComponent::DeviceInfoComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::DeviceInfoComponent::~DeviceInfoComponent()
{
}

Int BoCA::AS::DeviceInfoComponent::GetNumberOfDevices()
{
	return specs->func_GetNumberOfDevices(component);
}

const BoCA::Device &BoCA::AS::DeviceInfoComponent::GetNthDeviceInfo(Int n)
{
	return *((const Device *) specs->func_GetNthDeviceInfo(component, n));
}

Bool BoCA::AS::DeviceInfoComponent::OpenNthDeviceTray(Int n)
{
	return specs->func_OpenNthDeviceTray(component, n);
}

Bool BoCA::AS::DeviceInfoComponent::CloseNthDeviceTray(Int n)
{
	return specs->func_CloseNthDeviceTray(component, n);
}

const Array<String> &BoCA::AS::DeviceInfoComponent::GetNthDeviceTrackList(Int n)
{
	return *((const Array<String> *) specs->func_GetNthDeviceTrackList(component, n));
}

const BoCA::MCDI &BoCA::AS::DeviceInfoComponent::GetNthDeviceMCDI(Int n)
{
	return *((const MCDI *) specs->func_GetNthDeviceMCDI(component, n));
}
