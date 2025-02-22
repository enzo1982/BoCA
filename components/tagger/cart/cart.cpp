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

#include "cart.h"

using namespace smooth::IO;

const String &BoCA::TaggerCart::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>RIFF Cart Tagger</name>				\
	    <version>1.0</version>					\
	    <id>cart-tag</id>						\
	    <type>tagger</type>						\
	    <format>							\
	      <name>Microsoft Wave Files</name>				\
	      <extension>wav</extension>				\
	    </format>							\
	    <format>							\
	      <name>RIFF 64 Audio Files</name>				\
	      <extension>rf64</extension>				\
	    </format>							\
	    <tagspec default=\"false\">					\
	      <name>RIFF Cart Tag</name>				\
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

const String	 BoCA::TaggerCart::ConfigID = "Tags";

BoCA::TaggerCart::TaggerCart()
{
}

BoCA::TaggerCart::~TaggerCart()
{
}

Error BoCA::TaggerCart::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig = GetConfiguration();

	String		 encodingID    = currentConfig->GetStringValue(ConfigID, "RIFFCartTagEncoding", "ISO-8859-1");

	/* Set output encoding.
	 */
	String::OutputFormat	 outputFormat(encodingID);

	const Info	&info = track.GetInfo();
	const Format	&format = track.GetFormat();

	buffer.Resize(8);

	/* Cart chunk version 1.01.
	 */
	RenderStringItem("0101", 4, buffer);

	/* Artist and title.
	 */
	RenderStringItem(info.title, 64, buffer);
	RenderStringItem(info.artist, 64, buffer);

	/* Cut number.
	 */
	RenderStringItem(NIL, 64, buffer);

	/* Client ID.
	 */
	RenderStringItem(NIL, 64, buffer);

	/* Category.
	 */
	RenderStringItem("MUS", 64, buffer);

	/* Classification.
	 */
	RenderStringItem(NIL, 64, buffer);

	/* Out cue.
	 */
	RenderStringItem(info.album, 64, buffer);

	/* Start date and time.
	 */
	RenderStringItem("1900-01-01", 10, buffer);
	RenderStringItem("00:00:00", 8, buffer);

	/* End date and time.
	 */
	RenderStringItem("9999-12-31", 10, buffer);
	RenderStringItem("23:59:59", 8, buffer);

	/* Application ID and version.
	 */
	RenderStringItem(Application::Get()->getClientName.Call(), 64, buffer);
	RenderStringItem(Application::Get()->getClientVersion.Call(), 64, buffer);

	/* User defined string.
	 */
	RenderStringItem(String(info.genre).Append(info.year > 0 ? String("|").Append(String::FromInt(info.year)) : String()), 64, buffer);

	/* Level reference.
	 */
	RenderIntegerItem(Math::Pow(2, format.bits - 1), 4, buffer);

	/* Timers.
	 */
	for (Int i = 0; i < 7; i++) RenderTimerItem(NIL, 0, buffer);

	if (track.length >= 0)	RenderTimerItem("EOD", track.length, buffer);
	else			RenderTimerItem(NIL, 0, buffer);

	/* Reserved.
	 */
	RenderStringItem(NIL, 276, buffer);

	/* URL.
	 */
	RenderStringItem(NIL, 1024, buffer);

	/* Tag header.
	 */
	RenderTagHeader(buffer);

	return Success();
}

Int BoCA::TaggerCart::RenderTagHeader(Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 8);

	out.OutputString("cart");
	out.OutputNumber(buffer.Size() - 8, 4);

	return Success();
}

Int BoCA::TaggerCart::RenderStringItem(const String &value, Int bufferSize, Buffer<UnsignedByte> &buffer)
{
	buffer.Resize(buffer.Size() + bufferSize);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - bufferSize, bufferSize);

	Int		 stringSize = (value.Trim() != NIL) ? strlen(value.Trim()) : 0;

	out.OutputString(stringSize <= bufferSize ? value.Trim() : value.Trim().Head(bufferSize));

	for (Int i = 0; i < bufferSize - stringSize; i++) out.OutputNumber(0, 1);

	return Success();
}

Int BoCA::TaggerCart::RenderIntegerItem(Int value, Int bufferSize, Buffer<UnsignedByte> &buffer)
{
	buffer.Resize(buffer.Size() + bufferSize);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - bufferSize, bufferSize);

	out.OutputNumber(value, bufferSize);

	return Success();
}

Int BoCA::TaggerCart::RenderTimerItem(const String &fourcc, Int value, Buffer<UnsignedByte> &buffer)
{
	buffer.Resize(buffer.Size() + 8);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - 8, 8);
	Int		 fourccSize = (fourcc != NIL) ? strlen(fourcc) : 0;

	out.OutputString(fourccSize <= 4 ? fourcc : fourcc.Head(4));

	for (Int i = 0; i < 4 - fourccSize; i++) out.OutputNumber(0, 1);

	out.OutputNumber(value, 4);

	return Success();
}

