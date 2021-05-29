 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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
	      <name>Microsoft Wave Files</name>				\
	      <extension>wav</extension>				\
	    </format>							\
	    <format>							\
	      <name>Sony Media Wave64 Files</name>			\
	      <extension>w64</extension>				\
	    </format>							\
	    <format>							\
	      <name>RIFF 64 Audio Files</name>				\
	      <extension>rf64</extension>				\
	    </format>							\
	    <tagspec>							\
	      <name>RIFF INFO Tag</name>				\
	      <prependzero allowed=\"true\" default=\"true\"/>		\
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

const String	 BoCA::TaggerRIFF::ConfigID = "Tags";

BoCA::TaggerRIFF::TaggerRIFF()
{
}

BoCA::TaggerRIFF::~TaggerRIFF()
{
}

Error BoCA::TaggerRIFF::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig		 = GetConfiguration();

	String		 encodingID		 = currentConfig->GetStringValue(ConfigID, "RIFFINFOTagEncoding", "ISO-8859-1");

	Bool		 prependZero		 = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroRIFFINFOTag", True);

	Bool		 writeMCDI		 = currentConfig->GetIntValue(ConfigID, "WriteMCDI", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Set output encoding.
	 */
	String::OutputFormat	 outputFormat(encodingID);

	/* Save basic information.
	 */
	const Info	&info = track.GetInfo();

	buffer.Resize(12);

	if	(info.artist != NIL) RenderTagItem("IART", info.artist, buffer);
	if	(info.title  != NIL) RenderTagItem("INAM", info.title, buffer);
	if	(info.album  != NIL) RenderTagItem("IPRD", info.album, buffer);
	if	(info.genre  != NIL) RenderTagItem("IGNR", info.genre, buffer);
	if	(info.label  != NIL) RenderTagItem("IDST", info.label, buffer);

	if	(info.track   >   0) RenderTagItem("ITRK", String(prependZero && info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track)), buffer);
	if	(info.year    >   0) RenderTagItem("ICRD", String::FromInt(info.year).Append("-01-01"), buffer);

	if	(info.comment != NIL && !replaceExistingComments) RenderTagItem("ICMT", info.comment, buffer);
	else if (defaultComment != NIL)				  RenderTagItem("ICMT", defaultComment, buffer);

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_COMPOSER)	     RenderTagItem("IMUS", value, buffer);
		else if	(key == INFO_LYRICIST)	     RenderTagItem("IWRI", value, buffer);
		else if	(key == INFO_PRODUCER)	     RenderTagItem("IPRO", value, buffer);
		else if	(key == INFO_ENGINEER)	     RenderTagItem("IENG", value, buffer);

		else if	(key == INFO_COPYRIGHT)	     RenderTagItem("ICOP", value, buffer);

		else if	(key == INFO_MEDIATYPE)	     RenderTagItem("ISRF", value, buffer);

		else if	(key == INFO_RELEASECOUNTRY) RenderTagItem("ICNT", value, buffer);

		else if (key == INFO_WEB_ARTIST)     RenderTagItem("IURL", value, buffer);
	}

	/* Save CD table of contents.
	 */
	if (writeMCDI)
	{
		if	(info.mcdi.IsValid()) RenderTagItem("ITOC", info.mcdi.GetOffsetString(), buffer);
		else if (info.offsets != NIL) RenderTagItem("ITOC", info.offsets, buffer);
	}

	/* Save encoder version.
	 */
	Application	*app = Application::Get();

	RenderTagItem("ISFT", app->getClientName.Call().Append(" ").Append(app->getClientVersion.Call()), buffer);

	/* Render tag header.
	 */
	RenderTagHeader(buffer);

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
	String		 data     = value.Trim();
	Int		 dataSize = data != NIL ? strlen(data) + 1 : 1;
	Int		 size	  = dataSize + (dataSize & 1) + 8;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputString(id);
	out.OutputNumber(dataSize, 4);
	out.OutputString(data);
	out.OutputNumber(0, 1 + (dataSize & 1));

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
	Info			 info = track.GetInfo();
	String::InputFormat	 inputFormat("ISO-8859-1");

	while (!error)
	{
		if (in.GetPos() >= in.Size()) break;

		String	 id	= in.InputString(4);
		Int	 length	= in.InputNumber(4);

		if (length > 0)
		{
			String	 value = in.InputString(length - 1).Trim();

			if	(id == "IART") info.artist  = value;
			else if (id == "INAM") info.title   = value;
			else if (id == "IPRD") info.album   = value;
			else if (id == "ICRD") info.year    = value.Head(4).ToInt();
			else if (id == "IGNR") info.genre   = value;
			else if (id == "ICMT") info.comment = value;
			else if (id == "IDST") info.label   = value;
			else if (id == "TORG") info.label   = value;

			else if (id == "IPRT") info.track   = value.ToInt();
			else if (id == "ITRK") info.track   = value.ToInt();
			else if (id == "TRCK") info.track   = value.ToInt();

			else if (id == "IMUS") info.SetOtherInfo(INFO_COMPOSER,	      value);
			else if (id == "IWRI") info.SetOtherInfo(INFO_LYRICIST,	      value);
			else if (id == "IPRO") info.SetOtherInfo(INFO_PRODUCER,	      value);
			else if (id == "IENG") info.SetOtherInfo(INFO_ENGINEER,	      value);

			else if (id == "ICOP") info.SetOtherInfo(INFO_COPYRIGHT,      value);

			else if (id == "ISRF") info.SetOtherInfo(INFO_MEDIATYPE,      value);

			else if (id == "ICNT") info.SetOtherInfo(INFO_RELEASECOUNTRY, value);

			else if (id == "IURL") info.SetOtherInfo(INFO_WEB_ARTIST,     value);

			else if (id == "ITOC") info.offsets = value;

			/* Read final null character(s).
			 */
			in.InputNumber(1 + (length & 1));
		}
	}

	track.SetInfo(info);

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
		UnsignedInt32	 rSize = in.InputNumber(4);

		in.RelSeek(4);

		Bool	 error = False;
		String	 chunk;
		Int64	 dSize = -1;

		while (!error && chunk != "LIST")
		{
			if (in.GetPos() >= in.Size()) break;

			/* Read next chunk.
			 */
			chunk = in.InputString(4);

			UnsignedInt64	 cSize = (UnsignedInt32) in.InputNumber(4);

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
				if (rSize == 0xFFFFFFFF || rSize == 0 ||
				    cSize == 0xFFFFFFFF || cSize == 0) cSize = in.Size() - in.GetPos();

				if (dSize >= 0) in.RelSeek(dSize + dSize % 2);
				else		in.RelSeek(cSize + cSize % 2);
			}
			else
			{
				/* Skip chunk.
				 */
				if (!in.RelSeek(cSize + cSize % 2)) error = True;
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
				if (!in.RelSeek(cSize - 24 + (cSize % 8 > 0 ? 8 - (cSize % 8) : 0))) error = True;
			}
		}

		if (!error) return Success();
	}

	return Error();
}
