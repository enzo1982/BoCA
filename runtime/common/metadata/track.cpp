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

#include <boca/common/metadata/track.h>
#include <boca/common/config.h>
#include <boca/common/i18n.h>
#include <boca/common/utilities.h>

Int BoCA::Track::nextTrackID = 0;

BoCA::Track::Track(const int nil)
{
	trackID		= Threads::Access::Increment(nextTrackID);

	sampleOffset	= 0;

	length		= -1;
	approxLength	= -1;

	fileSize	= -1;

	isCDTrack	= False;

	drive		= -1;
	cdTrack		= -1;

	discid		= 0;

	lossless	= False;
}

BoCA::Track::Track(const Track &oTrack)
{
	*this = oTrack;
}

BoCA::Track::~Track()
{
}

BoCA::Track &BoCA::Track::operator =(const int nil)
{
	trackID		= -1;

	format		= Format();

	info		= Info();
	originalInfo	= Info();

	pictures.RemoveAll();
	tracks.RemoveAll();

	return *this;
}

BoCA::Track &BoCA::Track::operator =(const Track &oTrack)
{
	if (&oTrack == this) return *this;

	trackID		= oTrack.trackID;

	format		= oTrack.format;

	info		= oTrack.info;
	originalInfo	= oTrack.originalInfo;

	sampleOffset	= oTrack.sampleOffset;

	length		= oTrack.length;
	approxLength	= oTrack.approxLength;

	fileSize	= oTrack.fileSize;

	isCDTrack	= oTrack.isCDTrack;
	drive		= oTrack.drive;
	cdTrack		= oTrack.cdTrack;

	pictures.RemoveAll();

	foreach (const Picture &picture, oTrack.pictures) pictures.Add(picture);

	tracks.RemoveAll();

	foreach (const Track &track, oTrack.tracks) tracks.Add(track);

	discid		= oTrack.discid;

	outputFile	= oTrack.outputFile;
	fileName	= oTrack.fileName;

	decoderID	= oTrack.decoderID;

	lossless	= oTrack.lossless;
	md5		= oTrack.md5;

	return *this;
}

Bool BoCA::Track::operator ==(const int nil) const
{
	if (format == Format())	return True;
	else			return False;
}

Bool BoCA::Track::operator !=(const int nil) const
{
	if (format == Format())	return False;
	else			return True;
}

String BoCA::Track::GetLengthString() const
{
	Int	 seconds = 0;
	String	 secondsString;

	if	(length	      >= 0) seconds = Math::Round(Float(length)	      / format.rate);
	else if (approxLength >= 0) seconds = Math::Round(Float(approxLength) / format.rate);

	secondsString = String(seconds >= 3600 ? String(seconds / 3600	    < 10 ? "0" : NIL).Append(String::FromInt(seconds / 3600	)).Append(":") : String())
						.Append(seconds % 3600 / 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 / 60)).Append(":")
						.Append(seconds % 3600 % 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 % 60));

	String		 lengthString;
	static wchar_t	 sign[2] = { 0x2248, 0 };

	if	(length >= 0)	    lengthString = secondsString;
	else if (approxLength >= 0) lengthString = String(sign).Append(" ").Append(secondsString);
	else			    lengthString = "?";

	return lengthString;
}

String BoCA::Track::GetFileSizeString() const
{
	if (fileSize > 0) return S::I18n::Number::GetLocalizedNumberString(fileSize);
	else		  return "?";
}

Void BoCA::Track::AdjustSampleCounts(const Format &newFormat)
{
	if (sampleOffset > 0) sampleOffset = sampleOffset * newFormat.rate / format.rate;

	if (length       > 0) length       = length	  * newFormat.rate / format.rate;
	if (approxLength > 0) approxLength = approxLength * newFormat.rate / format.rate;

	foreach (Track &subTrack, tracks) subTrack.AdjustSampleCounts(newFormat);
}

