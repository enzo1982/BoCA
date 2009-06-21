 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

Error BoCA::CS::DecoderComponent::UpdateStreamInfo(const String &file, const Track &track)
{
	return Error();
}
