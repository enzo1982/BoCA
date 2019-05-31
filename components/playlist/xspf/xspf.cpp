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

	reader.parseMemory(memory, numBytes, &callback, String("file://").Append(Encoding::URLEncode::Encode(file.Replace("\\", "/")).Replace("%2F", "/").Replace("%3A", ":")));

	delete [] memory;

	/* Complete relative filenames.
	 */
	for (Int i = 0; i < trackList.Length(); i++)
	{
		Track	&track = trackList.GetNthReference(i);

		/* Get file name.
		 */
		String	&fileName = track.fileName;

		/* Handle relative paths.
		 */
#ifdef __WIN32__
		if (fileName[1] != ':' && !fileName.StartsWith("\\\\") && !fileName.Contains("://"))
#else
		if (!fileName.StartsWith(Directory::GetDirectoryDelimiter()) && !fileName.StartsWith("~") && !fileName.Contains("://"))
#endif
		{
			fileName = File(file).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(fileName);
		}
	}

	in.Close();

	return trackList;
}

Error BoCA::PlaylistXSPF::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	String::OutputFormat	 outputFormat("UTF-8");

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
		String	 fileName = Utilities::GetRelativeFileName(Utilities::GetCDTrackFileName(track), actualFile);
			 fileName = Encoding::URLEncode::Encode(fileName.Replace("\\", "/")).Replace("%2F", "/").Replace("%3A", ":");

		/* Handle absolute paths.
		 */
#ifdef __WIN32__
		if (fileName[1] == ':') fileName = String("/").Append(fileName);
#endif

		if (fileName[0] == '/') fileName = String("file://").Append(fileName);

		/* Add info to XSPF.
		 */
		const Info	&info = track.GetInfo();

		XspfTrack	 xspfTrack;

		if (info.artist	 != NIL) xspfTrack.lendCreator(info.artist);
		if (info.album	 != NIL) xspfTrack.lendAlbum(info.album);
		if (info.title	 != NIL) xspfTrack.lendTitle(info.title);
		if (info.track   >    0) xspfTrack.setTrackNum(info.track);
		if (track.length >=   0) xspfTrack.setDuration(Math::Round((Float) track.length / track.GetFormat().rate * 1000.0));

		xspfTrack.lendAppendLocation(fileName);

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

	return Success();
}

BoCA::PlaylistXSPFCallback::PlaylistXSPFCallback(PlaylistXSPF *playlist)
{
	this->playlist = playlist;
}

void BoCA::PlaylistXSPFCallback::addTrack(XspfTrack *xspfTrack)
{
	String::InputFormat	 inputFormat("UTF-8");

	/* Get title info.
	 */
	Track	 track;
	Info	 info;

	info.artist = xspfTrack->getCreator();
	info.album  = xspfTrack->getAlbum();
	info.title  = xspfTrack->getTitle();
	info.track  = xspfTrack->getTrackNum();

	track.SetInfo(info);

	/* Decode file name.
	 */
	String	 fileName = Encoding::URLEncode::Decode(String(xspfTrack->getLocation(0)).Replace("file://", NIL)).Replace("/", Directory::GetDirectoryDelimiter());

#ifdef __WIN32__
	if (fileName.StartsWith(Directory::GetDirectoryDelimiter()) && fileName[2] == ':') fileName = fileName.Tail(fileName.Length() - 1);
#endif

	track.fileName = fileName;

	/* Add track to playlist.
	 */
	playlist->trackList.Add(track);

	delete xspfTrack;
}
