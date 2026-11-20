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

#include <boca/component/metadatacomponent.h>

BoCA::CS::MetadataComponent::MetadataComponent()
{
	canceled = False;
}

BoCA::CS::MetadataComponent::~MetadataComponent()
{
}

/* Turn an ID or URL that the user entered into the ID that Query
 * reads from INFO_METADATA_RECORDID. Returns NIL if the input is
 * not valid.
 */
String BoCA::CS::MetadataComponent::NormalizeRecordID(const String &)
{
	return NIL;
}

Int BoCA::CS::MetadataComponent::GetSubmitData() const
{
	return METADATA_SUBMIT_DATA_NONE;
}

Int BoCA::CS::MetadataComponent::GetSubmitMethod() const
{
	return METADATA_SUBMIT_BY_PROVIDER;
}

Bool BoCA::CS::MetadataComponent::Submit(const DiscMetadata &)
{
	return False;
}

/* Cover art is reserved for a later change; no provider implements it yet.
 */
Bool BoCA::CS::MetadataComponent::CanReadPictures() const
{
	return False;
}

Bool BoCA::CS::MetadataComponent::ReadPictures(const String &, Array<Picture> &)
{
	return False;
}

/* Ask a running Query, Read or Submit to stop. The application calls
 * this from another thread. A provider stops its network requests
 * when the canceled flag is set. After Cancel, the component is not
 * used again.
 */
Void BoCA::CS::MetadataComponent::Cancel()
{
	canceled = True;
}
