 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <string.h>

#include "riff.h"

using namespace smooth::IO;

const String &BoCA::TaggerRIFF::GetComponentSpecs()
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
	    <format>							\
	      <name>Sony Wave64 Files</name>				\
	      <extension>w64</extension>				\
	    </format>							\
	    <format>							\
	      <name>RIFF 64 Audio Files</name>				\
	      <extension>rf64</extension>				\
	    </format>							\
	    <tagspec>							\
	      <name>RIFF INFO Tag</name>				\
	      <encodings>						\
		<encoding default=\"true\">ISO-8859-1</encoding>	\
		<encoding>UTF-8</encoding>				\
	      </encodings>						\
	    </tagspec>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

BoCA::TaggerRIFF::TaggerRIFF()
{
}

BoCA::TaggerRIFF::~TaggerRIFF()
{
}

Error BoCA::TaggerRIFF::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config		*currentConfig = Config::Get();
	String		 prevOutFormat = String::SetOutputFormat(currentConfig->GetStringValue("Tags", "RIFFINFOTagEncoding", "ISO-8859-1"));

	const Info	&info = track.GetInfo();

	buffer.Resize(12);

	if	(info.artist != NIL) RenderTagItem("IART", info.artist, buffer);
	if	(info.title  != NIL) RenderTagItem("INAM", info.title, buffer);
	if	(info.album  != NIL) RenderTagItem("IPRD", info.album, buffer);
	if	(info.genre  != NIL) RenderTagItem("IGNR", info.genre, buffer);
	if	(info.track   >   0) RenderTagItem("ITRK", String(info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track)), buffer);
	if	(info.year    >   0) RenderTagItem("ICRD", String::FromInt(info.year).Append("-01-01"), buffer);

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	RenderTagItem("ICMT", info.comment, buffer);
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				RenderTagItem("ICMT", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer);

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":") + 1);
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if (key == String(INFO_WEB_ARTIST).Append(":")) RenderTagItem("IURL", value, buffer);
	}

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if	(info.mcdi.GetData().Size() > 0) RenderTagItem("ITOC", info.mcdi.GetOffsetString(), buffer);
		else if (info.offsets != NIL)		 RenderTagItem("ITOC", info.offsets, buffer);
	}

	RenderTagHeader(buffer);

	String::SetOutputFormat(prevOutFormat);

	return Success();
}

Int BoCA::TaggerRIFF::RenderTagHeader(Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 12);

	out.OutputString("LIST");
	out.OutputNumber(buffer.Size() - 8, 4);
	out.OutputString("INFO");

	return Success();
}

Int BoCA::TaggerRIFF::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 stringSize = strlen(value) + 1;
	Int		 bufferSize = stringSize + (stringSize & 1) + 8;

	buffer.Resize(buffer.Size() + bufferSize);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - bufferSize, bufferSize);

	out.OutputString(id);
	out.OutputNumber(stringSize, 4);
	out.OutputString(value);
	out.OutputNumber(0, 1 + (stringSize & 1));

	return Success();
}

Error BoCA::TaggerRIFF::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
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
	Info	 info = track.GetInfo();
	String	 prevInFormat = String::SetInputFormat("ISO-8859-1");

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
		else if (id == "ICRD") info.year    = value.Head(4).ToInt();
		else if (id == "IGNR") info.genre   = value;
		else if (id == "ICMT") info.comment = value;

		else if (id == "IURL") info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append(value));

		else if (id == "ITOC") info.offsets = value;

		/* Read final null character(s).
		 */
		in.InputNumber(1 + (length & 1));
	}

	track.SetInfo(info);

	String::SetInputFormat(prevInFormat);

	if (error) return Error();
	else	   return Success();
}

Error BoCA::TaggerRIFF::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Read RIFF chunk.
	 */
	String		 riff = in.InputString(4);

	if (riff == "RIFF" || riff == "RF64")
	{
		in.RelSeek(8);

		Bool	 error = False;
		String	 chunk;
		Int64	 dSize = 0;

		while (!error && chunk != "LIST")
		{
			if (in.GetPos() >= in.Size()) break;

			/* Read next chunk.
			 */
			chunk = in.InputString(4);

			UnsignedInt	 cSize = in.InputNumber(4);

			if (chunk == "LIST")
			{
				Buffer<UnsignedByte>	 buffer(cSize + 8);

				in.RelSeek(-8);
				in.InputData(buffer, cSize + 8);

				if (ParseBuffer(buffer, track) != Success()) error = True;
			}
			else if (chunk == "ds64")
			{
				in.RelSeek(8);

				dSize = in.InputNumber(8);

				in.RelSeek(cSize - 16);
			}
			else if (chunk == "data")
			{
				if (cSize != UnsignedInt(-1)) in.RelSeek(cSize + cSize % 2);
				else			      in.RelSeek(dSize + dSize % 2);
			}
			else
			{
				/* Skip chunk.
				 */
				in.RelSeek(cSize + cSize % 2);
			}
		}

		if (!error) return Success();
	}
	else if (riff == "riff")
	{
		static unsigned char	 guidRIFF[16] = { 'r', 'i', 'f', 'f', 0x2E, 0x91, 0xCF, 0x11, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00 };
		static unsigned char	 guidLIST[16] = { 'l', 'i', 's', 't', 0x2F, 0x91, 0xCF, 0x11, 0xA5, 0xD6, 0x28, 0xDB, 0x04, 0xC1, 0x00, 0x00 };

		in.RelSeek(-4);

		Bool		 error = False;
		unsigned char	 guid[16];

		in.InputData(guid, 16);

		if (memcmp(guid, guidRIFF, 16) != 0) error = True;

		in.RelSeek(24);

		while (!error && memcmp(guid, guidLIST, 16) != 0)
		{
			if (in.GetPos() >= in.Size()) break;

			/* Read next chunk.
			 */
			in.InputData(guid, 16);

			Int64	 cSize = in.InputNumber(8);

			if (memcmp(guid, guidLIST, 16) == 0)
			{
				Buffer<UnsignedByte>	 buffer(cSize - 16);

				buffer[0] = 'L'; buffer[4] =  cSize	   & 255;
				buffer[1] = 'I'; buffer[5] = (cSize >>  8) & 255;
				buffer[2] = 'S'; buffer[6] = (cSize >> 16) & 255;
				buffer[3] = 'T'; buffer[7] = (cSize >> 24) & 255;

				in.InputData(buffer + 8, cSize - 24);

				if (ParseBuffer(buffer, track) != Success()) error = True;
			}
			else
			{
				/* Skip chunk.
				 */
				in.RelSeek(cSize - 24 + (cSize % 8 > 0 ? 8 - (cSize % 8) : 0));
			}
		}

		if (!error) return Success();
	}

	return Error();
}
