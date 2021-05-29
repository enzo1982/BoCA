 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/device.h>

BoCA::Device::Device()
{
	type = DEVICE_NONE;

	canOpenTray = False;
}

BoCA::Device::~Device()
{
}

String BoCA::Device::GetName() const
{
	return String(vendor).Append(" ").Append(model).Append(" ").Append(revision).Trim();
}

String BoCA::Device::GetID() const
{
	String	 name = GetName();

	return Number(Int64(name.ComputeCRC32())).ToHexString(8);
}
