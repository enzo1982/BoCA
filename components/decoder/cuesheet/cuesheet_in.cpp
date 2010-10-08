 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "cuesheet_in.h"

using namespace smooth::IO;

using namespace BoCA::AS;

const String &BoCA::CueSheetIn::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Cue Sheet Reader</name>		\
	    <version>1.0</version>			\
	    <id>cuesheet-in</id>			\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Cue Sheets</name>			\
	      <extension>cue</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::CueSheetIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".cue");
}

Error BoCA::CueSheetIn::GetStreamInfo(const String &streamURI, Track &track)
{
	Track		 iTrack;
	Format		 format = track.GetFormat();
	Info		 info;

	Bool		 trackMode = False;
	Bool		 dataMode = False;

	Int		 discLength = 0;

	/* Standard format for audio discs.
	 */
	format.bits	= 16;
	format.channels	= 2;
	format.rate	= 44100;
	format.order	= BYTE_INTEL;

	track.SetFormat(format);

	/* Open cue sheet.
	 */
	InStream	*file	= new InStream(STREAM_FILE, streamURI, IS_READ);

	/* Look for UTF-8 BOM and set input format.
	 */
	const char	*prevInputFormat = String::SetInputFormat("ISO-8859-1");

	Int		 bom[3] = { file->InputNumber(1), file->InputNumber(1), file->InputNumber(1) };

	if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) String::SetInputFormat("UTF-8");
	else							file->Seek(0);

	/* Actual parsing action.
	 */
	while (file->GetPos() < file->Size())
	{
		String	 line = file->InputLine().Trim();

		/* Parse metadata comments.
		 */
		if (line.StartsWith("REM GENRE "))
		{
			if (line.Find("\"") >= 0) info.genre = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			  info.genre = line.Tail(line.Length() - line.FindLast(" ") - 1);
		}

		if (line.StartsWith("REM DATE ")) info.year = line.Tail(line.Length() - line.FindLast(" ") - 1).ToInt();

		if (line.StartsWith("REM COMMENT "))
		{
			if (line.Find("\"") >= 0) info.comment = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			  info.comment = line.Tail(line.Length() - line.FindLast(" ") - 1);
		}

		/* Parse Replay Gain comments.
		 */
		if	(line.StartsWith("REM REPLAYGAIN_ALBUM_GAIN ")) info.album_gain = line.Tail(line.Length() - 26);
		else if (line.StartsWith("REM REPLAYGAIN_ALBUM_PEAK ")) info.album_peak = line.Tail(line.Length() - 26);
		else if (line.StartsWith("REM REPLAYGAIN_TRACK_GAIN ")) info.track_gain = line.Tail(line.Length() - 26);
		else if (line.StartsWith("REM REPLAYGAIN_TRACK_PEAK ")) info.track_peak = line.Tail(line.Length() - 26);

		/* Skip other comments.
		 */
		if (line.StartsWith("REM ")) continue;

		/* File reference.
		 */
		if (line.StartsWith("FILE "))
		{
			if (trackMode && (iTrack.length >= 0 || iTrack.approxLength >= 0))
			{
				/* Add previous track.
				 */
				iTrack.SetFormat(format);
				iTrack.SetInfo(info);

				AddTrack(iTrack, track.tracks);

				iTrack.length = -1;
				iTrack.approxLength = -1;

				info.track_gain = NIL;
				info.track_peak = NIL;

				trackMode = False;
			}

			if (line.Find("\"") >= 0) iTrack.origFilename = String(File(streamURI).GetFilePath()).Append(Directory::GetDirectoryDelimiter()).Append(line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1));
			else			  iTrack.origFilename = String(File(streamURI).GetFilePath()).Append(Directory::GetDirectoryDelimiter()).Append(line.SubString(line.Find(" ") + 1, line.FindLast(" ") - line.Find(" ") - 1));

			if (!File(iTrack.origFilename).Exists())
			{
				errorState  = True;
				errorString = "File not found";

				break;
			}

			AS::DecoderComponent	*decoder = AS::Registry::Get().CreateDecoderForStream(iTrack.origFilename);

			if (decoder == NIL)
			{
				errorState  = True;
				errorString = "Unknown file type";

				break;
			}

			Track	 infoTrack;

			errorState  = decoder->GetStreamInfo(iTrack.origFilename, infoTrack);
			errorString = decoder->GetErrorString();

			AS::Registry::Get().DeleteComponent(decoder);

			if (errorState) break;

			format = infoTrack.GetFormat();

			if	(infoTrack.length	>= 0) { discLength += infoTrack.length;	      iTrack.length	  = infoTrack.length;	    }
			else if (infoTrack.approxLength >= 0) { discLength += infoTrack.approxLength; iTrack.approxLength = infoTrack.approxLength; }
		}

		/* Regular metadata...
		 */
		if (line.StartsWith("PERFORMER "))
		{
			if (line.Find("\"") >= 0) info.artist = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			  info.artist = line.Tail(line.Length() - line.FindLast(" ") - 1);
		}

		if (line.StartsWith("SONGWRITER "))
		{
			String	 songwriter;

			if (line.Find("\"") >= 0) songwriter = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			  songwriter = line.Tail(line.Length() - line.FindLast(" ") - 1);

			for (Int i = 0; i < info.other.Length(); i++)
			{
				if (info.other.GetNth(i).StartsWith(String(INFO_COMPOSER).Append(":"))) info.other.RemoveNth(i);
			}

			info.other.Add(String(INFO_COMPOSER).Append(":").Append(songwriter));
		}

		if (line.StartsWith("TITLE "))
		{
			String	 title;

			if (line.Find("\"") >= 0) title = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			  title = line.Tail(line.Length() - line.FindLast(" ") - 1);

			if (!trackMode && !dataMode) info.album = title;
			else			     info.title = title;
		}

		if (line.StartsWith("ISRC ")) info.isrc = line.Tail(line.Length() - line.FindLast(" ") - 1);

		/* Track sample offset.
		 */
		if (line.StartsWith("INDEX 01 "))
		{
			String	 msf = line.Tail(line.Length() - line.FindLast(" ") - 1);

			Int	 samplePos = msf.Head(2).ToInt() * 60 * format.rate * format.channels +
					     msf.SubString(3, 2).ToInt() * format.rate * format.channels +
					     msf.SubString(6, 2).ToInt() * format.rate * format.channels / 75;

			iTrack.sampleOffset = samplePos;
		}

		/* End of a track.
		 */
		if ((line.StartsWith("TRACK ") || file->GetPos() == file->Size()) && trackMode)
		{
			if ((iTrack.length == -1 && iTrack.approxLength == -1) || iTrack.length == discLength || iTrack.approxLength == discLength)
			{
				/* Get previous track length.
				 */
				if (file->GetPos() == file->Size()) iTrack.length = discLength - iTrack.sampleOffset;

				Int	 filePos = file->GetPos();

				while (file->GetPos() < file->Size())
				{
					String	 line = file->InputLine().Trim();

					if (line.StartsWith("FILE ")) break;

					if (line.StartsWith("INDEX 01 "))
					{
						String	 msf = line.Tail(line.Length() - line.FindLast(" ") - 1);

						Int	 samplePos = msf.Head(2).ToInt() * 60 * format.rate * format.channels +
								     msf.SubString(3, 2).ToInt() * format.rate * format.channels +
								     msf.SubString(6, 2).ToInt() * format.rate * format.channels / 75;

						iTrack.length = samplePos - iTrack.sampleOffset;

						break;
					}
				}

				file->Seek(filePos);
			}

			/* Add previous track.
			 */
			iTrack.SetFormat(format);
			iTrack.SetInfo(info);

			AddTrack(iTrack, track.tracks);

			iTrack.length = -1;
			iTrack.approxLength = -1;

			info.track_gain = NIL;
			info.track_peak = NIL;
		}

		/* Start of a track.
		 */
		if (line.StartsWith("TRACK "))
		{
			trackMode =  line.EndsWith(" AUDIO");
			dataMode  = !line.EndsWith(" AUDIO");

			info.track = line.SubString(line.Find(" ") + 1, line.FindLast(" ") - line.Find(" ") - 1).ToInt();
		}
	}

	file->Close();

	delete file;

	/* Restore previous input format.
	 */
	String::SetInputFormat(prevInputFormat);

	/* Return on error.
	 */
	if (errorState) return Error();

	/* Generate offset string.
	 */
	Int	 offset	 = 150;

	if (track.tracks.Length() > 0)
	{
		const Track	&iTrack = track.tracks.GetNthReference(0);
		const Format	&format = iTrack.GetFormat();

		if (iTrack.sampleOffset >= 0) offset += iTrack.sampleOffset / format.channels / (format.rate / 75);
	}

	String	 offsets = Number((Int64) track.tracks.Length()).ToHexString();

	for (Int i = 0; i < track.tracks.Length(); i++)
	{
		offsets.Append("+").Append(Number((Int64) offset).ToHexString());

		const Track	&iTrack = track.tracks.GetNthReference(i);
		const Format	&format = iTrack.GetFormat();

		if	(iTrack.length	     >= 0) offset += iTrack.length / format.channels / (format.rate / 75);
		else if (iTrack.approxLength >= 0) offset += iTrack.approxLength / format.channels / (format.rate / 75);
		else				   { offsets = NIL; break; }
	}

	if (offsets != NIL) offsets = offsets.Append("+").Append(Number((Int64) offset).ToHexString()).ToUpper();

	/* Add offset string to all track.
	 */
	for (Int i = 0; i < track.tracks.Length(); i++)
	{
		Track	&iTrack = track.tracks.GetNthReference(i);

		if (offsets != NIL)
		{
			Info	 info = iTrack.GetInfo();

			info.offsets = offsets;

			iTrack.SetInfo(info);
		}
	}

	return Success();
}

Bool BoCA::CueSheetIn::AddTrack(const Track &track, Array<Track> &tracks)
{
	/* Copy track data and add it to list of tracks.
	 */
	Track	 rTrack;

	rTrack.origFilename = track.origFilename;

	rTrack.sampleOffset = track.sampleOffset;

	rTrack.length	    = track.length;
	rTrack.approxLength = track.approxLength;

	rTrack.fileSize	    = track.length * (track.GetFormat().bits / 8);

	rTrack.SetFormat(track.GetFormat());
	rTrack.SetInfo(track.GetInfo());

	tracks.Add(rTrack);

	return True;
}

BoCA::CueSheetIn::CueSheetIn()
{
}

BoCA::CueSheetIn::~CueSheetIn()
{
}

Bool BoCA::CueSheetIn::Activate()
{
	return False;
}

Bool BoCA::CueSheetIn::Deactivate()
{
	return False;
}

Int BoCA::CueSheetIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	return -1;
}
