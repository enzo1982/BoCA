 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>meh! - multi encoder hub</name>	\
	    <version>1.0</version>			\
	    <id>meh-enc</id>				\
	    <type>encoder</type>			\
	    <format>					\
	      <name>multi encoder hub Output</name>	\
	    </format>					\
	  </component>					\
							\
	";

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

Config		*BoCA::EncoderMultiEncoderHub::configuration = NIL;

Array<Track>	 BoCA::EncoderMultiEncoderHub::tracksToConvert;
Array<Track>	 BoCA::EncoderMultiEncoderHub::convertedTracks;

Track		 BoCA::EncoderMultiEncoderHub::playlistTrack;

BoCA::EncoderMultiEncoderHub::EncoderMultiEncoderHub()
{
	finished    = False;
	cancelled   = False;

	trackLength = 0;
	totalLength = 0;

	configLayer = NIL;

	Engine	*engine = Engine::Get();

	engine->onFinishTrackConversion.Connect(&EncoderMultiEncoderHub::OnFinishTrackConversion, this);
	engine->onCancelTrackConversion.Connect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);
}

BoCA::EncoderMultiEncoderHub::~EncoderMultiEncoderHub()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);

	const Config	*config	 = GetConfiguration();
	AS::Registry	&boca	 = AS::Registry::Get();

	Engine		*engine	 = Engine::Get();
	JobList		*joblist = JobList::Get();

	engine->onFinishTrackConversion.Disconnect(&EncoderMultiEncoderHub::OnFinishTrackConversion, this);
	engine->onCancelTrackConversion.Disconnect(&EncoderMultiEncoderHub::OnCancelTrackConversion, this);

	/* Delete output file if it still exists.
	 */
	if (track.outfile != NIL)
	{
		File(track.outfile).Delete();

		if (track.outfile.Contains(Directory::GetDirectoryDelimiter())) track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;

		if (!cancelled && !config->GetIntValue("Settings", "EncodeToSingleFile", False) &&
				   config->GetIntValue("Settings", "RemoveTracks", True)) joblist->onComponentRemoveTrack.Emit(track);
	}

	/* Delete empty folders if <filetype> was used in path.
	 */
	while (track.outfile.Contains("[FILETYPE]"))
	{
		Directory(track.outfile).Delete();

		if (track.outfile.Contains(Directory::GetDirectoryDelimiter())) track.outfile[track.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;
		else								break;
	}
}

