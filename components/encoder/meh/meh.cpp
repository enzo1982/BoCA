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

#include <smooth.h>
#include <smooth/dll.h>

#include "meh.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderMultiEncoderHub::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::Encoders");

	static String	 componentSpecs = String("						\
												\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
	  <component>										\
	    <name>").Append(i18n->TranslateString("meh! - multi encoder hub")).Append("</name>	\
	    <version>1.0</version>								\
	    <id>meh-enc</id>									\
	    <type>encoder</type>								\
	    <format>										\
	      <name>multi encoder hub Output</name>						\
	    </format>										\
	  </component>										\
												\
	");

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	/* Register conversion event handlers.
	 */
	Engine	*engine = Engine::Get();

	engine->onStartConversion.Connect(&EncoderMultiEncoderHub::OnStartConversion);
	engine->onFinishConversion.Connect(&EncoderMultiEncoderHub::OnFinishConversion);
	engine->onCancelConversion.Connect(&EncoderMultiEncoderHub::OnCancelConversion);
}

Void smooth::DetachDLL()
{
	/* Unregister conversion event handlers.
	 */
	Engine	*engine = Engine::Get();

	engine->onStartConversion.Disconnect(&EncoderMultiEncoderHub::OnStartConversion);
	engine->onFinishConversion.Disconnect(&EncoderMultiEncoderHub::OnFinishConversion);
	engine->onCancelConversion.Disconnect(&EncoderMultiEncoderHub::OnCancelConversion);
}

Array<BoCA::ConversionData *>	 BoCA::EncoderMultiEncoderHub::conversionData;

BoCA::EncoderMultiEncoderHub::EncoderMultiEncoderHub()
{
	conversionData.EnableLocking();

	conversionID = -1;

	finished     = False;
	cancelled    = False;

	trackLength  = 0;
	totalLength  = 0;

	configLayer  = NIL;

	Engine	*engine = Engine::Get();

	engine->onFinishTrackConversion.Connect(&EncoderMultiEncoderHub::OnFinishTrackConversion, this);
	engine->onCancelTrackConversion.Connect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);
}

BoCA::EncoderMultiEncoderHub::~EncoderMultiEncoderHub()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);

	Engine	*engine = Engine::Get();

	engine->onFinishTrackConversion.Disconnect(&EncoderMultiEncoderHub::OnFinishTrackConversion, this);
	engine->onCancelTrackConversion.Disconnect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);

	/* Delete output file if it still exists.
	 */
	if (track.outputFile != NIL)
	{
		File(track.outputFile).Delete();

		if (track.outputFile.Contains(Directory::GetDirectoryDelimiter())) track.outputFile[track.outputFile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
	}

	/* Delete empty folders if <filetype> was used in path.
	 */
	while (track.outputFile.Contains("[FILETYPE]"))
	{
		Directory(track.outputFile).Delete();

		if (track.outputFile.Contains(Directory::GetDirectoryDelimiter())) track.outputFile[track.outputFile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
		else								   break;
	}
}

Bool BoCA::EncoderMultiEncoderHub::IsThreadSafe() const
{
	const Config	*config = GetConfiguration();

	/* Check if at least one downstream encoder is not thread safe.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue(ConfigureMultiEncoderHub::ConfigID, "Encoders", "flac-enc,lame-enc").Explode(",");
	Bool			 threadSafe = True;

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			encoder->SetConfiguration(config);

			if (!encoder->IsThreadSafe()) threadSafe = False;

			boca.DeleteComponent(encoder);
		}
	}

	String::ExplodeFinish();

	return threadSafe;
}

Bool BoCA::EncoderMultiEncoderHub::IsLossless() const
{
	const Config	*config = GetConfiguration();

	/* Check if at least one downstream encoder is lossless.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue(ConfigureMultiEncoderHub::ConfigID, "Encoders", "flac-enc,lame-enc").Explode(",");
	Bool			 lossless   = False;

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			encoder->SetConfiguration(config);

			if (encoder->IsLossless()) lossless = True;

			boca.DeleteComponent(encoder);
		}
	}

	String::ExplodeFinish();

	return lossless;
}

Bool BoCA::EncoderMultiEncoderHub::Activate()
{
	const Config	*config = GetConfiguration();
	const Format	&format = track.GetFormat();

	finished    = False;
	cancelled   = False;

	trackLength = 0;
	totalLength = 0;

	/* Instantiate encoders.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue(ConfigureMultiEncoderHub::ConfigID, "Encoders", "flac-enc,lame-enc").Explode(",");

	String	 fileNamePattern = GetFileNamePattern(config, track);

	foreach (const String &encoderID, encoderIDs)
	{
		AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

		if (encoder != NIL)
		{
			/* Set up encoder and stream.
			 */
			encoder->SetConfiguration(config);

			Track	 encoderTrack = track;
			String	 fileName     = String(fileNamePattern).Replace("[FILETYPE]", encoder->GetOutputFileExtension().ToUpper()).Append(".").Append(encoder->GetOutputFileExtension());

			encoderTrack.outputFile = fileName;

			encoder->SetAudioTrackInfo(encoderTrack);

			OutStream	*stream = new OutStream(STREAM_FILE, Utilities::CreateDirectoryForFile(fileName), OS_REPLACE);

			stream->SetPackageSize(32768 * format.channels * (format.bits / 8));
			stream->SetFilter(encoder);

			encoders.Add(encoder);
			streams.Add(stream);

			/* Set up mutexes, buffers and threads.
			 */
			if ((config->GetIntValue("Settings", "EncodeToSingleFile", False) || !IsThreadSafe()) && CPU().GetNumLogicalCPUs() > 1)
			{
				mutexes.Add(new Threads::Mutex());
				buffers.Add(new Buffer<UnsignedByte>);
				threads.Add(NonBlocking1<Int>(&EncoderMultiEncoderHub::EncodeThread, this).Call(threads.Length()));
			}
		}
	}

	String::ExplodeFinish();

	return True;
}

