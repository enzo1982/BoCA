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

#include <boca/component/streamcomponent.h>

BoCA::CS::StreamComponent::StreamComponent()
{
	driver = NIL;
}

BoCA::CS::StreamComponent::~StreamComponent()
{
}

Bool BoCA::CS::StreamComponent::SetAudioTrackInfo(const Track &track)
{
	this->track = track;

	return True;
}

Bool BoCA::CS::StreamComponent::IsThreadSafe() const
{
	return True;
}

Void BoCA::CS::StreamComponent::SetDriver(IO::Driver *driver)
{
	this->driver = driver;
}

Bool BoCA::CS::StreamComponent::Activate()
{
	return True;
}

Bool BoCA::CS::StreamComponent::Deactivate()
{
	return True;
}