Bool BoCA::EncoderMultiEncoderHub::IsThreadSafe() const
{
	const Config	*config = GetConfiguration();

	/* Check if at least one downstream encoder is not thread safe.
	 */
	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");
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
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");
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
	const Array<String>	&encoderIDs = config->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");

	String	 fileNamePattern = GetFileNamePattern(track);

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

			encoderTrack.outfile = fileName;

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

	String	 fileNamePattern = GetFileNamePattern(track);

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

		encoderTrack.outfile = fileName;

		encoder->SetAudioTrackInfo(encoderTrack);

		stream->RemoveFilter();

		if (encoder->GetErrorState()) { errorState = True; errorString = encoder->GetErrorString(); }

		delete stream;

		boca.DeleteComponent(encoder);

		if (cancelled)
		{
			File(encoderTrack.outfile).Delete();

			if (config->GetIntValue("meh!", "SeparateFolders", False) && !config->GetIntValue("Settings", "EncodeToSingleFile", False))
			{
				encoderTrack.outfile[encoderTrack.outfile.FindLast(Directory::GetDirectoryDelimiter())] = 0;

				Directory(encoderTrack.outfile).Delete();
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
		playlistTrack = track;
		playlistTrack.length = totalLength;
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

			mutex->Lock();

			buffer->Resize(data.Size());

			memcpy(*buffer, data, data.Size());

			mutex->Release();
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

String BoCA::EncoderMultiEncoderHub::GetFileNamePattern(const Track &track)
{
	String	 fileNamePattern = track.outfile;

	if (fileNamePattern.EndsWith(".[FILETYPE]")) fileNamePattern[fileNamePattern.Length() - 11] = 0;

	if (configuration->GetIntValue("meh!", "SeparateFolders", False) && !configuration->GetIntValue("Settings", "EncodeToSingleFile", False))
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

String BoCA::EncoderMultiEncoderHub::GetPlaylistFileName(const Track &track)
{
	I18n		*i18n = I18n::Get();

	const Info	&info = track.GetInfo();

	String	 outputDir	   = configuration->GetStringValue("Settings", "EncoderOutDir", NIL);
	Bool	 useUnicode	   = configuration->GetIntValue("Settings", "UseUnicodeFilenames", True);
	Bool	 replaceSpaces	   = configuration->GetIntValue("Settings", "FilenamesReplaceSpaces", False);

	String	 playlistOutputDir = Utilities::GetAbsolutePathName(configuration->GetIntValue("Playlist", "UseEncoderOutputDir", True) ? outputDir : configuration->GetStringValue("Playlist", "OutputDir", outputDir));
	String	 playlistFileName  = playlistOutputDir;

	if (info.artist != NIL || info.album != NIL)
	{
		String	 shortOutFileName = configuration->GetStringValue("Playlist", "FilenamePattern", String("<artist> - <album>").Append(Directory::GetDirectoryDelimiter()).Append("<artist> - <album>"));

		shortOutFileName.Replace("<artist>", Utilities::ReplaceIncompatibleCharacters(info.artist.Length() > 0 ? info.artist : i18n->TranslateString("unknown artist")));
		shortOutFileName.Replace("<album>", Utilities::ReplaceIncompatibleCharacters(info.album.Length() > 0 ? info.album : i18n->TranslateString("unknown album")));
		shortOutFileName.Replace("<genre>", Utilities::ReplaceIncompatibleCharacters(info.genre.Length() > 0 ? info.genre : i18n->TranslateString("unknown genre")));
		shortOutFileName.Replace("<year>", Utilities::ReplaceIncompatibleCharacters(info.year > 0 ? String::FromInt(info.year) : i18n->TranslateString("unknown year")));

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

		playlistTrack.outfile = playlistFileName;

		playlistFileName = GetFileNamePattern(playlistTrack);
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
		while (buffer->Size() == 0 && !finished) S::System::System::Sleep(0);

		mutex->Lock();

		stream->OutputData(*buffer, buffer->Size());

		buffer->Resize(0);

		mutex->Release();
	}
}

Void BoCA::EncoderMultiEncoderHub::OnStartConversion(const Array<Track> &tracks)
{
	configuration	= Config::Copy();

	if (configuration->GetStringValue("Settings", "Encoder", "lame-enc") != "meh-enc") return;

	/* Set tracks to convert.
	 */
	tracksToConvert = tracks;

	/* Enable locking on playlist track arrays.
	 */
	tracksToConvert.EnableLocking();
	convertedTracks.EnableLocking();
}

Void BoCA::EncoderMultiEncoderHub::OnFinishConversion()
{
	AS::Registry	&boca = AS::Registry::Get();

	/* Get config values.
	 */
	Bool	 encodeToSingleFile	= configuration->GetIntValue("Settings", "EncodeToSingleFile", False);

	Bool	 separateFolders	= configuration->GetIntValue("meh!", "SeparateFolders", False);

	Bool	 createPlaylist		= configuration->GetIntValue("Playlist", "CreatePlaylist", False);
	Bool	 createCueSheet		= configuration->GetIntValue("Playlist", "CreateCueSheet", False);

	Bool	 useEncoderOutputDir	= configuration->GetIntValue("Playlist", "UseEncoderOutputDir", True);
	Bool	 singlePlaylistFile	= configuration->GetIntValue("Playlist", "SinglePlaylistFile", False);

	/* Compute playlist/cuesheet track list.
	 */
	Array<Track>	 playlistTracks;
	Array<Track>	 cuesheetTracks;

	foreach (const Track &trackToConvert, tracksToConvert)
	{
		Track	 track = convertedTracks.Get(trackToConvert.GetTrackID());

		if (track == NIL) continue;

		if (!encodeToSingleFile)
		{
			Track	 playlistTrack = track;

			playlistTrack.isCDTrack	   = False;
			playlistTrack.origFilename = track.outfile;

			playlistTracks.Add(playlistTrack);
			cuesheetTracks.Add(playlistTrack);
		}
		else
		{
			Track	 cuesheetTrack = trackToConvert;

			cuesheetTrack.isCDTrack	   = False;
			cuesheetTrack.sampleOffset = track.sampleOffset;
			cuesheetTrack.length	   = track.length;
			cuesheetTrack.origFilename = track.outfile;

			cuesheetTracks.Add(cuesheetTrack);
		}
	}

	if (tracksToConvert.Length() > 0 && encodeToSingleFile) playlistTracks.Add(playlistTrack);

	/* Write playlists and cue sheets.
	 */
	if ((createPlaylist && playlistTracks.Length() > 0) ||
	    (createCueSheet && cuesheetTracks.Length() > 0))
	{
		String			 playlistID	   = configuration->GetStringValue("Playlist", "PlaylistFormat", "m3u-playlist-m3u8");
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
			if (encodeToSingleFile) playlistFileNames.Add(playlistTrack.outfile);
			else			playlistFileNames.Add(GetPlaylistFileName(playlistTracks.GetFirst()));

			playlistTrackLists.Add(new Array<Track>(playlistTracks));
			cuesheetTrackLists.Add(new Array<Track>(cuesheetTracks));
		}
		else
		{
			foreach (const Track &track, playlistTracks)
			{
				/* Check if we already have a list for this playlist.
				 */
				String		 playlistFileName = GetPlaylistFileName(track);
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
		const Array<String>	&encoderIDs = configuration->GetStringValue("meh!", "Encoders", "flac-enc,lame-enc").Explode(",");

		foreach (const String &encoderID, encoderIDs)
		{
			/* Get encoder file extension.
			 */
			String			 formatExtension;
			AS::EncoderComponent	*encoder = (AS::EncoderComponent *) boca.CreateComponentByID(encoderID);

			if (encoder != NIL)
			{
				encoder->SetConfiguration(configuration);

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

				foreach (Track &playlistTrack, actualPlaylistTracks) playlistTrack.origFilename = String(playlistTrack.origFilename).Replace("[FILETYPE]", formatExtension.ToUpper()).Append(".").Append(formatExtension);
				foreach (Track &cuesheetTrack, actualCuesheetTracks) cuesheetTrack.origFilename = String(cuesheetTrack.origFilename).Replace("[FILETYPE]", formatExtension.ToUpper()).Append(".").Append(formatExtension);

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

	/* Clear tracks to convert and converted tracks array.
	 */
	tracksToConvert.RemoveAll();
	convertedTracks.RemoveAll();

	Config::Free(configuration);
}

Void BoCA::EncoderMultiEncoderHub::OnCancelConversion()
{
	/* Clear tracks to convert and converted tracks array.
	 */
	tracksToConvert.RemoveAll();
	convertedTracks.RemoveAll();

	Config::Free(configuration);
}

Void BoCA::EncoderMultiEncoderHub::OnFinishTrackConversion(const Track &finishedTrack)
{
	const Config	*config = GetConfiguration();

	/* Get config values.
	 */
	Bool	 encodeToSingleFile = config->GetIntValue("Settings", "EncodeToSingleFile", False);

	/* Check if this conversion is the one being finished.
	 */
	if ((encodeToSingleFile && finishedTrack.outfile == track.outfile) ||
				   finishedTrack.GetTrackID() == track.GetTrackID())
	{
		Track	 convertedTrack = finishedTrack;

		convertedTrack.outfile = GetFileNamePattern(track);
		convertedTrack.length  = trackLength;

		if (encodeToSingleFile)
		{
			convertedTrack.sampleOffset = Math::Round((Float) (totalLength - trackLength) / convertedTrack.GetFormat().rate * 75);

			trackLength = 0;
		}

		convertedTracks.Add(convertedTrack, convertedTrack.GetTrackID());
	}
}

Void BoCA::EncoderMultiEncoderHub::OnCancelTrackConversion(const Track &cancelledTrack)
{
	const Config	*config = GetConfiguration();

	/* Get config values.
	 */
	Bool	 encodeToSingleFile = config->GetIntValue("Settings", "EncodeToSingleFile", False);

	/* Check if this conversion is the one being cancelled.
	 */
	if ((encodeToSingleFile && cancelledTrack.outfile == track.outfile) ||
				   cancelledTrack.GetTrackID() == track.GetTrackID()) cancelled = True;
}

ConfigLayer *BoCA::EncoderMultiEncoderHub::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMultiEncoderHub();

	return configLayer;
}
