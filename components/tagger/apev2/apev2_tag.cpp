 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <string.h>

#include "apev2_tag.h"

using namespace smooth::IO;

const String &BoCA::APETag::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>APEv2 Tagger</name>			\
	    <version>1.0</version>			\
	    <id>apev2-tag</id>				\
	    <type>tagger</type>				\
	    <format>					\
	      <name>Monkey's Audio</name>		\
	      <extension>ape</extension>		\
	      <extension>mac</extension>		\
	    </format>					\
	    <tagformat>					\
	      <name>APEv2</name>			\
	      <coverart supported=\"true\"/>		\
	      <encodings>				\
		<encoding>UTF-8</encoding>		\
	      </encodings>				\
	    </tagformat>				\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::APETag::APETag()
{
}

BoCA::APETag::~APETag()
{
}

Error BoCA::APETag::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config		*currentConfig = Config::Get();
	String		 prevOutFormat = String::SetOutputFormat("UTF-8");

	const Info	&info = track.GetInfo();

	buffer.Resize(32);

	Int		 numItems = 0;

	if (info.artist != NIL) { RenderAPEItem("Artist", info.artist, buffer);		     numItems++; }
	if (info.title  != NIL) { RenderAPEItem("Title", info.title, buffer);		     numItems++; }
	if (info.album  != NIL) { RenderAPEItem("Album", info.album, buffer);		     numItems++; }
	if (info.year    >   0) { RenderAPEItem("Year", String::FromInt(info.year), buffer); numItems++; }
	if (info.genre  != NIL) { RenderAPEItem("Genre", info.genre, buffer);		     numItems++; }
	if (info.label  != NIL) { RenderAPEItem("Publisher", info.label, buffer);	     numItems++; }
	if (info.isrc   != NIL) { RenderAPEItem("ISRC", info.isrc, buffer);		     numItems++; }

	if (info.track > 0)
	{
		String	 trackString = String(info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks));

		{ RenderAPEItem("Track", trackString, buffer); numItems++; }
	}

	if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
	{
		String	 discString = String(info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs));

		{ RenderAPEItem("Disc", discString, buffer); numItems++; }
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ RenderAPEItem("Comment", info.comment, buffer);						  numItems++; }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		{ RenderAPEItem("Comment", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer); numItems++; }

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 pair  = info.other.GetNth(i);

		String	 key   = pair.Head(pair.Find(":") + 1);
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == String(INFO_SUBTITLE).Append(":"))  { RenderAPEItem("Subtitle",  value, buffer); numItems++; }

		else if	(key == String(INFO_CONDUCTOR).Append(":")) { RenderAPEItem("Conductor", value, buffer); numItems++; }
		else if	(key == String(INFO_COMPOSER).Append(":"))  { RenderAPEItem("Composer",  value, buffer); numItems++; }
	}

	/* Save Replay Gain info.
	 */
	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ RenderAPEItem("replaygain_track_gain", info.track_gain, buffer); numItems++; }
			{ RenderAPEItem("replaygain_track_peak", info.track_peak, buffer); numItems++; }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ RenderAPEItem("replaygain_album_gain", info.album_gain, buffer); numItems++; }
			{ RenderAPEItem("replaygain_album_peak", info.album_peak, buffer); numItems++; }
		}
	}

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if (info.mcdi.GetData().Size() > 0) { RenderAPEBinaryItem("MCDI", info.mcdi.GetData(), buffer); numItems++; }
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToAPEv2", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			String			 itemName = "Cover Art";
			Buffer<UnsignedByte>	 picBuffer(picInfo.data.Size() + 19);

			if	(picInfo.type == 3) itemName.Append(" (front)");
			else if	(picInfo.type == 4) itemName.Append(" (back)");
			else			    itemName.Append(" (other)");

			strncpy((char *) (unsigned char *) picBuffer, "c:\\music\\cover.jpg", 19);
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

	return Success();
}