Bool BoCA::EncoderMultiEncoderHub::Deactivate()
{
	const Config	*config = GetConfiguration();

	/* Signal encoder threads that we are done.
	 */
	finished = True;

	/* Free encoders.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	String	 fileNamePattern = GetFileNamePattern(config, track);

	for (Int i = encoders.Length() - 1; i >= 0; i--)
	{
		/* Delete mutexes, buffers and threads.
		 */
		if (threads.Length() > 0)
		{
			threads.GetNth(i)->Wait();

			delete buffers.GetNth(i);
			delete mutexes.GetNth(i);
		}

		/* Finish and delete encoder and stream.
		 */
		AS::EncoderComponent	*encoder = encoders.GetNth(i);
		OutStream		*stream	 = streams.GetNth(i);

		Track	 encoderTrack = track;
		String	 fileName     = String(fileNamePattern).Replace("[FILETYPE]", encoder->GetOutputFileExtension().ToUpper()).Append(".").Append(encoder->GetOutputFileExtension());

		encoderTrack.outputFile = fileName;

		encoder->SetAudioTrackInfo(encoderTrack);

		stream->RemoveFilter();

		if (encoder->GetErrorState()) { errorState = True; errorString = encoder->GetErrorString(); }

		delete stream;

		boca.DeleteComponent(encoder);

		if (cancelled)
		{
			File(encoderTrack.outputFile).Delete();

			if (config->GetIntValue(ConfigureMultiEncoderHub::ConfigID, "SeparateFolders", False) && !config->GetIntValue("Settings", "EncodeToSingleFile", False))
			{
				encoderTrack.outputFile[encoderTrack.outputFile.FindLast(Directory::GetDirectoryDelimiter())] = 0;

				Directory(encoderTrack.outputFile).Delete();
			}
		}
	}

	encoders.RemoveAll();
	streams.RemoveAll();

	mutexes.RemoveAll();
	buffers.RemoveAll();
	threads.RemoveAll();

	if (config->GetIntValue("Settings", "EncodeToSingleFile", False))
	{
		ConversionData	*data = conversionData.Get(conversionID);

		data->playlistTrack = track;
		data->playlistTrack.length = totalLength;
	}

	return True;
}

Int BoCA::EncoderMultiEncoderHub::WriteData(Buffer<UnsignedByte> &data)
{
	/* Hand data to encoders.
	 */
	if (threads.Length() == 0)
	{
		foreach (OutStream *stream, streams) stream->OutputData(data, data.Size());
	}
	else
	{
		for (Int i = 0; i < threads.Length(); i++)
		{
			Threads::Mutex		*mutex	= mutexes.GetNth(i);
			Buffer<UnsignedByte>	*buffer = buffers.GetNth(i);

			Bool	 done = False;

			while (!done)
			{
				mutex->Lock();

				if (buffer->Size() == 0)
				{
					buffer->Resize(data.Size());

					memcpy(*buffer, data, data.Size());

					done = True;
				}

				mutex->Release();
			}
		}
	}

	const Format	&format = track.GetFormat();

	trackLength += data.Size() / format.channels / (format.bits / 8);
	totalLength += data.Size() / format.channels / (format.bits / 8);

	return data.Size();
}

