 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "video.h"
#include "videosite.h"
#include "converter.h"

using namespace smooth::Net;
using namespace smooth::Threads;
using namespace smooth::IO;

Int	 BoCA::Video::activeDownloads = 0;

BoCA::Video::Video(VideoSite *iVideoSite)
{
	pageDownloaded = False;
	metadataQueried = False;

	videoDownloadStarted = False;
	videoDownloadFinished = False;

	doCancelDownload = False;

	videoSite = iVideoSite;
}

BoCA::Video::~Video()
{
	CancelDownload();
}

Int BoCA::Video::DownloaderThread(String targetFileName)
{
	Config	*config = Config::Get();

	/* Wait until number of active downloads drops below maximum.
	 */
	Int	&maxActiveDownloads = config->GetPersistentIntValue("YouTube", "MaxDownloads", 8);

	while (activeDownloads >= maxActiveDownloads) S::System::System::Sleep(100);

	activeDownloads++;

	/* Start download.
	 */
	startDownload.Emit(this);

	videoDownloadStarted = True;

	DownloadPage();

	/* Create target directory if it does not exist yet.
	 */
	Directory(File(targetFileName).GetFilePath()).Create();

	/* Get video URL and start download.
	 */
	Bool		 error = False;
	String		 cacheURL = videoSite->GetVideoURL(videoPageHTML);

	if (!(cacheURL.StartsWith("http://") || cacheURL.StartsWith("https://")) || cacheURL.Length() < 11) error = True;

	if (!error)
	{
		/* HTTPS is not supported yet, so try using HTTP.
		 */
		cacheURL.Replace("https://", "http://");

		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(cacheURL);

		protocol->downloadProgress.Connect(&downloadProgress);
		protocol->downloadSpeed.Connect(&downloadSpeed);
		protocol->doCancelDownload.DisconnectAll();
		protocol->doCancelDownload.Connect(&Video::DoCancelDownload, this);
		protocol->DownloadToFile(targetFileName);

		String	 streamURL = ((Protocols::HTTP *) protocol)->GetResponseHeaderField("Location");

		delete protocol;

		if (streamURL != NIL)
		{
			Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(streamURL);

			protocol->downloadProgress.Connect(&downloadProgress);
			protocol->downloadSpeed.Connect(&downloadSpeed);
			protocol->doCancelDownload.DisconnectAll();
			protocol->doCancelDownload.Connect(&Video::DoCancelDownload, this);
			protocol->DownloadToFile(targetFileName);

			delete protocol;
		}
	}

	/* Convert video file if requested.
	 */
	if (config->GetIntValue("YouTube", "OutputFormat", -1) >= 0)
	{
		Array<Converter *>	&converters = Converter::Get();
		Converter		*converter  = converters.GetNth(config->GetIntValue("YouTube", "OutputFormat", -1));

		String			 convertedFileName = targetFileName.Head(targetFileName.FindLast(".") + 1).Append(converter->GetFormat()->GetExtensions().GetFirst());

		if (targetFileName != convertedFileName)
		{
			if (converter->Run(targetFileName, convertedFileName) == Success())
			{
				SetVideoFile(convertedFileName);

				File(targetFileName).Delete();
			}
			else
			{
				File(convertedFileName).Delete();
			}
		}
	}

	/* Finish and clean up.
	 */
	finishDownload.Emit(this);

	videoDownloadFinished = True;

	activeDownloads--;

	if (error) return Error();
	else	   return Success();
}

Bool BoCA::Video::DownloadPage()
{
	downloadMutex.Lock();

	if (pageDownloaded) { downloadMutex.Release(); return True; }

	/* HTTPS is not supported yet, so try using HTTP.
	 */
	videoURL.Replace("https://", "http://");

	Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(videoURL);
	Buffer<UnsignedByte>	 buffer;

	protocol->DownloadToBuffer(buffer);

	String	 location = ((Protocols::HTTP *) protocol)->GetResponseHeaderField("Location");

	delete protocol;

	if (location != NIL)
	{
		if (location.StartsWith("/"))
		{
			String	 proto	= videoURL.Head(videoURL.Find("//") + 2);
			String	 server = videoURL.SubString(proto.Length(), videoURL.Tail(videoURL.Length() - proto.Length()).Find("/"));

			location = String(proto).Append(server).Append(location);
		}

		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(location);

		protocol->DownloadToBuffer(buffer);

		delete protocol;
	}

	videoPageHTML.ImportFrom("ISO-8859-1", (char *) (UnsignedByte *) buffer);

	pageDownloaded = True;

	downloadMutex.Release();

	return True;
}

Bool BoCA::Video::QueryMetadata()
{
	if (metadataQueried) return True;

	DownloadPage();

	Metadata	 metadata = videoSite->QueryMetadata(videoPageHTML);

	videoTitle	  = metadata.title;
	videoDescription  = metadata.description;
	videoDate	  = metadata.date;
	videoUploader	  = metadata.uploader;
	videoThumbnailURL = metadata.thumbnail;

	/* Download video thumbnail.
	 */
	if (videoThumbnailURL.StartsWith("http://") || videoThumbnailURL.StartsWith("https://"))
	{
		/* HTTPS is not supported yet, so try using HTTP.
		 */
		videoThumbnailURL.Replace("https://", "http://");

		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(videoThumbnailURL);
		Buffer<UnsignedByte>	 buffer;

		protocol->DownloadToBuffer(buffer);

		String		 streamURL = ((Protocols::HTTP *) protocol)->GetResponseHeaderField("Location");

		delete protocol;

		if (streamURL != NIL)
		{
			Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(streamURL);

			protocol->DownloadToBuffer(buffer);

			delete protocol;
		}

		videoThumbnail.mime = "image/jpeg";
		videoThumbnail.type = 0;
		videoThumbnail.data = buffer;
	}

	metadataQueried = True;

	return True;
}

Bool BoCA::Video::Download()
{
	if (videoFile == NIL) return False;

	Thread	*thread = NonBlocking1<String>(&Video::DownloaderThread, this).Call(videoFile);

	thread->SetFlags(THREAD_KILLFLAG_WAIT);

	return True;
}

Bool BoCA::Video::CancelDownload()
{
	if (videoDownloadStarted && !videoDownloadFinished)
	{
		/* Cancel download.
		 */
		doCancelDownload = True;

		while (!videoDownloadFinished) S::System::System::Sleep(0);

		/* Remove partially downloaded file.
		 */
		File(videoFile).Delete();
	}

	return True;
}
