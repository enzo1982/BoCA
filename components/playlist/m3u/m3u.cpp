 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "m3u.h"

using namespace smooth::IO;

const String &BoCA::PlaylistM3U::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>M3U Playlist</name>			\
	    <version>1.0</version>			\
	    <id>m3u-playlist</id>			\
	    <type>playlist</type>			\
	    <format>					\
	      <name>M3U Playlist</name>			\
	      <extension>m3u</extension>		\
	    </format>					\
	    <format>					\
	      <name>M3U Playlist (Unicode)</name>	\
	      <extension>m3u8</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::PlaylistM3U::PlaylistM3U()
{
}

BoCA::PlaylistM3U::~PlaylistM3U()
{
}

Bool BoCA::PlaylistM3U::CanOpenFile(const String &file)
{
	String	 lcFile = file.ToLower();

	return lcFile.EndsWith(".m3u") ||
	       lcFile.EndsWith(".m3u8");
}

const Array<BoCA::Track> &BoCA::PlaylistM3U::ReadPlaylist(const String &file)
{
	String		 format = String::SetInputFormat("UTF-8");
	InStream	 in(STREAM_FILE, file, IS_READ);

	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine();

		if (line != NIL && !line.StartsWith("#"))
		{
			Track	 track;

			track.origFilename = line;

#ifdef __WIN32__
			if (track.origFilename[1] != ':' && !track.origFilename.StartsWith("\\\\") && !track.origFilename.Contains("://"))
#else
			if (!track.origFilename.StartsWith(Directory::GetDirectoryDelimiter()) && !track.origFilename.StartsWith("~") && !track.origFilename.Contains("://"))
#endif
			{
				track.origFilename = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(track.origFilename);
			}

			trackList.Add(track);
		}
	}

	in.Close();

	String::SetInputFormat(format);

	return trackList;
}

Error BoCA::PlaylistM3U::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String		 format;

	if (file.EndsWith(".m3u8")) format = String::SetOutputFormat("UTF-8");
	else			    format = String::SetOutputFormat("ISO-8859-1");

	String		 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream	 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create playlist file:\n\n%1", actualFile);

		return Error();
	}

	BoCA::I18n	*i18n = BoCA::I18n::Get();

	out.OutputLine("#EXTM3U");

	foreach (const Track &track, trackList)
	{
		/* Special handling for CD tracks on Windows.
		 */
		String		 fileName = Utilities::GetCDTrackFileName(track);

		/* Write info to file.
		 */
		const Info	&info = track.GetInfo();

		out.OutputLine(String("#EXTINF:")
			      .Append(String::FromInt(track.length == -1 ? -1 : Math::Round((Float) track.length / track.GetFormat().rate))).Append(",")
			      .Append(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")).Append(" - ").Append(info.title.Length() > 0 ? info.title : i18n->TranslateString("unknown title")));
		out.OutputLine(Utilities::GetRelativeFileName(fileName, actualFile));
	}

	out.Close();

	String::SetOutputFormat(format);

	return Success();
}
