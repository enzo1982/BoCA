 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include "apev2.h"

using namespace smooth::IO;

const String &BoCA::TaggerAPEv2::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>APEv2 Tagger</name>				\
	    <version>1.0</version>				\
	    <id>apev2-tag</id>					\
	    <type>tagger</type>					\
	    <format>						\
	      <name>Monkey's Audio</name>			\
	      <extension>ape</extension>			\
	      <extension>mac</extension>			\
	    </format>						\
	    <tagspec>						\
	      <name>APEv2</name>				\
	      <coverart supported=\"true\"/>			\
	      <prependzero allowed=\"true\" default=\"true\"/>	\
	      <encodings>					\
		<encoding>UTF-8</encoding>			\
	      </encodings>					\
	    </tagspec>						\
	  </component>						\
								\
	";

	return componentSpecs;
}

const String	 BoCA::TaggerAPEv2::ConfigID = "Tags";

BoCA::TaggerAPEv2::TaggerAPEv2()
{
}

BoCA::TaggerAPEv2::~TaggerAPEv2()
{
}

Error BoCA::TaggerAPEv2::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig		 = GetConfiguration();

	Bool		 prependZero		 = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroAPEv2", True);

	Bool		 writeMCDI		 = currentConfig->GetIntValue(ConfigID, "WriteMCDI", True);

	Bool		 preserveReplayGain	 = currentConfig->GetIntValue(ConfigID, "PreserveReplayGain", True);

	Bool		 coverArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 coverArtWriteToAPEv2	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToAPEv2", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Set output encoding.
	 */
	String::OutputFormat	 outputFormat("UTF-8");

	/* Save basic information.
	 */
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
		String	 trackString = String(prependZero && info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(prependZero && info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks));

		{ RenderAPEItem("Track", trackString, buffer); numItems++; }
	}

	if (info.disc > 0)
	{
		String	 discString = String(prependZero && info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(prependZero && info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs));

		{ RenderAPEItem("Disc", discString, buffer); numItems++; }
	}

	if	(info.comment != NIL && !replaceExistingComments) { RenderAPEItem("Comment", info.comment, buffer);   numItems++; }
	else if (defaultComment != NIL && numItems > 0)		  { RenderAPEItem("Comment", defaultComment, buffer); numItems++; }

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST)    { RenderAPEItem("Album Artist",   value, buffer); numItems++; }

		else if	(key == INFO_CONTENTGROUP)   { RenderAPEItem("Grouping",       value, buffer); numItems++; }
		else if	(key == INFO_SUBTITLE)	     { RenderAPEItem("Subtitle",       value, buffer); numItems++; }

		else if	(key == INFO_CONDUCTOR)	     { RenderAPEItem("Conductor",      value, buffer); numItems++; }
		else if	(key == INFO_REMIX)	     { RenderAPEItem("MixArtist",      value, buffer); numItems++; }
		else if	(key == INFO_COMPOSER)	     { RenderAPEItem("Composer",       value, buffer); numItems++; }

		else if	(key == INFO_BPM)	     { RenderAPEItem("BPM",	       value, buffer); numItems++; }

		else if	(key == INFO_COPYRIGHT)	     { RenderAPEItem("Copyright",      value, buffer); numItems++; }
		else if	(key == INFO_CATALOGNUMBER)  { RenderAPEItem("CatalogNumber",  value, buffer); numItems++; }

		else if	(key == INFO_RELEASECOUNTRY) { RenderAPEItem("ReleaseCountry", value, buffer); numItems++; }

		else if	(key == INFO_DISCSUBTITLE)   { RenderAPEItem("DiscSubtitle",   value, buffer); numItems++; }
	}

	/* Save Replay Gain info.
	 */
	if (preserveReplayGain)
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
	if (writeMCDI)
	{
		if (info.mcdi.GetData().Size() > 0) { RenderAPEBinaryItem("MCDI", info.mcdi.GetData(), buffer); numItems++; }
	}

	/* Save cover art.
	 */
	if (coverArtWriteToTags && coverArtWriteToAPEv2)
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			String			 itemName = "Cover Art";
			Buffer<UnsignedByte>	 picBuffer(picInfo.data.Size() + 19);

			if	(picInfo.type == 3) itemName.Append(" (front)");
			else if	(picInfo.type == 4) itemName.Append(" (back)");
			else			    itemName.Append(" (other)");

			if (picInfo.mime == "image/png") strncpy((char *) (unsigned char *) picBuffer, "c:\\music\\cover.png", 19);
			else				 strncpy((char *) (unsigned char *) picBuffer, "c:\\music\\cover.jpg", 19);

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

	return Success();
}

