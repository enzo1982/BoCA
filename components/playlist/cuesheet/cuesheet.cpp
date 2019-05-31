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

#include "cuesheet.h"

using namespace smooth::IO;

const String &BoCA::PlaylistCueSheet::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Cue Sheet Playlist</name>		\
	    <version>1.0</version>			\
	    <id>cuesheet-playlist</id>			\
	    <type>playlist</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::PlaylistCueSheet::PlaylistCueSheet()
{
}

BoCA::PlaylistCueSheet::~PlaylistCueSheet()
{
}

Error BoCA::PlaylistCueSheet::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	const Config	*config = GetConfiguration();
	I18n		*i18n	= I18n::Get();

	/* Create cue sheet file.
	 */
	String		 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream	 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create cue sheet file:\n\n%1", actualFile);

		return Error();
	}

	/* Write UTF-8 BOM and set output format.
	 */
	out.OutputNumber(0xEF, 1);
	out.OutputNumber(0xBB, 1);
	out.OutputNumber(0xBF, 1);

	String::OutputFormat	 outputFormat("UTF-8");

	/* Check if all tracks belong to the same album and
	 * if we need to create a single or multi file cue sheet.
	 */
	Bool		 artistConsistent    = True;
	Bool		 albumConsistent     = True;

	Bool		 albumGainConsistent = True;

	Bool		 oneFile	     = True;

	for (Int c = 0; c < trackList.Length() - 1; c++)
	{
		const Track	&track	= trackList.GetNth(c);
		const Track	&track1	= trackList.GetNth(c + 1);

		const Info	&info	= track.GetInfo();
		const Info	&info1	= track1.GetInfo();

		if (info.artist	!= info1.artist) artistConsistent = False;
		if (info.album	!= info1.album)	 albumConsistent  = False;

		if (info.album_gain != info1.album_gain ||
		    info.album_peak != info1.album_peak) albumGainConsistent = False;

		if (track.fileName != track1.fileName) oneFile = False;
	}

	/* Metadata.
	 */
	{
		const Info	&info = trackList.GetNth(0).GetInfo();

		/* Output per album metadata.
		 */
		if (artistConsistent && albumConsistent)
		{
			if (info.genre  != NIL) out.OutputLine(String("REM GENRE \"").Append(info.genre).Append("\""));
			if (info.year    >   0) out.OutputLine(String("REM DATE ").Append(String::FromInt(info.year)));
		}

		if (config->GetStringValue("Tags", "DefaultComment", NIL) != NIL) out.OutputLine(String("REM COMMENT \"").Append(config->GetStringValue("Tags", "DefaultComment", NIL)).Append("\""));

		if (artistConsistent) out.OutputLine(String("PERFORMER \"").Append(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")).Append("\""));
		if (albumConsistent)  out.OutputLine(String("TITLE \"").Append(info.album.Length() > 0 ? info.album : i18n->TranslateString("unknown album")).Append("\""));

		/* Save Replay Gain info.
		 */
		if (albumGainConsistent && config->GetIntValue("Tags", "PreserveReplayGain", True))
		{
			if (info.album_gain != NIL && info.album_peak != NIL)
			{
				out.OutputLine(String("REM REPLAYGAIN_ALBUM_GAIN ").Append(info.album_gain));
				out.OutputLine(String("REM REPLAYGAIN_ALBUM_PEAK ").Append(info.album_peak));
			}
		}
	}

	/* Write actual track data.
	 */
	for (Int i = 0; i < trackList.Length(); i++)
	{
		const Track	&track = trackList.GetNth(i);
		const Info	&info  = track.GetInfo();

		if (!oneFile || i == 0) out.OutputLine(String("FILE \"").Append(Utilities::GetRelativeFileName(track.fileName, actualFile)).Append("\" ").Append(GetFileType(track.fileName)));

		out.OutputLine(String("  TRACK ").Append(i < 9 ? "0" : NIL).Append(String::FromInt(i + 1)).Append(" AUDIO"));
		out.OutputLine(String("    TITLE \"").Append(info.title.Length() > 0 ? info.title : i18n->TranslateString("unknown title")).Append("\""));

		if (!artistConsistent) out.OutputLine(String("    PERFORMER \"").Append(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")).Append("\""));

		if (info.isrc != NIL) out.OutputLine(String("    ISRC ").Append(info.isrc));

		/* Save Replay Gain info.
		 */
		if (config->GetIntValue("Tags", "PreserveReplayGain", True))
		{
			if (info.track_gain != NIL && info.track_peak != NIL)
			{
				out.OutputLine(String("    REM REPLAYGAIN_TRACK_GAIN ").Append(info.track_gain));
				out.OutputLine(String("    REM REPLAYGAIN_TRACK_PEAK ").Append(info.track_peak));
			}
		}

		/* Save index.
		 */
		Int	 minutes =  track.sampleOffset					       / (75 * 60);
		Int	 seconds = (track.sampleOffset			- (minutes * 60 * 75)) /  75      ;
		Int	 frames  =  track.sampleOffset - (seconds * 75) - (minutes * 60 * 75)		  ;

		out.OutputLine(String("    INDEX 01 ").Append(minutes < 10 ? "0" : NIL).Append(String::FromInt(minutes)).Append(":")
						      .Append(seconds < 10 ? "0" : NIL).Append(String::FromInt(seconds)).Append(":")
						      .Append(frames  < 10 ? "0" : NIL).Append(String::FromInt(frames )));
	}

	out.Close();

	return Success();
}

String BoCA::PlaylistCueSheet::GetFileType(const String &fileName)
{
	String	 fileType = "WAVE";

	if	(fileName.ToLower().EndsWith(".mp3"))  fileType = "MP3";
	else if	(fileName.ToLower().EndsWith(".aiff")) fileType = "AIFF";

	return fileType;
}
