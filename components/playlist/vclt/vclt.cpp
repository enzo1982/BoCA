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
	String::InputFormat	 inputFormat("UTF-8");
	InStream		 in(STREAM_FILE, file, IS_READ);

	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine();

		if (!line.StartsWith("FILENAME=")) continue;

		/* Get file name.
		 */
		String	 fileName = line.Tail(line.Length() - 9);

		/* Handle relative paths.
		 */
		if (Utilities::IsRelativePath(fileName)) fileName = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(fileName);

		/* Add track to track list.
		 */
		Track	 track;

		track.fileName = fileName;

		trackList.Add(track);
	}

	in.Close();

	return trackList;
}

Error BoCA::PlaylistVCLT::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String::OutputFormat	 outputFormat("UTF-8");

	String			 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream		 out(STREAM_FILE, actualFile, OS_REPLACE);

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
			Int64	 mseconds = 0;

			if	(track.length	    >= 0) mseconds = Math::Round(Float(track.length)	   / (Float(format.rate) / 1000));
			else if (track.approxLength >= 0) mseconds = Math::Round(Float(track.approxLength) / (Float(format.rate) / 1000));

			Int	 hours	 = mseconds / 1000 / 3600;
			Int	 minutes = mseconds / 1000 % 3600 / 60;
			Int	 seconds = mseconds / 1000 % 3600 % 60;
			Int	 msecs	 = mseconds % 1000;

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

	return Success();
}
