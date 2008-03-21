 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tag.h>

BoCA::Tag::Tag()
{
}

BoCA::Tag::~Tag()
{
}

Int BoCA::Tag::Render(const Track &track, Buffer<UnsignedByte> &buffer)
{
	return 0;
}

Int BoCA::Tag::ParseBuffer(Buffer<UnsignedByte> &buffer, Track *track)
{
	return Error();
}

Int BoCA::Tag::ParseFile(const String &fileName, Track *track)
{
	return Error();
}
