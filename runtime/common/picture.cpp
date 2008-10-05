 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/picture.h>

BoCA::Picture::Picture(int nil)
{
	type = 0;
}

BoCA::Picture::Picture(const Picture &oPicture)
{
	*this = oPicture;
}

BoCA::Picture::~Picture()
{
}

BoCA::Picture &BoCA::Picture::operator =(const Picture &oPicture)
{
	type = oPicture.type;
	mime = oPicture.mime;
	description = oPicture.description;

	data.Resize(oPicture.data.Size());

	memcpy(data, oPicture.data, data.Size());

	return *this;
}
