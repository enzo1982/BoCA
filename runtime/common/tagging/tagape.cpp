 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagape.h>
#include <boca/common/config.h>

using namespace smooth::IO;

BoCA::TagAPE::TagAPE()
{
}

BoCA::TagAPE::~TagAPE()
{
}

Int BoCA::TagAPE::Render(const Track &track, Buffer<UnsignedByte> &buffer)
{
	Config	*currentConfig = Config::Get();
	char	*prevOutFormat = String::SetOutputFormat("UTF-8");

	buffer.Resize(32);

	Int	 numItems = 0;

	if	(track.artist != NIL) { RenderAPEItem("Artist", track.artist, buffer);		      numItems++; }
	if	(track.title  != NIL) { RenderAPEItem("Title", track.title, buffer);		      numItems++; }
	if	(track.album  != NIL) { RenderAPEItem("Album", track.album, buffer);		      numItems++; }
	if	(track.track   >   0) { RenderAPEItem("Track", String::FromInt(track.track), buffer); numItems++; }
	if	(track.year    >   0) { RenderAPEItem("Year", String::FromInt(track.year), buffer);   numItems++; }
	if	(track.genre  != NIL) { RenderAPEItem("Genre", track.genre, buffer);		      numItems++; }
	if	(track.label  != NIL) { RenderAPEItem("Publisher", track.label, buffer);	      numItems++; }
	if	(track.isrc   != NIL) { RenderAPEItem("ISRC", track.isrc, buffer);		      numItems++; }

	if	(track.comment != NIL && !currentConfig->replace_comments) { RenderAPEItem("Comment", track.comment, buffer);		       numItems++; }
	else if (currentConfig->default_comment != NIL && numItems > 0)	   { RenderAPEItem("Comment", currentConfig->default_comment, buffer); numItems++; }

	if (currentConfig->GetIntValue("Settings", "CopyPictureTags", 1) && currentConfig->GetIntValue("Settings", "WriteAPEv2CoverArt", 0))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			String			 itemName = "Cover Art";
			Buffer<UnsignedByte>	 picBuffer(picInfo.data.Size() + 19);

			if	(picInfo.type == 3) itemName.Append(" (front)");
			else if	(picInfo.type == 4) itemName.Append(" (back)");
			else			    itemName.Append(" (other)");

			strncpy((char *) (unsigned char *) picBuffer, "c:\\music\\cover.jpg", 18);
			memcpy(picBuffer + 19, picInfo.data, picInfo.data.Size());

			RenderAPEBinaryItem(itemName, picBuffer, buffer);

			numItems++;
		}
	}

	if (numItems > 0)
	{
		Int	 tagSize = buffer.Size();

		RenderAPEHeader(tagSize, numItems, buffer);
		RenderAPEFooter(tagSize, numItems, buffer);
	}
	else
	{
		buffer.Resize(0);
	}

	String::SetOutputFormat(prevOutFormat);

	return buffer.Size();
}

Int BoCA::TagAPE::RenderAPEHeader(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 32);

	out.OutputString("APETAGEX");
	out.OutputNumber(2000, 4);
	out.OutputNumber(tagSize, 4);
	out.OutputNumber(numItems, 4);
	out.OutputNumber(0xA0000000, 4);
	out.OutputNumber(0, 4);
	out.OutputNumber(0, 4);

	return Success();
}

Int BoCA::TagAPE::RenderAPEFooter(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
{
	buffer.Resize(buffer.Size() + 32);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - 32, 32);

	out.OutputString("APETAGEX");
	out.OutputNumber(2000, 4);
	out.OutputNumber(tagSize, 4);
	out.OutputNumber(numItems, 4);
	out.OutputNumber(0x80000000, 4);
	out.OutputNumber(0, 4);
	out.OutputNumber(0, 4);

	return Success();
}

Int BoCA::TagAPE::RenderAPEItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value) + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(strlen(value), 4);
	out.OutputNumber(0, 4);
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputString(value);

	return Success();
}

Int BoCA::TagAPE::RenderAPEBinaryItem(const String &id, const Buffer<UnsignedByte> &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + value.Size() + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(value.Size(), 4);
	out.OutputNumber(0x01 << 1, 4); // set binary flag
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputData(value, value.Size());

	return Success();
}

