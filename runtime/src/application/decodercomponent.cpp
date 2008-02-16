 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/decodercomponent.h>

BoCA::AS::DecoderComponent::DecoderComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::DecoderComponent::~DecoderComponent()
{
}

Bool BoCA::AS::DecoderComponent::CanOpenStream(const String &file)
{
	return specs->func_CanOpenStream(component, file);
}

Error BoCA::AS::DecoderComponent::GetStreamInfo(const String &file, Track &track)
{
	return specs->func_GetStreamInfo(component, file, &track);
}

Void BoCA::AS::DecoderComponent::SetInputFormat(const Track &track)
{
	specs->func_SetInputFormat(component, &track);
}

Int BoCA::AS::DecoderComponent::GetPackageSize()
{
	return specs->func_GetPackageSize(component);
}

Int BoCA::AS::DecoderComponent::SetDriver(IO::Driver *driver)
{
	return specs->func_SetDriver(component, driver);
}

Bool BoCA::AS::DecoderComponent::Activate()
{
	return specs->func_Activate(component);
}

Bool BoCA::AS::DecoderComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Int BoCA::AS::DecoderComponent::ReadData(Buffer<UnsignedByte> &buffer, Int size)
{
	return specs->func_ReadData(component, &buffer, size);
}

Int64 BoCA::AS::DecoderComponent::GetInBytes()
{
	return specs->func_GetInBytes(component);
}
