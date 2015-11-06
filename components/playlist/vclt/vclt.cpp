 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "vclt.h"

using namespace smooth::IO;

const String &BoCA::PlaylistVCLT::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>VCLT Playlist</name>			\
	    <version>1.0</version>			\
	    <id>vclt-playlist</id>			\
	    <type>playlist</type>			\
	    <format>					\
	      <name>VCLT Playlist</name>		\
	      <extension>vclt</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::PlaylistVCLT::PlaylistVCLT()
{
}

BoCA::PlaylistVCLT::~PlaylistVCLT()
{
}

Bool BoCA::PlaylistVCLT::CanOpenFile(const String &file)
{
	return file.ToLower().EndsWith(".vclt");
}

const Array<BoCA::Track> &BoCA::PlaylistVCLT::ReadPlaylist(const String &file)
{
	String		 format = String::SetInputFormat("UTF-8");
	InStream	 in(STREAM_FILE, file, IS_READ);

	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine();

		if (line.StartsWith("FILENAME="))
		{
			Track	 track;

			track.origFilename = line.Tail(line.Length() - 9);

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

Error BoCA::PlaylistVCLT::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String		 format = String::SetOutputFormat("UTF-8");

	String		 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream	 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create playlist file:\n\n%1", actualFile);

		return Error();
	}

	foreach (const Track &track, trackList)
	{
		/* Special handling for CD tracks on Windows.
		 */
		String		 fileName = Utilities::GetCDTrackFileName(track);

		/* Convert directory delimiters unless drive is mentioned.
		 */
		fileName = Utilities::GetRelativeFileName(fileName, actualFile);

		if (fileName[1] != ':') fileName.Replace("\\", "/");

		/* Write info to file.
		 */
		const Info	&info	= track.GetInfo();
		const Format	&format	= track.GetFormat();

		out.OutputString(String("FILENAME=").Append(fileName).Append("\n"));

		out.OutputString(String("AUDIOINFO=").Append("rate:").Append(String::FromInt(format.rate)).Append("Hz ")
						     .Append("bits:").Append(String::FromInt(format.bits)).Append(" ")
						     .Append("channels:").Append(String::FromInt(format.channels)).Append("\n"));

		if (track.length >= 0 || track.approxLength >= 0)
		{
			Int64	 length	  = track.length >= 0 ? track.length : track.approxLength;

			Int	 hours	 = length / format.rate / 3600;
			Int	 minutes = length / format.rate % 3600 / 60;
			Int	 seconds = length / format.rate % 3600 % 60;
			Int	 msecs	 = Math::Round(Math::Fract(Float(length) / format.rate) * 1000);

			out.OutputString(String("LENGTH=").Append(hours > 0 ? String(			 hours   <  10 ? "0" : NIL).Append(String::FromInt(hours)).Append(":") : String())
							  .Append(	      String(			 minutes <  10 ? "0" : NIL).Append(String::FromInt(minutes))).Append(":")
							  .Append(	      String(			 seconds <  10 ? "0" : NIL).Append(String::FromInt(seconds))).Append(".")
							  .Append(	      String(msecs < 10 ? "00" : msecs   < 100 ? "0" : NIL).Append(String::FromInt(msecs))).Append("\n"));
		}

		if (info.artist	!= NIL) out.OutputString(String("ARTIST=").Append(info.artist).Append("\n"));
		if (info.title	!= NIL) out.OutputString(String("TITLE=").Append(info.title).Append("\n"));
		if (info.album	!= NIL) out.OutputString(String("ALBUM=").Append(info.album).Append("\n"));
		if (info.track	 >   0) out.OutputString(String("TRACKNUMBER=").Append(String(info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track))).Append("\n"));

		out.OutputString("==\n");
	}

	out.Close();

	String::SetOutputFormat(format);

	return Success();
}
