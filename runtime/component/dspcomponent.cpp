 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/component/dspcomponent.h>

BoCA::CS::DSPComponent::DSPComponent()
{
}

BoCA::CS::DSPComponent::~DSPComponent()
{
}

Int BoCA::CS::DSPComponent::GetPackageSize()
{
	return 0;
}

Bool BoCA::CS::DSPComponent::SetAudioTrackInfo(const Track &nTrack)
{
	track = nTrack;

	return True;
}

Void BoCA::CS::DSPComponent::GetFormatInfo(Format &oFormat)
{
	oFormat = format;
}

Int BoCA::CS::DSPComponent::Flush(Buffer<UnsignedByte> &buffer)
{
	return 0;
}
