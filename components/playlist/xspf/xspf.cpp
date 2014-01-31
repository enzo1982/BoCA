 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <xspf/Xspf.h>
#include <uriparser/Uri.h>

#include "xspf.h"

using namespace Xspf;
using namespace smooth::IO;

const String &BoCA::PlaylistXSPF::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>XSPF Playlist</name>			\
	    <version>1.0</version>			\
	    <id>xspf-playlist</id>			\
	    <type>playlist</type>			\
	    <format>					\
	      <name>XSPF Playlist</name>		\
	      <extension>xspf</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

namespace BoCA
{
	class PlaylistXSPFCallback : public XspfReaderCallback
	{
		private:
			PlaylistXSPF	*playlist;
		public:
					 PlaylistXSPFCallback(PlaylistXSPF *);

			void		 addTrack(XspfTrack *);
 	};
};

BoCA::PlaylistXSPF::PlaylistXSPF()
{
}

BoCA::PlaylistXSPF::~PlaylistXSPF()
{
}

Bool BoCA::PlaylistXSPF::CanOpenFile(const String &file)
{
	return file.ToLower().EndsWith(".xspf");
}

const Array<BoCA::Track> &BoCA::PlaylistXSPF::ReadPlaylist(const String &file)
{
	InStream		 in(STREAM_FILE, file, IS_READ);

	/* Create XSPF reader.
	 */
	XspfReader		 reader;
	PlaylistXSPFCallback	 callback(this);

	int			 numBytes = in.Size() + 1;
	char			*memory	  = new char [numBytes];

	in.InputData(memory, numBytes - 1);

	memory[numBytes - 1] = 0;

	reader.parseMemory(memory, numBytes, &callback, String("file://").Append(EncodeURI(file.Replace("\\", "/")).Replace("%3A", ":")));

	delete [] memory;

	/* Complete relative filenames.
	 */
	for (Int i = 0; i < trackList.Length(); i++)
	{
		Track	&track = trackList.GetNthReference(i);

#ifdef __WIN32__
		if (track.origFilename[1] != ':' && !track.origFilename.StartsWith("\\\\") && !track.origFilename.Contains("://"))
#else
		if (!track.origFilename.StartsWith(Directory::GetDirectoryDelimiter()) && !track.origFilename.StartsWith("~") && !track.origFilename.Contains("://"))
#endif
		{
			track.origFilename = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(track.origFilename);
		}
	}

	in.Close();

	return trackList;
}

Error BoCA::PlaylistXSPF::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String			 format  = String::SetOutputFormat("UTF-8");

	String			 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream		 out(STREAM_FILE, actualFile, OS_REPLACE);

	/* Create XSPF writer.
	 */
	XspfIndentFormatter	 formatter;
	XspfWriter		*writer = XspfWriter::makeWriter(formatter, NIL, false);

	/* Add tracks.
	 */
	foreach (const Track &track, trackList)
	{
		/* Special handling for CD tracks on Windows.
		 */
		String		 fileName = Utilities::GetRelativeFileName(Utilities::GetCDTrackFileName(track), actualFile);

		/* Add info to XSPF.
		 */
		const Info	&info = track.GetInfo();

		XspfTrack	 xspfTrack;

		if (info.artist	 != NIL) xspfTrack.lendCreator(info.artist);
		if (info.album	 != NIL) xspfTrack.lendAlbum(info.album);
		if (info.title	 != NIL) xspfTrack.lendTitle(info.title);
		if (info.track   >    0) xspfTrack.setTrackNum(info.track);
		if (track.length >=   0) xspfTrack.setDuration(Math::Round((Float) track.length / track.GetFormat().rate * 1000.0));

		xspfTrack.lendAppendLocation(EncodeURI(fileName.Replace("\\", "/")).Replace("%3A", ":"));

		writer->addTrack(xspfTrack);
	}

	/* Write file and clean up.
	 */
	char	*memory	  = NIL;
	int	 numBytes = 0;

	if (writer->writeMemory(memory, numBytes) == XSPF_WRITER_SUCCESS)
	{
		/* Replace tabs with double spaces in output XSPF.
		 */
		String	 xml;

		xml.ImportFrom("UTF-8", memory);
		xml.Replace("\t", "  ");

		out.OutputString(xml);

		delete [] memory;
	}

	delete writer;

	out.Close();

	String::SetOutputFormat(format);

	return Success();
}

String BoCA::PlaylistXSPF::EncodeURI(const String &uri)
{
	char	*encoded = new char [12 * uri.Length() + 1];

	uriEscapeA(uri.ConvertTo("UTF-8"), encoded, false, false);

	String	 result;

	result.ImportFrom("ISO-8859-1", encoded);
	result.Replace("%2F", "/");

	delete [] encoded;

	return result;
}

String BoCA::PlaylistXSPF::DecodeURI(const String &uri)
{
	char	*decoded = new char [uri.Length() + 1];

	strcpy(decoded, uri.ConvertTo("ISO-8859-1"));

	uriUnescapeInPlaceA(decoded);

	String	 result;

	result.ImportFrom("UTF-8", decoded);
	result.Replace("%2F", "/");

	delete [] decoded;

	return result;
}

BoCA::PlaylistXSPFCallback::PlaylistXSPFCallback(PlaylistXSPF *playlist)
{
	this->playlist = playlist;
}

void BoCA::PlaylistXSPFCallback::addTrack(XspfTrack *xspfTrack)
{
	String	 inFormat = String::SetInputFormat("UTF-8");

	Track	 track;
	Info	 info;

	info.artist = xspfTrack->getCreator();
	info.album  = xspfTrack->getAlbum();
	info.title  = xspfTrack->getTitle();
	info.track  = xspfTrack->getTrackNum();

	track.SetInfo(info);

	track.origFilename = playlist->DecodeURI(String(xspfTrack->getLocation(0)).Replace("file://", NIL));

	playlist->trackList.Add(track);

	String::SetInputFormat(inFormat);

	delete xspfTrack;
}
