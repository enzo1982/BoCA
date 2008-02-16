 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/encodercomponent.h>

BoCA::AS::EncoderComponent::EncoderComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::EncoderComponent::~EncoderComponent()
{
}

Int BoCA::AS::EncoderComponent::GetPackageSize()
{
	return specs->func_GetPackageSize(component);
}

Int BoCA::AS::EncoderComponent::SetDriver(IO::Driver *driver)
{
	return specs->func_SetDriver(component, driver);
}

Bool BoCA::AS::EncoderComponent::SetAudioTrackInfo(const Track &track)
{
	return specs->func_SetAudioTrackInfo(component, &track);
}

Bool BoCA::AS::EncoderComponent::Activate()
{
	SetDriver(driver);

	if (specs->func_Activate(component))
	{
		packageSize = GetPackageSize();

		return True;
	}

	return False;
}

Bool BoCA::AS::EncoderComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Int BoCA::AS::EncoderComponent::WriteData(Buffer<UnsignedByte> &buffer, Int size)
{
	return specs->func_WriteData(component, &buffer, size);
}
