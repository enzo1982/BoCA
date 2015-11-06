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

#include <boca/component/outputcomponent.h>

BoCA::CS::OutputComponent::OutputComponent()
{
}

BoCA::CS::OutputComponent::~OutputComponent()
{
}

Bool BoCA::CS::OutputComponent::SetAudioTrackInfo(const Track &nTrack)
{
	track = nTrack;

	return True;
}

Bool BoCA::CS::OutputComponent::Finish()
{
	return True;
}

Int BoCA::CS::OutputComponent::CanWrite()
{
	return 4096;
}

Int BoCA::CS::OutputComponent::SetPause(Bool pause)
{
	return Error();
}

Bool BoCA::CS::OutputComponent::IsPlaying()
{
	return False;
}
