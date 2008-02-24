 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

Bool BoCA::CS::EncoderComponent::SetAudioTrackInfo(const Track &nFormat)
{
	format = nFormat;

	return True;
}

String BoCA::CS::EncoderComponent::GetOutputFileExtension()
{
	return NIL;
}