String BoCA::EncoderMultiEncoderHub::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (!config->GetIntValue("Settings", "EncodeToSingleFile", False)) return "[FILETYPE]";

	return NIL;
}

String BoCA::EncoderMultiEncoderHub::GetFileNamePattern(const Config *configuration, const Track &track)
{
	String	 fileNamePattern = track.outputFile;

	if (fileNamePattern.EndsWith(".[FILETYPE]")) fileNamePattern[fileNamePattern.Length() - 11] = 0;

	if (configuration->GetIntValue(ConfigureMultiEncoderHub::ConfigID, "SeparateFolders", False) && !configuration->GetIntValue("Settings", "EncodeToSingleFile", False))
	{
		String	 pre;
		String	 post = fileNamePattern;

		if (fileNamePattern.Contains(Directory::GetDirectoryDelimiter()))
		{
			pre  = fileNamePattern.Head(fileNamePattern.FindLast(Directory::GetDirectoryDelimiter()) + 1);
			post = fileNamePattern.Tail(fileNamePattern.Length() - fileNamePattern.FindLast(Directory::GetDirectoryDelimiter()) - 1);
		}

		fileNamePattern = String(pre).Append("[FILETYPE]").Append(Directory::GetDirectoryDelimiter()).Append(post);
	}

	return fileNamePattern;
}

String BoCA::EncoderMultiEncoderHub::GetPlaylistFileName(const Config *configuration, const Track &track, const Array<Track> &originalTracks)
{
	I18n		*i18n = I18n::Get();

	const Info	&info = track.GetInfo();

	/* Find playlist output folder.
	 */
	const Track	&originalTrack = originalTracks.Get(track.GetTrackID());

	String		 outputDir     = configuration->GetStringValue("Settings", "EncoderOutDir", NIL);
	String		 inputDir      = originalTrack.fileName.Head(Math::Max(originalTrack.fileName.FindLast("\\"), originalTrack.fileName.FindLast("/")) + 1);

	if (configuration->GetIntValue("Settings", "WriteToInputDirectory", False) && !originalTrack.isCDTrack)
	{
		String		 file = String(inputDir).Append(String::FromInt(S::System::System::Clock())).Append(".temp");
		OutStream	 temp(STREAM_FILE, file, OS_REPLACE);

		if (temp.GetLastError() == IO_ERROR_OK) outputDir = inputDir;

		temp.Close();

		File(file).Delete();
	}

	/* Generate playlist file name.
	 */
	Bool	 useUnicode	   = configuration->GetIntValue("Settings", "UseUnicodeFilenames", True);
	Bool	 replaceSpaces	   = configuration->GetIntValue("Settings", "FilenamesReplaceSpaces", False);

	String	 playlistOutputDir = Utilities::GetAbsolutePathName(configuration->GetIntValue("Playlist", "UseEncoderOutputDir", True) ? outputDir : configuration->GetStringValue("Playlist", "OutputDir", outputDir));
	String	 playlistFileName  = playlistOutputDir;

	if (info.artist != NIL || info.album != NIL)
	{
		String	 shortOutFileName = configuration->GetStringValue("Playlist", "FilenamePattern", String("<albumartist> - <album>").Append(Directory::GetDirectoryDelimiter()).Append("<albumartist> - <album>"));

		DateTime	 currentDateTime  = DateTime::Current();
		String		 currentDate	  = String().FillN('0', 3 - Math::Floor(Math::Log10(currentDateTime.GetYear()))).Append(String::FromInt(currentDateTime.GetYear()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetMonth())))).Append(String::FromInt(currentDateTime.GetMonth()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetDay())))).Append(String::FromInt(currentDateTime.GetDay()));
		String		 currentTime	  = String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetHour()))).Append(String::FromInt(currentDateTime.GetHour()))
					    .Append(String().FillN('0', 1 - Math::Floor(Math::Log10(currentDateTime.GetMinute())))).Append(String::FromInt(currentDateTime.GetMinute()));

		shortOutFileName.Replace("<artist>", Utilities::ReplaceIncompatibleCharacters(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")));
		shortOutFileName.Replace("<album>", Utilities::ReplaceIncompatibleCharacters(info.album.Length() > 0 ? info.album : i18n->TranslateString("unknown album")));
		shortOutFileName.Replace("<genre>", Utilities::ReplaceIncompatibleCharacters(info.genre.Length() > 0 ? info.genre : i18n->TranslateString("unknown genre")));
		shortOutFileName.Replace("<year>", Utilities::ReplaceIncompatibleCharacters(info.year > 0 ? String::FromInt(info.year) : i18n->TranslateString("unknown year")));
		shortOutFileName.Replace("<currentdate>", currentDate);
		shortOutFileName.Replace("<currenttime>", currentTime);

		/* Replace <disc> pattern.
		 */
		shortOutFileName.Replace("<disc>", String::FromInt(info.disc < 0 ? 0 : info.disc));

		for (Int i = 1; i <= 4; i++)
		{
			String	 pattern = String("<disc(").Append(String::FromInt(i)).Append(")>");

			shortOutFileName.Replace(pattern, String().FillN('0', i - ((Int) Math::Log10(info.disc > 0 ? info.disc : 1) + 1)).Append(String::FromInt(info.disc < 0 ? 0 : info.disc)));
		}

		/* Replace other text fields.
		 */
		foreach (const String &pair, info.other)
		{
			String	 key   = pair.Head(pair.Find(":"));
			String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

			if (value == NIL) continue;

			if	(key == INFO_ALBUMARTIST) shortOutFileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(value));
			else if	(key == INFO_CONDUCTOR)	  shortOutFileName.Replace("<conductor>", Utilities::ReplaceIncompatibleCharacters(value));
			else if	(key == INFO_COMPOSER)	  shortOutFileName.Replace("<composer>", Utilities::ReplaceIncompatibleCharacters(value));
		}

		if (info.artist.Length() > 0) shortOutFileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(info.artist));

		shortOutFileName.Replace("<albumartist>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown album artist")));
		shortOutFileName.Replace("<conductor>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown conductor")));
		shortOutFileName.Replace("<composer>", Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown composer")));

		playlistFileName.Append(Utilities::ReplaceIncompatibleCharacters(shortOutFileName, useUnicode, False, replaceSpaces));
	}
	else if (track.isCDTrack)
	{
		playlistFileName.Append("cd").Append(String::FromInt(track.drive));
	}
	else
	{
		playlistFileName.Append(Utilities::ReplaceIncompatibleCharacters(i18n->TranslateString("unknown playlist"), useUnicode, True, replaceSpaces));
	}

	if (configuration->GetIntValue("Playlist", "UseEncoderOutputDir", True))
	{
		Track	 playlistTrack;

		playlistTrack.outputFile = playlistFileName;

		playlistFileName = GetFileNamePattern(configuration, playlistTrack);
	}

	return Utilities::NormalizeFileName(playlistFileName);
}

