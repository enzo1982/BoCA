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

#include <boca/application/taggercomponent.h>

BoCA::AS::TaggerComponent::TaggerComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::TaggerComponent::~TaggerComponent()
{
}

Void BoCA::AS::TaggerComponent::SetVendorString(const String &vendor)
{
	specs->func_SetVendorString(component, vendor);
}

Error BoCA::AS::TaggerComponent::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	return specs->func_ParseBuffer(component, &buffer, &track);
}

Error BoCA::AS::TaggerComponent::ParseStreamInfo(const String &file, Track &track)
{
	return specs->func_ParseStreamInfo(component, file, &track);
}

Error BoCA::AS::TaggerComponent::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	return specs->func_RenderBuffer(component, &buffer, &track);
}

Error BoCA::AS::TaggerComponent::RenderStreamInfo(const String &file, const Track &track)
{
	return specs->func_RenderStreamInfo(component, file, &track);
}

Error BoCA::AS::TaggerComponent::UpdateStreamInfo(const String &file, const Track &track)
{
	return specs->func_UpdateStreamInfo(component, file, &track);
}
