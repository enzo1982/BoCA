 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/verifiercomponent.h>

BoCA::AS::VerifierComponent::VerifierComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::VerifierComponent::~VerifierComponent()
{
}

Bool BoCA::AS::VerifierComponent::CanVerifyTrack(const Track &track)
{
	return specs->func_CanVerifyTrack(component, &track);
}

Bool BoCA::AS::VerifierComponent::SetAudioTrackInfo(const Track &track)
{
	return specs->func_SetAudioTrackInfo(component, &track);
}

Bool BoCA::AS::VerifierComponent::Activate()
{
	return specs->func_Activate(component);
}

Bool BoCA::AS::VerifierComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Int BoCA::AS::VerifierComponent::ProcessData(Buffer<UnsignedByte> &buffer)
{
	return specs->func_ProcessData(component, &buffer);
}

Bool BoCA::AS::VerifierComponent::Verify()
{
	return specs->func_Verify(component);
}
