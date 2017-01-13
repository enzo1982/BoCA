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

#include <boca/component/encodercomponent.h>

BoCA::CS::EncoderComponent::EncoderComponent()
{
}

BoCA::CS::EncoderComponent::~EncoderComponent()
{
}

Bool BoCA::CS::EncoderComponent::SetAudioTrackInfo(const Track &nTrack)
{
	track = nTrack;

	return True;
}

Bool BoCA::CS::EncoderComponent::SetOutputFormat(Int n)
{
	return True;
}

String BoCA::CS::EncoderComponent::GetOutputFileExtension() const
{
	return NIL;
}

Int BoCA::CS::EncoderComponent::GetNumberOfPasses() const
{
	return 1;
}

Bool BoCA::CS::EncoderComponent::IsThreadSafe() const
{
	return True;
}

Bool BoCA::CS::EncoderComponent::IsLossless() const
{
	return False;
}

Bool BoCA::CS::EncoderComponent::NextPass()
{
	return False;
}
