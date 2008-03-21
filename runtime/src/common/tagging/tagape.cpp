 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
	Config		*currentConfig = Config::Get();

	buffer.Resize(32);

	Int		 numItems = 0;

	if (track.artist		   != NIL) { RenderAPEItem("Artist", track.artist, buffer);		       numItems++; }
	if (track.title			   != NIL) { RenderAPEItem("Title", track.title, buffer);		       numItems++; }
	if (track.album			   != NIL) { RenderAPEItem("Album", track.album, buffer);		       numItems++; }
	if (track.track			    >   0) { RenderAPEItem("Track", String::FromInt(track.track), buffer);     numItems++; }
	if (track.year			    >   0) { RenderAPEItem("Year", String::FromInt(track.year), buffer);       numItems++; }
	if (track.genre			   != NIL) { RenderAPEItem("Genre", track.genre, buffer);		       numItems++; }
	if (currentConfig->default_comment != NIL) { RenderAPEItem("Comment", currentConfig->default_comment, buffer); numItems++; }

	Int		 tagSize = buffer.Size();

	RenderAPEHeader(tagSize, numItems, buffer);
	RenderAPEFooter(tagSize, numItems, buffer);

	return buffer.Size();
}

Int BoCA::TagAPE::RenderAPEHeader(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 32);

	out.OutputString("APETAGEX");
	out.OutputNumber(2000, 4);
	out.OutputNumber(tagSize, 4);
	out.OutputNumber(numItems, 4);
	out.OutputNumber(0xE0000000, 4);
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
	out.OutputNumber(0xC0000000, 4);
	out.OutputNumber(0, 4);
	out.OutputNumber(0, 4);

	return Success();
}

Int BoCA::TagAPE::RenderAPEItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value.ConvertTo("UTF-8")) + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(strlen(value.ConvertTo("UTF-8")), 4);
	out.OutputNumber(0, 4);
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputString(value.ConvertTo("UTF-8"));

	return Success();
}

Int BoCA::TagAPE::ParseBuffer(Buffer<UnsignedByte> &buffer, Track *track)
{
	Int	 numItems = 0;
	Int	 offset = 32;

	if (!ParseAPEHeader(buffer, NIL, &numItems))
	{
		offset = 0;

		if (!ParseAPEFooter(buffer, NIL, &numItems)) return Error();
	}

	for (Int i = 0; i < numItems; i++)
	{
		String	 id;
		String	 value;

		ParseAPEItem(buffer, offset, &id, &value);

		if	(id == "Artist") track->artist	= value;
		else if (id == "Title")	 track->title	= value;
		else if (id == "Album")	 track->album	= value;
		else if (id == "Track")	 track->track	= value.ToInt();
		else if (id == "Year")	 track->year	= value.ToInt();
		else if (id == "Genre")	 track->genre	= value;
	}

	return Success();
}

Int BoCA::TagAPE::ParseFile(const String &fileName, Track *track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READONLY);
	Buffer<UnsignedByte>	 buffer(32);

	in.InputData(buffer, 32);

	Int	 tagSize = 0;

	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize + 32);

		in.InputData(buffer + 32, tagSize);

		return ParseBuffer(buffer, track);
	}

	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

	if (ParseAPEFooter(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize + 32);

		in.Seek(in.Size() - (tagSize + 32));
		in.InputData(buffer, tagSize + 32);

		return ParseBuffer(buffer, track);
	}

	return Error();
}

Bool BoCA::TagAPE::ParseAPEHeader(Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::TagAPE::ParseAPEFooter(Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::TagAPE::ParseAPEItem(Buffer<UnsignedByte> &buffer, Int &offset, String *id, String *value)
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

	(*value).ImportFrom("UTF-8", in.InputString(valueBytes));

	offset += in.GetPos();

	return True;
}
