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
#include "config.h"

using namespace smooth::IO;

using namespace BoCA::AS;

const String &BoCA::DecoderCueSheet::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::Decoders");

	static String	 componentSpecs = String("					\
											\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
	  <component>									\
	    <name>").Append(i18n->TranslateString("Cue Sheet Reader")).Append("</name>	\
	    <version>1.0</version>							\
	    <id>cuesheet-dec</id>							\
	    <type>decoder</type>							\
	    <format>									\
	      <name>Cue Sheets</name>							\
	      <extension>cue</extension>						\
	    </format>									\
	  </component>									\
											\
	");

	return componentSpecs;
}

Bool BoCA::DecoderCueSheet::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".cue");
}

Error BoCA::DecoderCueSheet::GetStreamInfo(const String &streamURI, Track &track)
{
	const Config	*config		     = GetConfiguration();

	Bool		 readInfoTags	     = config->GetIntValue(ConfigureCueSheet::ConfigID, "ReadInformationTags", True);
	Bool		 preferCueSheets     = config->GetIntValue(ConfigureCueSheet::ConfigID, "PreferCueSheets", True);

	Bool		 lookForAlternatives = config->GetIntValue(ConfigureCueSheet::ConfigID, "LookForAlternativeFiles", False);

	Bool		 ignoreErrors	     = config->GetIntValue(ConfigureCueSheet::ConfigID, "IgnoreErrors", False);

	Track		 iTrack;
	Format		 format		     = track.GetFormat();

	Info		 info;
	Array<Picture>	 pictures;

	Track		 albumTrack;
	Info		 albumInfo;

	Bool		 trackMode	     = False;
	Bool		 dataMode	     = False;

	Int		 fileLength	     = 0;

	/* Standard format for audio discs.
	 */
	format.bits	= 16;
	format.channels	= 2;
	format.rate	= 44100;
	format.order	= BYTE_INTEL;

	track.SetFormat(format);

	/* Open cue sheet.
	 */
	InStream		 in(STREAM_FILE, streamURI, IS_READ);

	/* Look for UTF-8 BOM and set input format.
	 */
	String::InputFormat	 inputFormat("ISO-8859-1");

	if (in.InputNumberRaw(3) == 0xEFBBBF) String::SetInputFormat("UTF-8");
	else				      in.Seek(0);

	/* Actual parsing action.
	 */
	while (in.GetPos() < in.Size())
	{
		String	 line = in.InputLine().Trim();

		/* Parse metadata comments.
		 */
		if (line.StartsWith("REM GENRE "))
		{
			String	 genre;

			if (line.Contains("\"")) genre = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 genre = line.Tail(line.Length() - 10);

			if (!readInfoTags || preferCueSheets) info.genre = genre;

			if (!trackMode && !dataMode) albumInfo.genre = genre;
		}

		if (line.StartsWith("REM DATE ")) info.year = line.Tail(line.Length() - 9).ToInt();

		if (line.StartsWith("REM COMMENT "))
		{
			String	 comment;

			if (line.Contains("\"")) comment = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 comment = line.Tail(line.Length() - 12);

			if (!readInfoTags || preferCueSheets) info.comment = comment;

			if (!trackMode && !dataMode) albumInfo.comment = comment;
		}

		/* Parse Replay Gain comments.
		 */
		if	(line.StartsWith("REM REPLAYGAIN_ALBUM_GAIN ")) { info.album_gain = line.Tail(line.Length() - 26); albumInfo.album_gain = info.album_gain; }
		else if (line.StartsWith("REM REPLAYGAIN_ALBUM_PEAK ")) { info.album_peak = line.Tail(line.Length() - 26); albumInfo.album_peak = info.album_peak; }
		else if (line.StartsWith("REM REPLAYGAIN_TRACK_GAIN "))	  info.track_gain = line.Tail(line.Length() - 26);
		else if (line.StartsWith("REM REPLAYGAIN_TRACK_PEAK "))	  info.track_peak = line.Tail(line.Length() - 26);

		/* Skip other comments.
		 */
		if (line.StartsWith("REM ")) continue;

		/* Regular metadata...
		 */
		if (line.StartsWith("PERFORMER "))
		{
			String	 artist;

			if (line.Contains("\"")) artist = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 artist = line.Tail(line.Length() - 10);

			if (!readInfoTags || preferCueSheets)
			{
				info.artist = artist;

				if (albumInfo.artist != NIL &&
				    albumInfo.artist != artist) info.SetOtherInfo(INFO_ALBUMARTIST, albumInfo.artist);
			}

			if (!trackMode && !dataMode) albumInfo.artist = artist;
		}

		if (line.StartsWith("SONGWRITER "))
		{
			String	 songwriter;

			if (line.Contains("\"")) songwriter = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 songwriter = line.Tail(line.Length() - 11);

			if (!readInfoTags || preferCueSheets)
			{
				for (Int i = 0; i < info.other.Length(); i++)
				{
					if (info.other.GetNth(i).StartsWith(String(INFO_COMPOSER).Append(":"))) info.other.RemoveNth(i);
				}

				info.other.Add(String(INFO_COMPOSER).Append(":").Append(songwriter));
			}

			if (!trackMode && !dataMode)
			{
				for (Int i = 0; i < albumInfo.other.Length(); i++)
				{
					if (albumInfo.other.GetNth(i).StartsWith(String(INFO_COMPOSER).Append(":"))) albumInfo.other.RemoveNth(i);
				}

				albumInfo.other.Add(String(INFO_COMPOSER).Append(":").Append(songwriter));
			}
		}

		if (line.StartsWith("TITLE "))
		{
			String	 title;

			if (line.Contains("\"")) title = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 title = line.Tail(line.Length() - 6);

			if (!readInfoTags || preferCueSheets)
			{
				if (!trackMode && !dataMode) info.album = title;
				else			     info.title = title;
			}

			if (!trackMode && !dataMode) albumInfo.album = title;
		}

		if (line.StartsWith("ISRC "))
		{
			/* Check if the ISRC is valid.
			 */
			String	 isrc = line.Tail(line.Length() - 5);

			if (Info::IsISRC(isrc)) info.isrc = isrc;
		}

		/* Track sample offset.
		 */
		if (line.StartsWith("INDEX 01 "))
		{
			String	 msf = line.Tail(line.Length() - line.FindLast(" ") - 1);

			Int	 samplePos = msf.Head(2).ToInt() * 60 * format.rate +
					     msf.SubString(3, 2).ToInt() * format.rate +
					     msf.SubString(6, 2).ToInt() * format.rate / 75;

			iTrack.sampleOffset = samplePos;
		}

		/* End of a track.
		 */
		if ((line.StartsWith("TRACK ") || line.StartsWith("FILE ") || in.GetPos() == in.Size()) && trackMode)
		{
			if ((iTrack.length == -1 && iTrack.approxLength == -1) || iTrack.length == fileLength || iTrack.approxLength == fileLength)
			{
				/* Get previous track length.
				 */
				if (line.StartsWith("FILE ") || in.GetPos() == in.Size()) iTrack.length = fileLength - iTrack.sampleOffset;

				if (line.StartsWith("TRACK "))
				{
					Int	 filePos = in.GetPos();

					while (in.GetPos() < in.Size())
					{
						String	 line = in.InputLine().Trim();

						if (line.StartsWith("FILE ")) break;

						if (line.StartsWith("INDEX 01 "))
						{
							String	 msf = line.Tail(line.Length() - line.FindLast(" ") - 1);

							Int	 samplePos = msf.Head(2).ToInt() * 60 * format.rate +
									     msf.SubString(3, 2).ToInt() * format.rate +
									     msf.SubString(6, 2).ToInt() * format.rate / 75;

							iTrack.length = samplePos - iTrack.sampleOffset;

							break;
						}
					}

					in.Seek(filePos);
				}
			}

			/* Add previous track.
			 */
			iTrack.SetFormat(format);
			iTrack.SetInfo(info);

			iTrack.pictures = pictures;

			AddTrack(iTrack, track.tracks);

			iTrack.length = -1;
			iTrack.approxLength = -1;

			info.track_gain = NIL;
			info.track_peak = NIL;

			trackMode = False;
		}

		/* File reference.
		 */
		if (line.StartsWith("FILE "))
		{
			/* Get referenced file name.
			 */
			String	 fileName;

			if (line.Contains("\"")) fileName = line.SubString(line.Find("\"") + 1, line.FindLast("\"") - line.Find("\"") - 1);
			else			 fileName = line.SubString(line.Find(" ")  + 1, line.FindLast(" ")  - line.Find(" ")  - 1);

			/* Handle relative paths.
			 */
			String	 resolvedFileName = fileName;

			if (Utilities::IsRelativePath(resolvedFileName)) resolvedFileName = File(streamURI).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(resolvedFileName);

			/* If file is not found, try interpreting the file name using the default system encoding.
			 */
			if (!File(resolvedFileName).Exists())
			{
				String::InputFormat	 inputFormat(String::GetDefaultEncoding());
				String			 nativeFileName = fileName.ConvertTo("ISO-8859-1");

				if (Utilities::IsRelativePath(nativeFileName)) nativeFileName = File(streamURI).GetFilePath().Append(Directory::GetDirectoryDelimiter()).Append(nativeFileName);

				if (File(nativeFileName).Exists()) resolvedFileName = nativeFileName;
			}

			iTrack.fileName = File(resolvedFileName);

			/* Look for compressed files in place of referenced Wave, Wave64, RF64 or AIFF files.
			 */
			if (!File(iTrack.fileName).Exists() && lookForAlternatives &&
			    (iTrack.fileName.ToLower().EndsWith(".wav") || iTrack.fileName.ToLower().EndsWith(".w64")  || iTrack.fileName.ToLower().EndsWith(".rf64") ||
			     iTrack.fileName.ToLower().EndsWith(".aif") || iTrack.fileName.ToLower().EndsWith(".aiff") || iTrack.fileName.ToLower().EndsWith(".aifc")))
			{
				const char	*extensions[]  = { "wav", "w64", "rf64", "aif", "aiff", "aifc", "flac", "ape", "ofr", "tak", "tta", "wv", "m4a", "wma", "mp3", "ogg", "oga", "opus", "spx", NIL };
				const String	 fileNameNoExt = iTrack.fileName.SubString(0, iTrack.fileName.FindLast(".") + 1);

				for (Int i = 0; extensions[i] != NIL; i++)
				{
					const String	 fileName = fileNameNoExt.Append(extensions[i]);

					if (File(fileName).Exists())
					{
						iTrack.fileName = fileName;

						break;
					}
				}
			}

			/* Check file existence.
			 */
			if (!File(iTrack.fileName).Exists())
			{
				errorState  = True;
				errorString = "File referenced in cue sheet not found";

				if (ignoreErrors) errorState = False;

				track = NIL;

				break;
			}

			/* Create decoder component.
			 */
			AS::Registry		&boca	 = AS::Registry::Get();
			AS::DecoderComponent	*decoder = boca.CreateDecoderForStream(iTrack.fileName, GetConfiguration());

			if (decoder == NIL)
			{
				errorState  = True;
				errorString = "Unknown file type referenced in cue sheet";

				if (ignoreErrors) errorState = False;

				track = NIL;

				break;
			}

			/* Disable reading chapters.
			 */
			Config	*decoderConfig = Config::Copy(GetConfiguration());

			decoderConfig->SetIntValue("Tags", "ReadChapters", False);

			decoder->SetConfiguration(decoderConfig);

			/* Get stream info.
			 */
			Track	 infoTrack;

			errorState  = decoder->GetStreamInfo(iTrack.fileName, infoTrack);
			errorString = decoder->GetErrorString();

			boca.DeleteComponent(decoder);

			Config::Free(decoderConfig);

			if (errorState)
			{
				if (ignoreErrors) errorState = False;

				track = NIL;

				break;
			}

			format = infoTrack.GetFormat();

			if (readInfoTags)
			{
				info	 = infoTrack.GetInfo();
				pictures = infoTrack.pictures;

				if (preferCueSheets) UpdateInfoWithAlbumInfo(info, albumInfo);
			}
			else
			{
				info	 = Info();
				pictures.RemoveAll();

				UpdateInfoWithAlbumInfo(info, albumInfo);
			}

			if (infoTrack.tracks.Length() > 0) albumTrack.tracks = infoTrack.tracks;

			if	(infoTrack.length	>= 0) { fileLength = infoTrack.length;	     iTrack.length	 = infoTrack.length;	   }
			else if (infoTrack.approxLength >= 0) { fileLength = infoTrack.approxLength; iTrack.approxLength = infoTrack.approxLength; }

			iTrack.decoderID = infoTrack.decoderID;
			iTrack.lossless	 = infoTrack.lossless;
		}

		/* Start of a track.
		 */
		if (line.StartsWith("TRACK "))
		{
			trackMode =  line.EndsWith(" AUDIO");
			dataMode  = !line.EndsWith(" AUDIO");

			Int	 track = line.SubString(line.Find(" ") + 1, line.FindLast(" ") - line.Find(" ") - 1).ToInt();

			if (albumTrack.tracks.Length() >= track)
			{
				if (readInfoTags)
				{
					info	 = albumTrack.tracks.GetNth(track - 1).GetInfo();
					pictures = albumTrack.tracks.GetNth(track - 1).pictures;

					if (preferCueSheets) UpdateInfoWithAlbumInfo(info, albumInfo);
				}
				else
				{
					info	 = Info();
					pictures.RemoveAll();

					UpdateInfoWithAlbumInfo(info, albumInfo);
				}
			}

			info.track = track;
		}
	}

	in.Close();

	/* Return on error.
	 */
	if (errorState) return Error();

	/* Generate offset string.
	 */
	Int	 offset	 = 150;

	if (track.tracks.Length() > 0)
	{
		const Track	&iTrack = track.tracks.GetNth(0);
		const Format	&format = iTrack.GetFormat();

		if (iTrack.sampleOffset >= 0) offset += iTrack.sampleOffset / (format.rate / 75);
	}

	String	 offsets = Number((Int64) track.tracks.Length()).ToHexString();

	for (Int i = 0; i < track.tracks.Length(); i++)
	{
		offsets.Append("+").Append(Number((Int64) offset).ToHexString());

		const Track	&iTrack = track.tracks.GetNth(i);
		const Format	&format = iTrack.GetFormat();

		if	(iTrack.length	     >= 0) offset += iTrack.length / (format.rate / 75);
		else if (iTrack.approxLength >= 0) offset += iTrack.approxLength / (format.rate / 75);
		else				   { offsets = NIL; break; }
	}

	if (offsets != NIL)
	{
		offsets = offsets.Append("+").Append(Number((Int64) offset).ToHexString()).ToUpper();

		/* Add offset string to all tracks.
		 */
		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			Track	&iTrack = track.tracks.GetNthReference(i);
			Info	 info = iTrack.GetInfo();

			info.offsets = offsets;

			iTrack.SetInfo(info);
		}
	}

	/* Set lossless flag if all tracks are lossless.
	 */
	track.lossless = True;

	foreach (const Track &iTrack, track.tracks)
	{
		if (iTrack.lossless) continue;

		track.lossless = False;

		break;
	}

	/* Set number of tracks.
	 */
	for (Int i = 0; i < track.tracks.Length(); i++)
	{
		Track	&iTrack = track.tracks.GetNthReference(i);
		Info	 info = iTrack.GetInfo();

		info.numTracks = track.tracks.Length();

		iTrack.SetInfo(info);
	}

	return Success();
}

