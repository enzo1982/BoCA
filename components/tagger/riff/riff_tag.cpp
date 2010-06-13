 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "riff_tag.h"

using namespace smooth::IO;

const String &BoCA::RIFFTag::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>RIFF Tagger</name>					\
	    <version>1.0</version>					\
	    <id>riff-tag</id>						\
	    <type>tagger</type>						\
	    <format>							\
	      <name>Windows Wave Files</name>				\
	      <extension>wav</extension>				\
	    </format>							\
	    <tagformat>							\
	      <name>RIFF INFO Tag</name>				\
	      <encodings>						\
		<encoding default=\"true\">ISO-8859-1</encoding>	\
		<encoding>UTF-8</encoding>				\
	      </encodings>						\
	    </tagformat>						\
	  </component>							\
									\
	";

	return componentSpecs;
}

BoCA::RIFFTag::RIFFTag()
{
}

BoCA::RIFFTag::~RIFFTag()
{
}

Error BoCA::RIFFTag::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config		*currentConfig = Config::Get();
	char		*prevOutFormat = String::SetOutputFormat("UTF-8");

	const Info	&info = track.GetInfo();

	buffer.Resize(12);

	Int	 numItems = 0;

	if	(info.artist != NIL) RenderTagItem("IART", info.artist, buffer);
	if	(info.title  != NIL) RenderTagItem("INAM", info.title, buffer);
	if	(info.album  != NIL) RenderTagItem("IPRD", info.album, buffer);
	if	(info.track   >   0) RenderTagItem("ITRK", String::FromInt(info.track), buffer);
	if	(info.year    >   0) RenderTagItem("ICRD", String::FromInt(info.year), buffer);
	if	(info.genre  != NIL) RenderTagItem("IGNR", info.genre, buffer);

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	RenderTagItem("ICMT", info.comment, buffer);
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		RenderTagItem("ICMT", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer);

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if (value.StartsWith(String(INFO_WEB_ARTIST).Append(":"))) RenderTagItem("IURL", value.Tail(value.Length() - value.Find(":") - 1), buffer);
	}

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if (info.offsets != NIL)
		{
			RenderTagItem("ITOC", info.offsets, buffer);
		}
	}

	RenderTagHeader(buffer);

	String::SetOutputFormat(prevOutFormat);

	return Success();
}

Int BoCA::RIFFTag::RenderTagHeader(Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 12);

	out.OutputString("LIST");
	out.OutputNumber(buffer.Size() - 8, 4);
	out.OutputString("INFO");

	return Success();
}

Int BoCA::RIFFTag::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = strlen(value) + (strlen(value) & 1) + 8;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputString(id);
	out.OutputNumber(size - 8, 4);
	out.OutputString(value);
	out.OutputNumber(0, 1 + (strlen(value) & 1));

	return Success();
}

Error BoCA::RIFFTag::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());
	Bool		 error = False;

	/* Read LIST chunk.
	 */
	if (in.InputString(4) != "LIST") error = True;

	/* Skip header.
	 */
	in.RelSeek(8);

	/* Parse individual comment items.
	 */
	Info	&info = track.GetInfo();
	char	*prevInFormat = String::SetInputFormat("UTF-8");

	while (!error)
	{
		if (in.GetPos() >= in.Size()) break;

		String	 id	= in.InputString(4);
		Int	 length	= in.InputNumber(4);
		String	 value  = in.InputString(length - 1);

		if	(id == "IART") info.artist  = value;
		else if (id == "INAM") info.title   = value;
		else if (id == "IPRD") info.album   = value;
		else if (id == "ITRK") info.track   = value.ToInt();
		else if (id == "ICRD") info.year    = value.ToInt();
		else if (id == "IGNR") info.genre   = value;
		else if (id == "ICMT") info.comment = value;

		else if (id == "IURL") info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append(value));

		else if (id == "ITOC") info.offsets = value;

		/* Read final null character(s).
		 */
		in.InputNumber(1 + (length & 1));
	}

	String::SetInputFormat(prevInFormat);

	if (error) return Error();
	else	   return Success();
}

Error BoCA::RIFFTag::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READONLY);
	Bool		 error = False;

	/* Read RIFF chunk.
	 */
	if (in.InputString(4) != "RIFF") error = True;

	in.RelSeek(8);

	String		 chunk;

	while (!error && chunk != "LIST")
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		chunk = in.InputString(4);

		Int	 cSize = in.InputNumber(4);

		if (chunk == "LIST")
		{
			Buffer<UnsignedByte>	 buffer(cSize + 8);

			in.RelSeek(-8);
			in.InputData(buffer, cSize + 8);

			if (ParseBuffer(buffer, track) != Success()) error = True;
		}
		else
		{
			/* Skip chunk.
			 */
			in.RelSeek(cSize);
		}
	}

	if (error) return Error();
	else	   return Success();
}