 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2026 Shen-Ta Hsieh
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/metadatacomponent.h>

BoCA::AS::MetadataComponent::MetadataComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::MetadataComponent::~MetadataComponent()
{
}

Int BoCA::AS::MetadataComponent::GetQueryKeys() const
{
	return specs->func_GetQueryKeys(component);
}

Int BoCA::AS::MetadataComponent::Query(const DiscMetadata &known, Array<MetadataMatch> &matches)
{
	return specs->func_Query(component, &known, &matches);
}

Bool BoCA::AS::MetadataComponent::Read(const String &recordID, DiscMetadata &record)
{
	return specs->func_Read(component, recordID, &record);
}

String BoCA::AS::MetadataComponent::NormalizeRecordID(const String &input)
{
	String	 result;

	specs->func_NormalizeRecordID(component, input, &result);

	return result;
}

Int BoCA::AS::MetadataComponent::GetSubmitData() const
{
	return specs->func_GetSubmitData(component);
}

Int BoCA::AS::MetadataComponent::GetSubmitMethod() const
{
	return specs->func_GetSubmitMethod(component);
}

Bool BoCA::AS::MetadataComponent::Submit(const DiscMetadata &record)
{
	return specs->func_Submit(component, &record);
}

Bool BoCA::AS::MetadataComponent::CanReadPictures() const
{
	return specs->func_CanReadPictures(component);
}

Bool BoCA::AS::MetadataComponent::ReadPictures(const String &recordID, Array<Picture> &pictures)
{
	return specs->func_ReadPictures(component, recordID, &pictures);
}

Void BoCA::AS::MetadataComponent::Cancel()
{
	if (specs->func_Cancel != NIL) specs->func_Cancel(component);
}
