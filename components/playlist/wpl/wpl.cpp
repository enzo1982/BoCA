 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "wpl.h"

using namespace smooth::IO;

const String &BoCA::PlaylistWPL::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Windows Media Playlist</name>		\
	    <version>1.0</version>			\
	    <id>wpl-playlist</id>			\
	    <type>playlist</type>			\
	    <format>					\
	      <name>Windows Media Playlist</name>	\
	      <extension>wpl</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::PlaylistWPL::PlaylistWPL()
{
}

BoCA::PlaylistWPL::~PlaylistWPL()
{
}

Bool BoCA::PlaylistWPL::CanOpenFile(const String &file)
{
	InStream	 in(STREAM_FILE, file, IS_READ);

	if (in.InputLine().Contains("<?wpl ")) return True;
	else				       return False;
}

const Array<BoCA::Track> &BoCA::PlaylistWPL::ReadPlaylist(const String &file)
{
	/* Read file and replace WPL marker with XML.
	 */
	String		 format = String::SetInputFormat("UTF-8");
	InStream	 in(STREAM_FILE, file, IS_READ);

	String		 wpl	 = in.InputString(in.Size()).Replace("<?wpl ", "<?xml ");
	char		*wplChar = wpl.ConvertTo("UTF-8");

	in.Close();

	String::SetInputFormat(format);

	/* Parse XML document.
	 */
	XML::Document	*doc = new XML::Document();

	doc->ParseMemory(wplChar, strlen(wplChar));

	XML::Node	*body = doc->GetRootNode()->GetNodeByName("body");

	if (body == NIL) { delete doc; return trackList; }

	XML::Node	*seq = body->GetNodeByName("seq");

	if (seq == NIL) { delete doc; return trackList; }

	for (Int i = 0; i < seq->GetNOfNodes(); i++)
	{
		XML::Node	*media = seq->GetNthNode(i);

		if (media->GetName() != "media") continue;

		XML::Attribute	*src = media->GetAttributeByName("src");

		if (src == NIL) continue;

		/* Add track to track list.
		 */
		Track	 track;

		track.origFilename = src->GetContent();

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

	delete doc;

	return trackList;
}

Error BoCA::PlaylistWPL::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String		 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream	 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create playlist file:\n\n%1", actualFile);

		return Error();
	}

	/* Create XML document.
	 */
	XML::Document	*doc = new XML::Document();

	XML::Node	*smil = new XML::Node("smil");
	XML::Node	*head = smil->AddNode("head");

	XML::Node	*itemCount = head->AddNode("meta");

	itemCount->SetAttribute("name", "ItemCount");
	itemCount->SetAttribute("content", String::FromInt(trackList.Length()));

	XML::Node	*body = smil->AddNode("body");
	XML::Node	*seq  = body->AddNode("seq");

	/* Add tracks.
	 */
	foreach (const Track &track, trackList)
	{
		/* Special handling for CD tracks on Windows.
		 */
		String		 fileName = Utilities::GetRelativeFileName(Utilities::GetCDTrackFileName(track), actualFile);

		/* Add info to XML.
		 */
		XML::Node	*media = seq->AddNode("media");

		media->SetAttribute("src", fileName);
	}

	/* Write XML to temporary file.
	 */
	doc->SetEncoding("UTF-8");
	doc->SetRootNode(smil);

	doc->SaveFile(String(actualFile).Append(".xml"));

	delete doc;
	delete smil;

	/* Convert to WPL and clean up.
	 */
	String::InputFormat	 inputFormat("UTF-8");
	String::OutputFormat	 outputFormat("UTF-8");

	InStream	 in(STREAM_FILE, String(actualFile).Append(".xml"), IS_READ);

	String		 wpl = in.InputString(in.Size()).Replace("<?xml ", "<?wpl ");

	out.OutputString(wpl);

	in.Close();
	out.Close();

	File(String(actualFile).Append(".xml")).Delete();

	return Success();
}
