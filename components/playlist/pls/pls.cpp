 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "pls.h"

using namespace smooth::IO;

const String &BoCA::PlaylistPLS::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>PLS Playlist</name>			\
	    <version>1.0</version>			\
	    <id>pls-playlist</id>			\
	    <type>playlist</type>			\
	    <format>					\
	      <name>PLS Playlist</name>			\
	      <extension>pls</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::PlaylistPLS::PlaylistPLS()
{
}

BoCA::PlaylistPLS::~PlaylistPLS()
{
}

Bool BoCA::PlaylistPLS::CanOpenFile(const String &file)
{
	InStream		 in(STREAM_FILE, file, IS_READ);

	/* Skip UTF-8 BOM.
	 */
	String::InputFormat	 inputFormat("ISO-8859-1");

	if (in.InputNumberRaw(3) != 0xEFBBBF) in.Seek(0);

	/* Check file format.
	 */
	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine();

		if	(line == "[playlist]")		       return True;
		else if (line != NIL && !line.StartsWith(";")) return False;
	}

	return False;
}

const Array<BoCA::Track> &BoCA::PlaylistPLS::ReadPlaylist(const String &file)
{
	InStream		 in(STREAM_FILE, file, IS_READ);

	/* Detect input format.
	 */
	String::InputFormat	 inputFormat("ISO-8859-1");

	if (in.InputNumberRaw(3) == 0xEFBBBF)
	{
		/* Found UTF-8 BOM.
		 */
		String::SetInputFormat("UTF-8");
	}
	else
	{
		in.Seek(0);

		/* Check for UTF-8, otherwise use ISO-8859-1.
		 */
		String	 data = in.InputString(in.Size());
		String	 dataUTF;

		if (dataUTF.ImportFrom("UTF-8", data.ConvertTo("ISO-8859-1")) == Success() && dataUTF != data)
		{
			/* Encoding appears to be UTF-8.
			 */
			String::SetInputFormat("UTF-8");
		}

		in.Seek(0);
	}

	/* Parse file line by line.
	 */
	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine();

		if (!line.StartsWith("File")) continue;

		/* Get file name.
		 */
		String	 fileName = line.Tail(line.Length() - line.Find("=") - 1);

		/* Handle relative paths.
		 */
		String	 resolvedFileName = fileName;

		if (Utilities::IsRelativePath(resolvedFileName)) resolvedFileName = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(resolvedFileName);

		/* If file is not found, try interpreting the file name using the default system encoding.
		 */
		if (!File(resolvedFileName).Exists())
		{
			String::InputFormat	 inputFormat(String::GetDefaultEncoding());
			String			 nativeFileName = fileName.ConvertTo("ISO-8859-1");

			if (Utilities::IsRelativePath(nativeFileName)) nativeFileName = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(nativeFileName);

			if (File(nativeFileName).Exists()) resolvedFileName = nativeFileName;
		}

		/* Add track to track list.
		 */
		Track	 track;

		track.fileName = resolvedFileName;

		trackList.Add(track);
	}

	in.Close();

	return trackList;
}

Error BoCA::PlaylistPLS::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String::OutputFormat	 outputFormat("ISO-8859-1");

	String			 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream		 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create playlist file:\n\n%1", actualFile);

		return Error();
	}

	BoCA::I18n	*i18n = BoCA::I18n::Get();

	out.OutputLine("[playlist]");
	out.OutputLine("Version=2");
	out.OutputLine(NIL);
	out.OutputLine(String("NumberOfEntries=").Append(String::FromInt(trackList.Length())));

	for (Int i = 0; i < trackList.Length(); i++)
	{
		const Track	&track = trackList.GetNth(i);

		/* Special handling for CD tracks on Windows.
		 */
		String		 fileName = Utilities::GetCDTrackFileName(track);

		/* Write info to file.
		 */
		const Info	&info = track.GetInfo();

		out.OutputLine(NIL);
		out.OutputLine(String("File").Append(String::FromInt(i + 1)).Append("=").Append(Utilities::GetRelativeFileName(fileName, actualFile)));
		out.OutputLine(String("Title").Append(String::FromInt(i + 1)).Append("=").Append(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")).Append(" - ").Append(info.title.Length() > 0 ? info.title : i18n->TranslateString("unknown title")));
		out.OutputLine(String("Length").Append(String::FromInt(i + 1)).Append("=").Append(String::FromInt(track.length == -1 ? -1 : Math::Round((Float) track.length / track.GetFormat().rate))));
	}

	out.Close();

	return Success();
}