Void BoCA::EncoderMultiEncoderHub::EncodeThread(Int n)
{
	Threads::Mutex		*mutex	= mutexes.GetNth(n);
	Buffer<UnsignedByte>	*buffer = buffers.GetNth(n);

	OutStream		*stream = streams.GetNth(n);

	while (!finished)
	{
		mutex->Lock();

		if (buffer->Size() > 0)
		{
			stream->OutputData(*buffer, buffer->Size());

			buffer->Resize(0);
		}

		mutex->Release();
	}
}

Void BoCA::EncoderMultiEncoderHub::OnStartConversion(Int conversionID, const Array<Track> &tracks)
{
	ConversionData	*data = new ConversionData();

	data->configuration   = Config::Copy();

	conversionData.Add(data, conversionID);

	if (data->configuration->GetStringValue("Settings", "Encoder", "lame-enc") != "meh-enc") return;

	/* Set tracks to convert.
	 */
	data->tracksToConvert = tracks;

	/* Enable locking on playlist track arrays.
	 */
	data->tracksToConvert.EnableLocking();
	data->convertedTracks.EnableLocking();
}

Void BoCA::EncoderMultiEncoderHub::OnFinishConversion(Int conversionID)
{
	AS::Registry	&boca = AS::Registry::Get();

	/* Get config values.
	 */
	ConversionData	*data = conversionData.Get(conversionID);

	Bool	 encodeToSingleFile	= data->configuration->GetIntValue("Settings", "EncodeToSingleFile", False);

	Bool	 separateFolders	= data->configuration->GetIntValue(ConfigureMultiEncoderHub::ConfigID, "SeparateFolders", False);

	Bool	 createPlaylist		= data->configuration->GetIntValue("Playlist", "CreatePlaylist", False);
	Bool	 createCueSheet		= data->configuration->GetIntValue("Playlist", "CreateCueSheet", False);

	Bool	 useEncoderOutputDir	= data->configuration->GetIntValue("Playlist", "UseEncoderOutputDir", True);
	Bool	 singlePlaylistFile	= data->configuration->GetIntValue("Playlist", "SinglePlaylistFile", False);

	/* Compute playlist/cuesheet track list.
	 */
	Array<Track>	 playlistTracks;
	Array<Track>	 cuesheetTracks;

	foreach (const Track &trackToConvert, data->tracksToConvert)
	{
		Track	 track = data->convertedTracks.Get(trackToConvert.GetTrackID());

		if (track == NIL) continue;

		if (!encodeToSingleFile)
		{
			Track	 playlistTrack = track;

			playlistTrack.isCDTrack	= False;
			playlistTrack.fileName	= track.outputFile;

			playlistTracks.Add(playlistTrack);
			cuesheetTracks.Add(playlistTrack);
		}
		else
		{
			Track	 cuesheetTrack = trackToConvert;

			cuesheetTrack.isCDTrack	   = False;
			cuesheetTrack.sampleOffset = track.sampleOffset;
			cuesheetTrack.length	   = track.length;
			cuesheetTrack.fileName	   = track.outputFile;

			cuesheetTracks.Add(cuesheetTrack);
		}
	}

	if (data->tracksToConvert.Length() > 0 && encodeToSingleFile) playlistTracks.Add(data->playlistTrack);

	/* Write playlists and cue sheets.
	 */
	if ((createPlaylist && playlistTracks.Length() > 0) ||
	    (createCueSheet && cuesheetTracks.Length() > 0))
	{
		String			 playlistID	   = data->configuration->GetStringValue("Playlist", "PlaylistFormat", "m3u-playlist-m3u8");
		String			 playlistExtension = playlistID.Tail(playlistID.Length() - playlistID.FindLast("-") - 1);

		/* Split playlist tracks to individual playlists.
		 */
		Array<String>		 playlistFileNames;

		Array<Array<Track> *>	 playlistTrackLists;
		Array<Array<Track> *>	 cuesheetTrackLists;

		if (encodeToSingleFile || singlePlaylistFile)
		{
			/* Set playlist filename so it is written to the same place as a single output file.
			 */
			if (encodeToSingleFile) playlistFileNames.Add(data->playlistTrack.outputFile);
			else			playlistFileNames.Add(GetPlaylistFileName(data->configuration, playlistTracks.GetFirst(), data->tracksToConvert));

			playlistTrackLists.Add(new Array<Track>(playlistTracks));
			cuesheetTrackLists.Add(new Array<Track>(cuesheetTracks));
		}
		else
		{
			foreach (const Track &track, playlistTracks)
			{
				/* Check if we already have a list for this playlist.
				 */
				String		 playlistFileName = GetPlaylistFileName(data->configuration, track, data->tracksToConvert);
				UnsignedInt32	 playlistFileCRC  = playlistFileName.ComputeCRC32();

				if (playlistFileNames.Add(playlistFileName, playlistFileCRC))
				{
					playlistTrackLists.Add(new Array<Track>(), playlistFileCRC);
					cuesheetTrackLists.Add(new Array<Track>(), playlistFileCRC);
				}

				/* Find current lists and add track.
				 */
				playlistTrackLists.Get(playlistFileCRC)->Add(track);
				cuesheetTrackLists.Get(playlistFileCRC)->Add(track);
			}
		}

		/* Save playlists per encoder.
		 */
		const Array<String>	&encoderIDs = data->configuration->GetStringValue(ConfigureMultiEncoderHub::ConfigID, "Encoders", "flac-enc,lame-enc").Explode(",");

		foreach (const String &encoderID, encoderIDs)
		{
			/* Get encoder file extension.
			 */
			String			 formatExtension;
			AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

			if (encoder != NIL)
			{
				encoder->SetConfiguration(data->configuration);

				formatExtension = encoder->GetOutputFileExtension();

				boca.DeleteComponent(encoder);
			}

			/* Write playlists and cue sheets.
			 */
			for (Int i = 0; i < playlistFileNames.Length(); i++)
			{
				/* Update playlist tracks.
				 */
				String		 actualPlaylistFileName = String(playlistFileNames.GetNth(i)).Replace("[FILETYPE]", formatExtension.ToUpper());

				if (!separateFolders || !useEncoderOutputDir || encodeToSingleFile) actualPlaylistFileName.Append(".").Append(formatExtension);

				Array<Track>	 actualPlaylistTracks	= *playlistTrackLists.GetNth(i);
				Array<Track>	 actualCuesheetTracks	= *cuesheetTrackLists.GetNth(i);

				foreach (Track &playlistTrack, actualPlaylistTracks) playlistTrack.fileName = String(playlistTrack.fileName).Replace("[FILETYPE]", formatExtension.ToUpper()).Append(".").Append(formatExtension);
				foreach (Track &cuesheetTrack, actualCuesheetTracks) cuesheetTrack.fileName = String(cuesheetTrack.fileName).Replace("[FILETYPE]", formatExtension.ToUpper()).Append(".").Append(formatExtension);

				/* Write playlist.
				 */
				AS::PlaylistComponent	*playlist = createPlaylist ? (AS::PlaylistComponent *) boca.CreateComponentByID(playlistID.Head(playlistID.FindLast("-"))) : NIL;

				if (playlist != NIL)
				{
					playlist->SetTrackList(actualPlaylistTracks);
					playlist->WritePlaylist(String(actualPlaylistFileName).Append(".").Append(playlistExtension));

					boca.DeleteComponent(playlist);
				}

				/* Write cue sheet.
				 */
				AS::PlaylistComponent	*cuesheet = createCueSheet ? (AS::PlaylistComponent *) boca.CreateComponentByID("cuesheet-playlist") : NIL;

				if (cuesheet != NIL)
				{
					cuesheet->SetTrackList(actualCuesheetTracks);
					cuesheet->WritePlaylist(String(actualPlaylistFileName).Append(".cue"));

					boca.DeleteComponent(cuesheet);
				}
			}
		}

		String::ExplodeFinish();

		/* Clean up playlist and cuesheet track lists.
		 */
		foreach (Array<Track> *trackList, playlistTrackLists) delete trackList;
		foreach (Array<Track> *trackList, cuesheetTrackLists) delete trackList;
	}

	/* Free conversion data for this conversion.
	 */
	Config::Free(data->configuration);

	delete data;

	conversionData.Remove(conversionID);
}

