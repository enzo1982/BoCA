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

#include <boca/component/decodercomponent.h>

BoCA::CS::DecoderComponent::DecoderComponent()
{
	inBytes = 0;
}

BoCA::CS::DecoderComponent::~DecoderComponent()
{
}

Bool BoCA::CS::DecoderComponent::SetAudioTrackInfo(const Track &nTrack)
{
	track = nTrack;

	return True;
}

Bool BoCA::CS::DecoderComponent::Activate()
{
	return True;
}

Bool BoCA::CS::DecoderComponent::Deactivate()
{
	return True;
}

Bool BoCA::CS::DecoderComponent::Seek(Int64 samplePosition)
{
	return False;
}