Int BoCA::TaggerAPEv2::RenderAPEHeader(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
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

Int BoCA::TaggerAPEv2::RenderAPEFooter(Int tagSize, Int numItems, Buffer<UnsignedByte> &buffer)
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

Int BoCA::TaggerAPEv2::RenderAPEItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value.Trim()) + 9;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(strlen(value.Trim()), 4);
	out.OutputNumber(0, 4);
	out.OutputString(id);
	out.OutputNumber(0, 1);
	out.OutputString(value.Trim());

	return Success();
}

Int BoCA::TaggerAPEv2::RenderAPEBinaryItem(const String &id, const Buffer<UnsignedByte> &value, Buffer<UnsignedByte> &buffer)
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

Error BoCA::TaggerAPEv2::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	Int		 numItems = 0;
	Int		 offset = 32;

	if (!ParseAPEHeader(buffer, NIL, &numItems))
	{
		offset = 0;

		if (!ParseAPEFooter(buffer, NIL, &numItems)) return Error();
	}

	String::InputFormat	 inputFormat("UTF-8");

	Info	 info = track.GetInfo();

	for (Int i = 0; i < numItems; i++)
	{
		String			 id;
		String			 value;
		Buffer<UnsignedByte>	 item;

		ParseAPEItem(buffer, offset, &id, &value);

		if	(id != "!Binary" && value == NIL) continue;
		else if (id == "!Binary"		) ParseAPEBinaryItem(buffer, offset, &id, item);

		id = id.ToUpper();

		if	(id == "ARTIST")	 info.artist  = value;
		else if (id == "TITLE")		 info.title   = value;
		else if (id == "ALBUM")		 info.album   = value;
		else if (id == "YEAR")		 info.year    = value.ToInt();
		else if (id == "GENRE")		 info.genre   = value;
		else if (id == "COMMENT")	 info.comment = value;
		else if (id == "PUBLISHER")	 info.label   = value;
		else if (id == "LABEL")		 info.label   = value;
		else if (id == "ISRC")		 info.isrc    = value;

		else if (id == "ALBUM ARTIST")	 info.SetOtherInfo(INFO_ALBUMARTIST,	value);

		else if (id == "GROUPING")	 info.SetOtherInfo(INFO_CONTENTGROUP,	value);
		else if (id == "SUBTITLE")	 info.SetOtherInfo(INFO_SUBTITLE,	value);

		else if (id == "CONDUCTOR")	 info.SetOtherInfo(INFO_CONDUCTOR,	value);
		else if (id == "MIXARTIST")	 info.SetOtherInfo(INFO_REMIX,		value);
		else if (id == "COMPOSER")	 info.SetOtherInfo(INFO_COMPOSER,	value);

		else if (id == "BPM")		 info.SetOtherInfo(INFO_BPM,		value);

		else if (id == "COPYRIGHT")	 info.SetOtherInfo(INFO_COPYRIGHT,	value);
		else if (id == "CATALOGNUMBER")  info.SetOtherInfo(INFO_CATALOGNUMBER,	value);

		else if (id == "RELEASECOUNTRY") info.SetOtherInfo(INFO_RELEASECOUNTRY,	value);

		else if (id == "DISCSUBTITLE")	 info.SetOtherInfo(INFO_DISCSUBTITLE,	value);

		else if (id == "TRACK")
		{
			info.track = value.ToInt();

			if (value.Contains("/")) info.numTracks = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}
		else if (id == "DISC")
		{
			info.disc = value.ToInt();

			if (value.Contains("/")) info.numDiscs = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
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
		else if (id.StartsWith("COVER ART"))
		{
			if (currentConfig->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
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

				if	(id.EndsWith("(FRONT)")) picture.type = 3; // Cover (front)
				else if (id.EndsWith("(BACK)"))	 picture.type = 4; // Cover (back)
				else				 picture.type = 0; // Other

				if (picture.data.Size() >= 16)
				{
					if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
					else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
						 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
						 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
						 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

					if (picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
				}
			}
		}
		else if (id == "CUESHEET")
		{
			if (currentConfig->GetIntValue(ConfigID, "ReadEmbeddedCueSheets", True))
			{
				/* Output cuesheet to temporary file.
				 */
				String		 cuesheet = value.Replace("\r\n", "\n");
				String		 cueFile  = S::System::System::GetTempDirectory().Append("cuesheet_temp_").Append(String::FromInt(S::System::System::Clock())).Append(".cue");
				OutStream	 out(STREAM_FILE, cueFile, OS_REPLACE);

				const Array<String>	&lines = cuesheet.Explode("\n");

				foreach (const String &line, lines)
				{
					if (line.Trim().StartsWith("FILE")) out.OutputLine(String("FILE \"").Append(track.fileName).Append("\" WAVE"));
					else				    out.OutputLine(line);
				}

				String::ExplodeFinish();

				out.Close();

				/* Get cue sheet stream info.
				 */
				AS::Registry		&boca	 = AS::Registry::Get();
				AS::DecoderComponent	*decoder = (AS::DecoderComponent *) boca.CreateComponentByID("cuesheet-dec");

				if (decoder != NIL)
				{
					Track	 cueTrack;
					Config	*cueConfig = Config::Copy(GetConfiguration());

					cueConfig->SetIntValue(ConfigID, "ReadEmbeddedCueSheets", False);

					cueConfig->SetIntValue("CueSheet", "ReadInformationTags", True);
					cueConfig->SetIntValue("CueSheet", "PreferCueSheets", True);
					cueConfig->SetIntValue("CueSheet", "LookForAlternativeFiles", False);
					cueConfig->SetIntValue("CueSheet", "IgnoreErrors", False);

					decoder->SetConfiguration(cueConfig);
					decoder->GetStreamInfo(cueFile, cueTrack);

					boca.DeleteComponent(decoder);

					Config::Free(cueConfig);

					if (cueTrack.tracks.Length() > 0) track.tracks = cueTrack.tracks;
				}

				File(cueFile).Delete();
			}
		}
		else
		{
			/* Save any other tags as user defined text.
			 */
			 info.other.Add(String(INFO_USERTEXT).Append(":").Append(id).Append(":|:").Append(value));
		}
	}

	/* Set artist to album artist if artist is not filled.
	 */
	if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

	track.SetInfo(info);

	return Success();
}

Error BoCA::TaggerAPEv2::ParseStreamInfo(const String &fileName, Track &track)
{
	/* Open file and check size.
	 */
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	if (in.Size() < 32) return Error();

	/* Read header tag.
	 */
	Buffer<UnsignedByte>	 buffer(32);
	Int			 tagSize = 0;

	in.InputData(buffer, 32);

	if (ParseAPEHeader(buffer, &tagSize, NIL))
	{
		buffer.Resize(tagSize);

		in.InputData(buffer, tagSize);

		return ParseBuffer(buffer, track);
	}

	/* Read footer tag.
	 */
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

Bool BoCA::TaggerAPEv2::ParseAPEHeader(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::TaggerAPEv2::ParseAPEFooter(const Buffer<UnsignedByte> &buffer, Int *tagSize, Int *numItems)
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

Bool BoCA::TaggerAPEv2::ParseAPEItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, String *value)
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

	*value = in.InputString(valueBytes).Trim();

	offset += in.GetPos();

	return True;
}

Bool BoCA::TaggerAPEv2::ParseAPEBinaryItem(const Buffer<UnsignedByte> &buffer, Int &offset, String *id, Buffer<UnsignedByte> &value)
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

Error BoCA::TaggerAPEv2::UpdateStreamInfo(const String &fileName, const Track &track)
{
	/* Open file and check size.
	 */
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	if (in.Size() < 32) return Error();

	/* Check for APEv2 tag at the beginning of the file.
	 */
	Buffer<UnsignedByte>	 buffer(32);
	Int			 tagSize = 0;

	in.InputData(buffer, 32);

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

	/* Check for APEv2 tag at the end of the file.
	 */
	in.Seek(in.Size() - 32);
	in.InputData(buffer, 32);

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

	/* Append new APEv2 tag.
	 */
	OutStream	 out(STREAM_FILE, fileName, OS_APPEND);

	if (out.GetLastError() == IO_ERROR_OK)
	{
		Buffer<UnsignedByte>	 buffer;

		RenderBuffer(buffer, track);

		out.OutputData(buffer, buffer.Size());

		return Success();
	}

	return Error();
}
