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

#include <boca/application/streamcomponent.h>

BoCA::AS::StreamComponent::StreamComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
	errorState  = False;
	errorString = "Unknown error";
}

BoCA::AS::StreamComponent::~StreamComponent()
{
}

Int BoCA::AS::StreamComponent::GetPackageSize() const
{
	return specs->func_GetPackageSize != NIL ? specs->func_GetPackageSize(component) : 0;
}

Int BoCA::AS::StreamComponent::SetDriver(IO::Driver *driver)
{
	return specs->func_SetDriver != NIL ? specs->func_SetDriver(component, driver) : Success();
}

Bool BoCA::AS::StreamComponent::SetAudioTrackInfo(const Track &track)
{
	this->track = track;

	return specs->func_SetAudioTrackInfo(component, &track);
}

Bool BoCA::AS::StreamComponent::Activate()
{
	SetDriver(driver);

	/* Activate component.
	 */
	if (!specs->func_Activate(component)) return False;

	packageSize = GetPackageSize();

	return True;
}

Bool BoCA::AS::StreamComponent::Deactivate()
{
	/* Deactivate component.
	 */
	return specs->func_Deactivate(component);
}

Bool BoCA::AS::StreamComponent::GetErrorState() const
{
	if (errorState) return True;

	return Component::GetErrorState();
}

const String &BoCA::AS::StreamComponent::GetErrorString() const
{
	if (errorState) return errorString;

	return Component::GetErrorString();
}