Void BoCA::EncoderMultiEncoderHub::OnCancelConversion(Int conversionID)
{
	/* Free conversion data for this conversion.
	 */
	ConversionData	*data = conversionData.Get(conversionID);

	Config::Free(data->configuration);

	delete data;

	conversionData.Remove(conversionID);
}

Void BoCA::EncoderMultiEncoderHub::OnFinishTrackConversion(Int conversionID, const Track &finishedTrack)
{
	const Config	*config = GetConfiguration();

	/* Get config values.
	 */
	Bool	 encodeToSingleFile = config->GetIntValue("Settings", "EncodeToSingleFile", False);

	/* Update conversion ID.
	 */
	this->conversionID = conversionID;

	/* Check if this conversion is the one being finished.
	 */
	if ((encodeToSingleFile && finishedTrack.outputFile == track.outputFile) ||
				   finishedTrack.GetTrackID() == track.GetTrackID())
	{
		Track	 convertedTrack = finishedTrack;

		convertedTrack.SetFormat(track.GetFormat());

		convertedTrack.outputFile = GetFileNamePattern(config, track);
		convertedTrack.length	  = trackLength;

		if (encodeToSingleFile)
		{
			convertedTrack.sampleOffset = Math::Round((Float) (totalLength - trackLength) / convertedTrack.GetFormat().rate * 75);

			trackLength = 0;
		}

		/* Add track to converted tracks.
		 */
		ConversionData	*data = conversionData.Get(conversionID);

		data->convertedTracks.Add(convertedTrack, convertedTrack.GetTrackID());
	}
}

Void BoCA::EncoderMultiEncoderHub::OnCancelTrackConversion(Int conversionID, const Track &cancelledTrack)
{
	const Config	*config = GetConfiguration();

	/* Get config values.
	 */
	Bool	 encodeToSingleFile = config->GetIntValue("Settings", "EncodeToSingleFile", False);

	/* Update conversion ID.
	 */
	this->conversionID = conversionID;

	/* Check if this conversion is the one being cancelled.
	 */
	if ((encodeToSingleFile && cancelledTrack.outputFile == track.outputFile) ||
				   cancelledTrack.GetTrackID() == track.GetTrackID()) cancelled = True;
}

ConfigLayer *BoCA::EncoderMultiEncoderHub::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMultiEncoderHub();

	return configLayer;
}