Void BoCA::DecoderCueSheet::UpdateInfoWithAlbumInfo(Info &info, const Info &albumInfo) const
{
	if (albumInfo.artist  != NIL) info.artist  = albumInfo.artist;
	if (albumInfo.album   != NIL) info.album   = albumInfo.album;
	if (albumInfo.genre   != NIL) info.genre   = albumInfo.genre;
	if (albumInfo.comment != NIL) info.comment = albumInfo.comment;

	for (Int i = 0; i < albumInfo.other.Length(); i++)
	{
		if (albumInfo.other.GetNth(i).StartsWith(String(INFO_COMPOSER).Append(":")))
		{
			for (Int j = 0; j < info.other.Length(); j++)
			{
				if (info.other.GetNth(j).StartsWith(String(INFO_COMPOSER).Append(":"))) info.other.RemoveNth(j);
			}

			info.other.Add(albumInfo.other.GetNth(i));
		}
	}

	if (albumInfo.album_gain != NIL) info.album_gain = albumInfo.album_gain;
	if (albumInfo.album_peak != NIL) info.album_peak = albumInfo.album_peak;
}

Bool BoCA::DecoderCueSheet::AddTrack(const Track &track, Array<Track> &tracks) const
{
	/* Copy track data and add it to list of tracks.
	 */
	Track	 rTrack;

	rTrack.decoderID    = track.decoderID;
	rTrack.fileName	    = track.fileName;

	rTrack.sampleOffset = track.sampleOffset;

	rTrack.length	    = track.length;
	rTrack.approxLength = track.approxLength;

	rTrack.fileSize	    = track.length * track.GetFormat().channels * (track.GetFormat().bits / 8);

	rTrack.lossless	    = track.lossless;

	rTrack.pictures	    = track.pictures;

	rTrack.SetFormat(track.GetFormat());
	rTrack.SetInfo(track.GetInfo());

	tracks.Add(rTrack);

	return True;
}

BoCA::DecoderCueSheet::DecoderCueSheet()
{
	configLayer = NIL;
}

BoCA::DecoderCueSheet::~DecoderCueSheet()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderCueSheet::Activate()
{
	return False;
}

Bool BoCA::DecoderCueSheet::Deactivate()
{
	return False;
}

Int BoCA::DecoderCueSheet::ReadData(Buffer<UnsignedByte> &data)
{
	return -1;
}

ConfigLayer *BoCA::DecoderCueSheet::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCueSheet();

	return configLayer;
}
