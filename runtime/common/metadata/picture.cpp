 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/picture.h>

using namespace smooth::IO;

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
	if (&oPicture == this) return *this;

	type		= oPicture.type;
	mime		= oPicture.mime;
	description	= oPicture.description;
	data		= oPicture.data;

	return *this;
}

Bool BoCA::Picture::operator ==(const Picture &oPicture) const
{
	return (type	    == oPicture.type	    &&
		mime	    == oPicture.mime	    &&
		description == oPicture.description &&
		data	    == oPicture.data);
}

Bool BoCA::Picture::operator !=(const Picture &oPicture) const
{
	return (type	    != oPicture.type	    ||
		mime	    != oPicture.mime	    ||
		description != oPicture.description ||
		data	    != oPicture.data);
}

Int BoCA::Picture::LoadFromFile(const String &fileName)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	type = 0x03; // Cover (front)
	mime = fileName.EndsWith(".png") ? "image/png" : "image/jpeg";

	Buffer<UnsignedByte>	 buffer(in.Size());

	in.InputData(buffer, in.Size());

	data = buffer;

	return Success();
}

Int BoCA::Picture::SaveToFile(const String &fileName) const
{
	OutStream	 out(STREAM_FILE, String(fileName).Append(mime == "image/png" ? ".png" : ".jpg"), OS_REPLACE);

	out.OutputData(data, data.Size());

	return Success();
}

Bitmap BoCA::Picture::GetBitmap() const
{
	Int	 format = -1;

	if	(mime == "image/jpeg" || mime == "image/jpg") format = IMAGE_FORMAT_JPEG;
	else if	(mime == "image/png")			      format = IMAGE_FORMAT_PNG;

	return ImageLoader::Load(data, format);
}
