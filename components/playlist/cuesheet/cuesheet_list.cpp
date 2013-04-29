 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "cuesheet_list.h"

using namespace smooth::IO;

const String &BoCA::CuesheetPlaylist::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Cuesheet Playlist</name>		\
	    <version>1.0</version>			\
	    <id>cuesheet-playlist</id>			\
	    <type>playlist</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::CuesheetPlaylist::CuesheetPlaylist()
{
}

BoCA::CuesheetPlaylist::~CuesheetPlaylist()
{
}

Error BoCA::CuesheetPlaylist::WritePlaylist(const String &file)
{
	if (trackList.Length() == 0) return Error();

	BoCA::Config	*config = BoCA::Config::Get();
	BoCA::I18n	*i18n	= BoCA::I18n::Get();

	/* Create cue sheet file.
	 */
	String		 actualFile = Utilities::CreateDirectoryForFile(file);
	OutStream	 out(STREAM_FILE, actualFile, OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK)
	{
		Utilities::ErrorMessage("Could not create cuesheet file:\n\n%1", actualFile);

		return Error();
	}

	/* Write UTF-8 BOM and set output format.
	 */
	out.OutputNumber(0xEF, 1);
	out.OutputNumber(0xBB, 1);
	out.OutputNumber(0xBF, 1);

	String		 format = String::SetOutputFormat("UTF-8");

	/* Check if all tracks belong to the same album and
	 * if we need to create a single or multi file cue sheet.
	 */
	Bool		 artistConsistent    = True;
	Bool		 albumConsistent     = True;

	Bool		 albumGainConsistent = True;

	Bool		 oneFile	     = True;

	for (Int c = 0; c < trackList.Length() - 1; c++)
	{
		const Track	&track	= trackList.GetNthReference(c);
		const Track	&track1	= trackList.GetNthReference(c + 1);

		const Info	&info	= track.GetInfo();
		const Info	&info1	= track1.GetInfo();

		if (info.artist	!= info1.artist) artistConsistent = False;
		if (info.album	!= info1.album)	 albumConsistent  = False;

		if (info.album_gain != info1.album_gain ||
		    info.album_peak != info1.album_peak) albumGainConsistent = False;

		if (track.origFilename != track1.origFilename) oneFile = False;
	}

	/* Metadata.
	 */
	{
		const Info	&info = trackList.GetNthReference(0).GetInfo();

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
		const Track	&track = trackList.GetNthReference(i);
		const Info	&info  = track.GetInfo();

		if (!oneFile || i == 0) out.OutputLine(String("FILE \"").Append(Utilities::GetRelativeFileName(track.origFilename, actualFile)).Append("\" ").Append(GetFileType(track.origFilename)));

		out.OutputLine(String("  TRACK ").Append(i < 9 ? "0" : NIL).Append(String::FromInt(i + 1)).Append(" AUDIO"));
		out.OutputLine(String("    TITLE \"").Append(info.title.Length() > 0 ? info.title : i18n->TranslateString("unknown title")).Append("\""));
		out.OutputLine(String("    PERFORMER \"").Append(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")).Append("\""));

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

	/* Restore previous output format.
	 */
	String::SetOutputFormat(format);

	return Success();
}

String BoCA::CuesheetPlaylist::GetFileType(const String &fileName)
{
	String	 fileType = "WAVE";

	if	(fileName.ToLower().EndsWith(".mp3"))  fileType = "MP3";
	else if	(fileName.ToLower().EndsWith(".aiff")) fileType = "AIFF";

	return fileType;
}