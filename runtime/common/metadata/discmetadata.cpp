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

#include <boca/common/metadata/discmetadata.h>

BoCA::DiscMetadata::DiscMetadata()
{
	revision = -1;
}

BoCA::DiscMetadata::DiscMetadata(const DiscMetadata &oRecord)
{
	*this = oRecord;
}

BoCA::DiscMetadata::~DiscMetadata()
{
}

BoCA::DiscMetadata &BoCA::DiscMetadata::operator =(const DiscMetadata &oRecord)
{
	if (&oRecord == this) return *this;

	provider = oRecord.provider;
	recordID = oRecord.recordID;
	revision = oRecord.revision;

	disc	 = oRecord.disc;

	tracks.RemoveAll();
	extensions.RemoveAll();

	foreach (const Info &track, oRecord.tracks) tracks.Add(track);
	foreach (const String &extension, oRecord.extensions) extensions.Add(extension);

	return *this;
}