Int BoCA::APETag::RenderAPEHeader(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
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

Int BoCA::APETag::RenderAPEFooter(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
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

Int BoCA::APETag::RenderAPEItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
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

Int BoCA::APETag::RenderAPEBinaryItem(const String &id, const Buffer<UnsignedByte> &value, Buffer<UnsignedByte> &buffer)
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

Error BoCA::APETag::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	Config		*currentConfig = Config::Get();

	Int		 numItems = 0;
	Int		 offset = 32;

	if (!ParseAPEHeader(buffer, NIL, &numItems))
	{
		offset = 0;

		if (!ParseAPEFooter(buffer, NIL, &numItems)) return Error();
	}

	String	 prevInFormat = String::SetInputFormat("UTF-8");

	Info	 info = track.GetInfo();

	for (Int i = 0; i < numItems; i++)
	{
		String			 id;
		String			 value;
		Buffer<UnsignedByte>	 item;

		ParseAPEItem(buffer, offset, &id, &value);

		id = id.ToUpper();

		if (id == "!BINARY") ParseAPEBinaryItem(buffer, offset, &id, item);

		if	(id == "ARTIST")    info.artist  = value;
		else if (id == "TITLE")	    info.title   = value;
		else if (id == "ALBUM")	    info.album   = value;
		else if (id == "YEAR")	    info.year	 = value.ToInt();
		else if (id == "GENRE")	    info.genre   = value;
		else if (id == "COMMENT")   info.comment = value;
		else if (id == "PUBLISHER") info.label   = value;
		else if (id == "ISRC")	    info.isrc	 = value;

		else if (id == "SUBTITLE")  info.other.Add(String(INFO_SUBTITLE).Append(":").Append(value));

		else if (id == "CONDUCTOR") info.other.Add(String(INFO_CONDUCTOR).Append(":").Append(value));
		else if (id == "COMPOSER")  info.other.Add(String(INFO_COMPOSER).Append(":").Append(value));

		else if (id == "TRACK")
		{
			info.track = value.ToInt();

			if (value.Find("/") >= 0) info.numTracks = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}
		else if (id == "DISC")
		{
			info.disc = value.ToInt();

			if (value.Find("/") >= 0) info.numDiscs = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}
		else if (id.StartsWith("REPLAYGAIN"))
		{
			if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
			else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
			else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
			else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
		}
		else if (id == "MCDI")
		{
			info.mcdi.SetData(item);
		}
		else if (id.StartsWith("COVER ART") && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
		{
			Picture	 picture;

			/* Read and ignore file name. Then copy
			 * picture data to Picture object.
			 */
			for (Int i = 0; i < item.Size(); i++)
			{
				if (item[i] == 0)
				{
					picture.data.Set(item + i + 1, item.Size() - i - 1);

					break;
				}
			}

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			if	(id.EndsWith("(FRONT)")) picture.type = 3;
			else if (id.EndsWith("(BACK)"))	 picture.type = 4;
			else				 picture.type = 0;

			track.pictures.Add(picture);
		}
	}

	track.SetInfo(info);

	String::SetInputFormat(prevInFormat);

	return Success();
}

Error BoCA::APETag::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(32);

	in.InputData(buffer, 32);

	Int	 tagSize = 0;

	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.InputData(buffer, tagSize);

		return ParseBuffer(buffer, track);
	}

	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

	if (ParseAPEFooter(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.Seek(in.Size() - tagSize);
		in.InputData(buffer, tagSize);

		return ParseBuffer(buffer, track);
	}

	return Error();
}

Bool BoCA::APETag::ParseAPEHeader(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::APETag::ParseAPEFooter(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::APETag::ParseAPEItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, String *value)
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

Bool BoCA::APETag::ParseAPEBinaryItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, Buffer<UnsignedByte> &value)
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

Error BoCA::APETag::UpdateStreamInfo(const String &fileName, const Track &track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(32);

	in.InputData(buffer, 32);

	Int	 tagSize = 0;

	/* Check for APEv2 tag at the beginning of the file.
	 */
	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		/* Copy to temporary file.
		 */
		in.RelSeek(tagSize);

		String		 tempFile = String(fileName).Append(".boca.temp");
		OutStream	 out(STREAM_FILE, tempFile, OS_APPEND);

		if (out.GetLastError() == IO_ERROR_OK)
		{
			Buffer<UnsignedByte>	 buffer;

			RenderBuffer(buffer, track);

			out.OutputData(buffer, buffer.Size());

			buffer.Resize(1024);

			for (Int i = in.GetPos(); i < in.Size(); i += buffer.Size())
			{
				Int	 bytes = Math::Min(Int64(buffer.Size()), in.Size() - i);

				in.InputData(buffer, bytes);
				out.OutputData(buffer, bytes);
			}

			in.Close();
			out.Close();

			File(fileName).Delete();
			File(tempFile).Move(fileName);

			return Success();
		}

		return Error();
	}

	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

	/* Check for APEv2 tag at the end of the file.
	 */
	if (ParseAPEFooter(buffer, &tagSize, NIL))
	{
		/* Check if this tag also has a header.
		 */
		in.RelSeek(-12);

		if (in.InputNumber(4) & 0x80000000) tagSize += 32;

		/* Copy to temporary file.
		 */
		in.Seek(0);

		String		 tempFile = String(fileName).Append(".boca.temp");
		OutStream	 out(STREAM_FILE, tempFile, OS_APPEND);

		if (out.GetLastError() == IO_ERROR_OK)
		{
			Buffer<UnsignedByte>	 buffer;

			buffer.Resize(1024);

			for (Int i = tagSize; i < in.Size(); i += buffer.Size())
			{
				Int	 bytes = Math::Min(Int64(buffer.Size()), in.Size() - i);

				in.InputData(buffer, bytes);
				out.OutputData(buffer, bytes);
			}

			buffer.Resize(0);

			RenderBuffer(buffer, track);

			out.OutputData(buffer, buffer.Size());

			in.Close();
			out.Close();

			File(fileName).Delete();
			File(tempFile).Move(fileName);

			return Success();
		}

		return Error();
	}

	in.Close();

	OutStream	 out(STREAM_FILE, fileName, OS_APPEND);

	/* Append new APEv2 tag.
	 */
	if (out.GetLastError() == IO_ERROR_OK)
	{
		Buffer<UnsignedByte>	 buffer;

		RenderBuffer(buffer, track);

		out.OutputData(buffer, buffer.Size());

		return Success();
	}

	return Error();
}
