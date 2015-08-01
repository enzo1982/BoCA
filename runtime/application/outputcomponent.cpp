 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/outputcomponent.h>

BoCA::AS::OutputComponent::OutputComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::OutputComponent::~OutputComponent()
{
}

Int BoCA::AS::OutputComponent::GetPackageSize()
{
	return specs->func_GetPackageSize(component);
}

Bool BoCA::AS::OutputComponent::SetAudioTrackInfo(const Track &track)
{
	return specs->func_SetAudioTrackInfo(component, &track);
}

Bool BoCA::AS::OutputComponent::Activate()
{
	SetDriver(driver);

	if (specs->func_Activate(component))
	{
		packageSize = GetPackageSize();

		return True;
	}

	return False;
}

Bool BoCA::AS::OutputComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Int BoCA::AS::OutputComponent::WriteData(Buffer<UnsignedByte> &buffer, Int size)
{
	return specs->func_WriteData(component, &buffer, size);
}

Bool BoCA::AS::OutputComponent::Finish()
{
	return specs->func_Finish(component);
}

Int BoCA::AS::OutputComponent::CanWrite()
{
	return specs->func_CanWrite(component);
}

Int BoCA::AS::OutputComponent::SetPause(Bool pause)
{
	return specs->func_SetPause(component, pause);
}

Bool BoCA::AS::OutputComponent::IsPlaying()
{
	return specs->func_IsPlaying(component);
}