Bool BoCA::Track::LoadCoverArtFiles()
{
	if (isCDTrack) return False;

	Config	*config = Config::Get();

	if (config->GetIntValue("Tags", "CoverArtReadFromFiles", True))
	{
		Directory		 directory = File(fileName).GetFilePath();
		const Array<File>	&jpgFiles = directory.GetFilesByPattern("*.jpg");

		foreach (const File &file, jpgFiles) LoadCoverArtFile(file);

		const Array<File>	&jpegFiles = directory.GetFilesByPattern("*.jpeg");

		foreach (const File &file, jpegFiles) LoadCoverArtFile(file);

		const Array<File>	&pngFiles = directory.GetFilesByPattern("*.png");

		foreach (const File &file, pngFiles) LoadCoverArtFile(file);
	}

	return True;
}

Bool BoCA::Track::LoadCoverArtFile(const String &file)
{
	/* Check if file size is within limits.
	 */
	Config	*config	     = Config::Get();
	Int	 maxFileSize = config->GetIntValue("Tags", "CoverArtMaxFileSize", 100) * 1024;

	if (maxFileSize > 0 && File(file).GetFileSize() > maxFileSize) return False;

	/* Load picture file.
	 */
	Picture	 nPicture;

	nPicture.LoadFromFile(file);

	/* Check if the cover art is already in our list.
	 */
	foreach (const Picture &picture, pictures)
	{
		if (picture.data == nPicture.data) return True;
	}

	if	(file.Contains("front")) nPicture.type = 0x03; // Cover (front)
	else if (file.Contains("back"))	 nPicture.type = 0x04; // Cover (back)
	else if (file.Contains("disc"))	 nPicture.type = 0x06; // Media

	if (nPicture.type == 0x03) pictures.InsertAtPos(0, nPicture);
	else			   pictures.Add(nPicture);

	return True;
}