Error BoCA::TaggerCart::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());
	Bool		 error = False;

	/* Read cart chunk.
	 */
	if (in.InputString(4) != "cart") error = True;

	/* Skip header.
	 */
	in.RelSeek(4);

	/* Parse individual comment items.
	 */
	Info			 info = track.GetInfo();
	String::InputFormat	 inputFormat("ISO-8859-1");

	/* Cart chunk version.
	 */
	String	 version = in.InputString(4);

	/* Continue if version == 1.x.
	 */
	if (version.ToInt() >= 100 && version.ToInt() < 200)
	{
		/* Artist and title.
		 */
		info.title  = in.InputString(64).Trim();
		info.artist = in.InputString(64).Trim();

		/* Cut number.
		 */
		in.RelSeek(64);

		/* Client ID.
		 */
		in.RelSeek(64);

		/* Category.
		 */
		in.RelSeek(64);

		/* Classification.
		 */
		in.RelSeek(64);

		/* Out cue.
		 */
		in.RelSeek(64);

		/* Start date and time.
		 */
		in.RelSeek(18);

		/* End date and time.
		 */
		in.RelSeek(18);

		/* Application ID and version.
		 */
		in.RelSeek(64);
		in.RelSeek(64);

		/* User defined string.
		 */
		in.RelSeek(64);

		/* Level reference.
		 */
		in.RelSeek(4);

		/* Timers.
		 */
		for (Int i = 0; i < 8; i++) in.RelSeek(8);

		/* Reserved.
		 */
		in.RelSeek(276);

		/* URL.
		 */
		in.RelSeek(1024);
	}

	track.SetInfo(info);

	if (error) return Error();
	else	   return Success();
}

Error BoCA::TaggerCart::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Read RIFF chunk.
	 */
	String		 riff  = in.InputString(4);

	if (riff != "RIFF" && riff != "RF64") return Error();

	UnsignedInt32	 rSize = in.InputNumber(4);

	in.RelSeek(4);

	/* Find and parse cart chunk.
	 */
	Bool	 error = False;
	String	 chunk;
	Int64	 dSize = -1;

	while (!error && chunk != "cart")
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		chunk = in.InputString(4);

		UnsignedInt64	 cSize = (UnsignedInt32) in.InputNumber(4);

		if (chunk == "cart")
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

	if (error) return Error();

	return Success();
}

Error BoCA::TaggerCart::UpdateStreamInfo(const String &fileName, const Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Read RIFF chunk.
	 */
	Bool		 error = False;
	String		 riff  = in.InputString(4);

	if (riff != "RIFF" && riff != "RF64") return Error();

	/* Create output file.
	 */
	OutStream	 out(STREAM_FILE, fileName.Append(".temp"), OS_REPLACE);

	UnsignedInt32	 rSize	 = in.InputNumber(4);
	UnsignedInt32	 rFormat = in.InputNumber(4);

	out.OutputString(riff);
	out.OutputNumber(rSize, 4);
	out.OutputNumber(rFormat, 4);

	Int64			 dSize = -1;
	Buffer<UnsignedByte>	 buffer(131072);

	while (!error)
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		String		 chunk = in.InputString(4);
		UnsignedInt64	 cSize = (UnsignedInt32) in.InputNumber(4);

		if (chunk == "cart")
		{
			/* Skip cart chunk.
			 */
			if (!in.RelSeek(cSize + cSize % 2)) error = True;

			continue;
		}

		out.OutputString(chunk);
		out.OutputNumber(cSize, 4);

		if (chunk == "ds64")
		{
			in.RelSeek(8);

			dSize = in.InputNumber(8);

			in.RelSeek(-16);
		}
		else if (chunk == "data")
		{
			if (rSize == 0xFFFFFFFF || rSize == 0 ||
			    cSize == 0xFFFFFFFF || cSize == 0) error = True;

			if (dSize >= 0) cSize = dSize;
		}

		/* Write chunk data to output file.
		 */
		cSize += cSize % 2;

		while (!error && cSize > 0)
		{
			Int64	 bytes = Math::Min(cSize, buffer.Size());

			in.InputData(buffer, bytes);
			out.OutputData(buffer, bytes);

			cSize -= bytes;
		}
	}

	/* Write new metadata.
	 */
	if (!error)
	{
		/* Write cart chunk.
		 */
		buffer.Resize(0);

		RenderBuffer(buffer, track);

		out.OutputData(buffer, buffer.Size());

		/* Update file size.
		 */
		UnsignedInt64	 fileSize = out.GetPos() - 8;

		rSize = Math::Min(0xFFFFFFFF, fileSize);

		out.Seek(4);
		out.OutputNumber(rSize, 4);
	}

	/* Clean up.
	 */
	in.Close();
	out.Close();

	if (error)
	{
		File(fileName.Append(".temp")).Delete();

		return Error();
	}

	File(fileName).Delete();
	File(fileName.Append(".temp")).Move(fileName);

	return Success();
}