Int BoCA::TagAPE::Parse(const Buffer<UnsignedByte> &buffer, Track *track)
{
	Int	 numItems = 0;
	Int	 offset = 32;

	if (!ParseAPEHeader(buffer, NIL, &numItems))
	{
		offset = 0;

		if (!ParseAPEFooter(buffer, NIL, &numItems)) return Error();
	}

	char		*prevInFormat = String::SetInputFormat("UTF-8");

	for (Int i = 0; i < numItems; i++)
	{
		String			 id;
		String			 value;
		Buffer<UnsignedByte>	 item;

		ParseAPEItem(buffer, offset, &id, &value);

		if (id == "!Binary") ParseAPEBinaryItem(buffer, offset, &id, item);

		if	(id == "Artist")    track->artist  = value;
		else if (id == "Title")	    track->title   = value;
		else if (id == "Album")	    track->album   = value;
		else if (id == "Track")	    track->track   = value.ToInt();
		else if (id == "Year")	    track->year	   = value.ToInt();
		else if (id == "Genre")	    track->genre   = value;
		else if (id == "Comment")   track->comment = value;
		else if (id == "Publisher") track->label   = value;
		else if (id == "ISRC")	    track->isrc	   = value;
		else if (id.StartsWith("Cover Art"))
		{
			Picture	 picture;

			/* Read and ignore file name. Then copy
			 * picture data to Picture object.
			 */
			for (Int i = 0; i < item.Size(); i++)
			{
				if (item[i] == 0)
				{
					picture.data.Resize(item.Size() - i - 1);

					memcpy(picture.data, item + i + 1, picture.data.Size());

					break;
				}
			}

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			if	(id.EndsWith("(front)")) picture.type = 3;
			else if (id.EndsWith("(back)"))	 picture.type = 4;
			else				 picture.type = 0;

			track->pictures.Add(picture);
		}
	}

	String::SetInputFormat(prevInFormat);

	return Success();
}

Int BoCA::TagAPE::Parse(const String &fileName, Track *track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READONLY);
	Buffer<UnsignedByte>	 buffer(32);

	in.InputData(buffer, 32);

	Int	 tagSize = 0;

	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.InputData(buffer, tagSize);

		return Parse(buffer, track);
	}

	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

	if (ParseAPEFooter(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.Seek(in.Size() - tagSize);
		in.InputData(buffer, tagSize);

		return Parse(buffer, track);
	}

	return Error();
}

Bool BoCA::TagAPE::ParseAPEHeader(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
{
	InStream	 in(STREAM_BUFFER, buffer, 32);

	if (in.InputString(8) != "APETAGEX")	return False;
	if (in.InputNumber(4) >= 3000)		return False;

	if (tagSize  != NIL) *tagSize  = in.InputNumber(4);
	else		     in.RelSeek(4);

	if (numItems != NIL) *numItems = in.InputNumber(4);
	else		     in.RelSeek(4);

	return True;
}

Bool BoCA::TagAPE::ParseAPEFooter(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
{
	InStream	 in(STREAM_BUFFER, buffer + buffer.Size() - 32, 32);

	if (in.InputString(8) != "APETAGEX")	return False;
	if (in.InputNumber(4) >= 3000)		return False;

	if (tagSize  != NIL) *tagSize  = in.InputNumber(4);
	else		     in.RelSeek(4);

	if (numItems != NIL) *numItems = in.InputNumber(4);
	else		     in.RelSeek(4);

	return True;
}

Bool BoCA::TagAPE::ParseAPEItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, String *value)
{
	InStream	 in(STREAM_BUFFER, buffer + offset, buffer.Size() - offset - 32);

	Int	 valueBytes = in.InputNumber(4);
	Int	 flags = in.InputNumber(4);

	/* Check if this is a binary tag item.
	 */
	if (((flags >> 1) & 3) == 1)
	{
		*id = "!Binary";

		return True;
	}

	*id = NIL;

	Byte	 lastChar = 0;

	do
	{
		lastChar = in.InputNumber(1);

		(*id)[id->Length()] = lastChar;
	}
	while (lastChar != 0);

	*value = in.InputString(valueBytes);

	offset += in.GetPos();

	return True;
}

Bool BoCA::TagAPE::ParseAPEBinaryItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, Buffer<UnsignedByte> &value)
{
	InStream	 in(STREAM_BUFFER, buffer + offset, buffer.Size() - offset - 32);

	Int	 valueBytes = in.InputNumber(4);

	in.InputNumber(4);

	*id = NIL;

	Byte	 lastChar = 0;

	do
	{
		lastChar = in.InputNumber(1);

		(*id)[id->Length()] = lastChar;
	}
	while (lastChar != 0);

	value.Resize(valueBytes);

	in.InputData(value, valueBytes);

	offset += in.GetPos();

	return True;
}
