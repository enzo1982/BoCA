 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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
	converter    = NIL;

	calculateMD5 = False;

	errorState   = False;
	errorString  = "Unknown error";
}

BoCA::AS::StreamComponent::~StreamComponent()
{
}

Int BoCA::AS::StreamComponent::SetDriver(IO::Driver *driver)
{
	if (specs->func_SetDriver != NIL) specs->func_SetDriver(component, driver);

	return Success();
}

Bool BoCA::AS::StreamComponent::SetAudioTrackInfo(const Track &track)
{
	this->track = track;

	return specs->func_SetAudioTrackInfo(component, &track);
}

Bool BoCA::AS::StreamComponent::IsThreadSafe() const
{
	if (!specs->threadSafe) return False;
	else			return specs->func_IsThreadSafe(component);
}

Void BoCA::AS::StreamComponent::SetCalculateMD5(Bool nCalculateMD5)
{
	calculateMD5 = nCalculateMD5;
}

String BoCA::AS::StreamComponent::GetMD5Checksum()
{
	if (!calculateMD5 || converter != NIL) return NIL;

	return md5.Finish();
}

Bool BoCA::AS::StreamComponent::Activate()
{
	if (driver != NIL) SetDriver(driver);

	/* Setup format converter and adjust track info.
	 */
	Format	 target = FormatConverter::GetBestTargetFormat(track.GetFormat(), this);

	converter = new FormatConverter(track.GetFormat(), target);

	if (converter->GetErrorState())
	{
		errorState  = True;
		errorString = converter->GetErrorString();

		delete converter;

		return False;
	}

	AdjustTrackSampleCounts(track, target);

	track.SetFormat(target);

	specs->func_SetAudioTrackInfo(component, &track);

	/* Activate component.
	 */
	if (!specs->func_Activate(component))
	{
		delete converter;

		return False;
	}

	return True;
}

Bool BoCA::AS::StreamComponent::Deactivate()
{
	/* Clean up format converter.
	 */
	delete converter;

	converter = NIL;

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
