 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Bool		 replaceExistingComments = config->GetIntValue("Tags", "ReplaceExistingComments", False);
	String		 defaultComment		 = config->GetStringValue("Tags", "DefaultComment", NIL);

	Bool		 preserveReplayGain	 = config->GetIntValue("Tags", "PreserveReplayGain", True);

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
	out.OutputNumberRaw(0xEFBBBF, 3);

	String::OutputFormat	 outputFormat("UTF-8");

	/* Check if all tracks belong to the same album and
	 * if we need to create a single or multi file cue sheet.
	 */
	Bool		 artistConsistent    = True;
	Bool		 albumConsistent     = True;
	Bool		 commentConsistent   = True;

	Bool		 albumGainConsistent = True;

	for (Int c = 0; c < trackList.Length() - 1; c++)
	{
		const Track	&track	 = trackList.GetNth(c);
		const Track	&track1	 = trackList.GetNth(c + 1);

		const Info	&info	 = track.GetInfo();
		const Info	&info1	 = track1.GetInfo();

		const String	&artist  = info.HasOtherInfo(INFO_ALBUMARTIST)  ? info.GetOtherInfo(INFO_ALBUMARTIST)  : info.artist;
		const String	&artist1 = info1.HasOtherInfo(INFO_ALBUMARTIST) ? info1.GetOtherInfo(INFO_ALBUMARTIST) : info1.artist;

		if (artist	 != artist1)	   artistConsistent  = False;
		if (info.album	 != info1.album)   albumConsistent   = False;
		if (info.comment != info1.comment) commentConsistent = False;

		if (info.album_gain != info1.album_gain ||
		    info.album_peak != info1.album_peak) albumGainConsistent = False;
	}

	/* Metadata.
	 */
	I18n		*i18n	= I18n::Get();

	const Info	&info	= trackList.GetNth(0).GetInfo();
	const String	&artist	= info.HasOtherInfo(INFO_ALBUMARTIST) ? info.GetOtherInfo(INFO_ALBUMARTIST) : info.artist;

	/* Output per album metadata.
	 */
	if (artistConsistent && albumConsistent)
	{
		if (info.genre  != NIL) out.OutputLine(String("REM GENRE \"").Append(EscapeString(info.genre)).Append("\""));
		if (info.year    >   0) out.OutputLine(String("REM DATE ").Append(String::FromInt(info.year)));
	}

	if	(info.comment != NIL && commentConsistent && !replaceExistingComments) out.OutputLine(String("REM COMMENT \"").Append(EscapeString(info.comment)).Append("\""));
	else if (defaultComment != NIL)						       out.OutputLine(String("REM COMMENT \"").Append(EscapeString(defaultComment)).Append("\""));

	if (artistConsistent) out.OutputLine(String("PERFORMER \"").Append(artist.Length() > 0 ? EscapeString(artist)	  : i18n->TranslateString("unknown artist")).Append("\""));
	if (albumConsistent)  out.OutputLine(String("TITLE \"").Append(info.album.Length() > 0 ? EscapeString(info.album) : i18n->TranslateString("unknown album")).Append("\""));

	/* Save Replay Gain info.
	 */
	if (albumGainConsistent && preserveReplayGain)
	{
		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			out.OutputLine(String("REM REPLAYGAIN_ALBUM_GAIN ").Append(info.album_gain));
			out.OutputLine(String("REM REPLAYGAIN_ALBUM_PEAK ").Append(info.album_peak));
		}
	}

	/* Write actual track data.
	 */
	String	 previousFile;

	for (Int i = 0; i < trackList.Length(); i++)
	{
		const Track	&track	= trackList.GetNth(i);
		const Format	&format	= track.GetFormat();
		const Info	&info	= track.GetInfo();

		if (track.fileName != previousFile) out.OutputLine(String("FILE \"").Append(Utilities::GetRelativeFileName(track.fileName, actualFile)).Append("\" ").Append(GetFileType(track.fileName)));

		out.OutputLine(String("  TRACK ").Append(i < 9 ? "0" : NIL).Append(String::FromInt(i + 1)).Append(" AUDIO"));

		if ((track.cdTrack > 0 && info.mcdi.GetNthEntryPreEmphasis(track.cdTrack - 1)) || info.GetOtherInfo(INFO_PRE_EMPHASIS).ToInt()) out.OutputLine("    FLAGS PRE");

		out.OutputLine(String("    TITLE \"").Append(info.title.Length() > 0 ? EscapeString(info.title) : i18n->TranslateString("unknown title")).Append("\""));

		if (!artistConsistent || info.artist != artist)				   out.OutputLine(String("    PERFORMER \"").Append(info.artist.Length() > 0 ? EscapeString(info.artist) : i18n->TranslateString("unknown artist")).Append("\""));
		if (!commentConsistent && info.comment != NIL && !replaceExistingComments) out.OutputLine(String("    REM COMMENT \"").Append(EscapeString(info.comment)).Append("\""));

		if (info.isrc != NIL) out.OutputLine(String("    ISRC ").Append(info.isrc));

		/* Save Replay Gain info.
		 */
		if (preserveReplayGain)
		{
			if (info.track_gain != NIL && info.track_peak != NIL)
			{
				out.OutputLine(String("    REM REPLAYGAIN_TRACK_GAIN ").Append(info.track_gain));
				out.OutputLine(String("    REM REPLAYGAIN_TRACK_PEAK ").Append(info.track_peak));
			}
		}

		/* Save index.
		 */
		Int	 minutes =  track.sampleOffset								      / format.rate / 60;
		Int	 seconds = (track.sampleOffset				 - (minutes * 60 * format.rate))      / format.rate	;
		Int	 frames  = (track.sampleOffset - (seconds * format.rate) - (minutes * 60 * format.rate)) * 75 / format.rate	;

		out.OutputLine(String("    INDEX 01 ").Append(minutes < 10 ? "0" : NIL).Append(String::FromInt(minutes)).Append(":")
						      .Append(seconds < 10 ? "0" : NIL).Append(String::FromInt(seconds)).Append(":")
						      .Append(frames  < 10 ? "0" : NIL).Append(String::FromInt(frames )));

		previousFile = track.fileName;
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

String BoCA::PlaylistCueSheet::EscapeString(const String &string)
{
	const Array<String>	&lines = string.Explode("\n");

	String	 result;

	foreach (const String &line, lines) result.Append(result == NIL ? "" : " // ").Append(line.Trim().Replace("\"", "''"));

	return result;
}
