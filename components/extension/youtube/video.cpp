 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "video.h"
#include "videosite.h"

using namespace smooth::Net;
using namespace smooth::Threads;
using namespace smooth::IO;

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
	startDownload.Emit(this);

	videoDownloadStarted = True;

	DownloadPage();

	Bool		 error = False;
	String		 cacheURL = videoSite->GetVideoURL(videoPageHTML);

	if (!cacheURL.StartsWith("http://") || cacheURL.Length() < 11) error = True;

	if (!error)
	{
		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(cacheURL);

		protocol->downloadProgress.Connect(&downloadProgress);
		protocol->downloadSpeed.Connect(&downloadSpeed);
		protocol->doCancelDownload.DisconnectAll();
		protocol->doCancelDownload.Connect(&Video::DoCancelDownload, this);
		protocol->DownloadToFile(targetFileName);

		String		 streamURL = ((Protocols::HTTP *) protocol)->GetResponseHeaderField("Location");

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

	finishDownload.Emit(this);

	videoDownloadFinished = True;

	if (error) return Error();
	else	   return Success();
}

Bool BoCA::Video::DownloadPage()
{
	downloadMutex.Lock();

	if (pageDownloaded) { downloadMutex.Release(); return True;}

	Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(videoURL);
	Buffer<UnsignedByte>	 buffer;

	protocol->DownloadToBuffer(buffer);

	videoPageHTML.ImportFrom("ISO-8859-1", (char *) (UnsignedByte *) buffer);

	delete protocol;

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
