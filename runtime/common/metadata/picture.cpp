 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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
	InStream		 in(STREAM_FILE, fileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(in.Size());

	in.InputData(buffer, buffer.Size());

	/* Try to guess picture type.
	 */
	String	 lowerCase = fileName.ToLower();

	if	(lowerCase.Contains("cover"))	type = 0x03; // Cover (front)
	else if (lowerCase.Contains("folder"))  type = 0x03; // Cover (front)
	else if (lowerCase.Contains("front"))	type = 0x03; // Cover (front)
	else if (lowerCase.Contains("back"))	type = 0x04; // Cover (back)
	else if (lowerCase.Contains("booklet"))	type = 0x05; // Leaflet page
	else if (lowerCase.Contains("cd"))	type = 0x06; // Media (e.g. lable side of CD)
	else					type = 0x00; // Other

	/* Find MIME type and assign data.
	 */
	if (buffer.Size() >= 16)
	{
		if	(buffer[0] == 0xFF && buffer[1] == 0xD8) mime = "image/jpeg";
		else if (buffer[0] == 0x89 && buffer[1] == 0x50 &&
			 buffer[2] == 0x4E && buffer[3] == 0x47 &&
			 buffer[4] == 0x0D && buffer[5] == 0x0A &&
			 buffer[6] == 0x1A && buffer[7] == 0x0A) mime = "image/png";

		if (buffer[0] != 0 && buffer[1] != 0) data = buffer;
	}

	return Success();
}

Int BoCA::Picture::SaveToFile(const String &fileName) const
{
	if (data.Size() == 0) return Error();

	OutStream	 out(STREAM_FILE, String(fileName).Append(mime == "image/png" ? ".png" : ".jpg"), OS_REPLACE);

	out.OutputData(data, data.Size());

	return Success();
}

Bitmap BoCA::Picture::GetBitmap() const
{
	if (data.Size() == 0) return NIL;

	Int	 format = -1;

	if	(mime == "image/jpeg" || mime == "image/jpg") format = IMAGE_FORMAT_JPEG;
	else if	(mime == "image/png")			      format = IMAGE_FORMAT_PNG;

	return ImageLoader::Load(data, format);
}