Bool BoCA::Track::SaveCoverArtFiles(const String &folder)
{
	Config	*config = Config::Get();
	I18n	*i18n	= I18n::Get();

	if (!config->GetIntValue("Tags", "CoverArtWriteToFiles", False)) return True;

	Int	 lastBs		 = Math::Max(fileName.FindLast("\\"), fileName.FindLast("/"));
	String	 inFileDirectory = fileName;

	inFileDirectory[lastBs + 1] = 0;

	foreach (const Picture &picture, pictures)
	{
		static const char	*type = "<type>";

		String	 fileName = config->GetStringValue("Tags", "CoverArtFilenamePattern", "<albumartist> - <album>\\<type>");

		if (fileName.Trim() == NIL) fileName = "<albumartist> - <album>\\<type>";

		switch (picture.type)
		{
			case  0: fileName.Replace(type, "other");	  break;
			case  1: fileName.Replace(type, "icon");	  break;
			case  2: fileName.Replace(type, "othericon");	  break;
			case  3: fileName.Replace(type, "front");	  break;
			case  4: fileName.Replace(type, "back");	  break;
			case  5: fileName.Replace(type, "leaflet");	  break;
			case  6: fileName.Replace(type, "disc");	  break;
			case  7: fileName.Replace(type, "leadartist");	  break;
			case  8: fileName.Replace(type, "artist");	  break;
			case  9: fileName.Replace(type, "conductor");	  break;
			case 10: fileName.Replace(type, "band");	  break;
			case 11: fileName.Replace(type, "composer");	  break;
			case 12: fileName.Replace(type, "writer");	  break;
			case 13: fileName.Replace(type, "location");	  break;
			case 14: fileName.Replace(type, "recording");	  break;
			case 15: fileName.Replace(type, "performing");	  break;
			case 16: fileName.Replace(type, "video");	  break;
			case 17: fileName.Replace(type, "fish");	  break;
			case 18: fileName.Replace(type, "illustration");  break;
			case 19: fileName.Replace(type, "artistlogo");	  break;
			case 20: fileName.Replace(type, "publisherlogo"); break;
			default: fileName.Replace(type, "unknown");	  break;
		}

		/* Replace standard fields.
		 */
		DateTime	 currentDateTime  = DateTime::Current();
		String		 currentDate	  = String().FillN('0', 3 - Math::Floor(Math::Log10(				      currentDateTime.GetYear()	     ))) .Append(String::FromInt(currentDateTime.GetYear()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(				      currentDateTime.GetMonth()     )))).Append(String::FromInt(currentDateTime.GetMonth()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(				      currentDateTime.GetDay()	     )))).Append(String::FromInt(currentDateTime.GetDay()));
		String		 currentTime	  = String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetHour()	> 0 ? currentDateTime.GetHour()	  : 1))) .Append(String::FromInt(currentDateTime.GetHour()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetMinute()	> 0 ? currentDateTime.GetMinute() : 1)))).Append(String::FromInt(currentDateTime.GetMinute()));

		fileName.Replace("<artist>", Utilities::ReplaceIncompatibleCharacters(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")));
		fileName.Replace("<album>", Utilities::ReplaceIncompatibleCharacters(info.album.Length() > 0 ? info.album : i18n->TranslateString("unknown album")));
		fileName.Replace("<genre>", Utilities::ReplaceIncompatibleCharacters(info.genre.Length() > 0 ? info.genre : i18n->TranslateString("unknown genre")));
		fileName.Replace("<year>", Utilities::ReplaceIncompatibleCharacters(info.year > 0 ? String::FromInt(info.year) : i18n->TranslateString("unknown year")));
		fileName.Replace("<discid>", Number((Int64) discid).ToHexString(8));
		fileName.Replace("<currentdate>", currentDate);
		fileName.Replace("<currenttime>", currentTime);

		/* Replace <disc> pattern.
		 */
		fileName.Replace("<disc>", String::FromInt(info.disc < 0 ? 0 : info.disc));

		for (Int i = 1; i <= 4; i++)
		{
			String	 pattern = String("<disc(").Append(String::FromInt(i)).Append(")>");

			fileName.Replace(pattern, String().FillN('0', i - ((Int) Math::Log10(info.disc > 0 ? info.disc : 1) + 1)).Append(String::FromInt(info.disc < 0 ? 0 : info.disc)));
		}

		/* Replace other text fields.
		 */
		foreach (const String &pair, info.other)
		{
			String	 key   = pair.Head(pair.Find(":"));
			String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

			if (value == NIL) continue;

			if	(key == INFO_ALBUMARTIST) fileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(value));
			else if	(key == INFO_CONDUCTOR)	  fileName.Replace("<conductor>", Utilities::ReplaceIncompatibleCharacters(value));
			else if	(key == INFO_COMPOSER)	  fileName.Replace("<composer>", Utilities::ReplaceIncompatibleCharacters(value));
		}

		if (info.artist.Length() > 0) fileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(info.artist));

		fileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown album artist")));
		fileName.Replace("<conductor>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown conductor")));
		fileName.Replace("<composer>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown composer")));

		/* Replace <directory> pattern.
		 */
		String	 directory = inFileDirectory;

		if	(directory[1] == ':')	       directory = directory.Tail(directory.Length() - 3);
		else if (directory.StartsWith("\\\\")) directory = directory.Tail(directory.Length() - 2);

		fileName.Replace("<directory>", directory);

		for (Int i = 0; i < 10 && fileName.Contains("<directory+"); i++)
		{
			String	 pattern = String("<directory+").Append(String::FromInt(i)).Append(">");

			if (fileName.Contains(pattern))
			{
				String	 value = directory;

				for (Int n = 0; n < i; n++) value = value.Tail(value.Length() - value.Find(Directory::GetDirectoryDelimiter()) - 1);

				fileName.Replace(pattern, value);
			}

			for (Int j = 0; j < 10 && fileName.Contains(String("<directory+").Append(String::FromInt(i)).Append("(")); j++)
			{
				String	 pattern = String("<directory+").Append(String::FromInt(i)).Append("(").Append(String::FromInt(j + 1)).Append(")>");

				if (!fileName.Contains(pattern)) continue;

				String	 value = directory;

				for (Int n = 0; n < i; n++) value = value.Tail(value.Length() - value.Find(Directory::GetDirectoryDelimiter()) - 1);

				Int	 bsCount = 0;

				for (Int n = 0; n < value.Length(); n++)
				{
					if (value[n] == '\\' || value[n] == '/') bsCount++;

					if (bsCount == j + 1)
					{
						value[n] = 0;

						break;
					}
				}

				fileName.Replace(pattern, value);
			}
		}

		/* Save cover art file.
		 */
		String	 file = String(folder).Append(folder.EndsWith(Directory::GetDirectoryDelimiter()) ? NIL : Directory::GetDirectoryDelimiter()).Append(fileName);

		picture.SaveToFile(Utilities::CreateDirectoryForFile(Utilities::NormalizeFileName(file)));
	}

	return True;
}
