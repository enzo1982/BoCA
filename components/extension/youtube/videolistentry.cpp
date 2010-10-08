 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "videolistentry.h"

using namespace smooth::Net;

BoCA::VideoListEntry::VideoListEntry(Video *iVideo) : ListEntry(NIL)
{
	SetWidth(80);

	video	 = iVideo;
	image	 = CreateImageFromVideo();

	image->SetMetrics(Point(3, 3), GetSize() - Size(6, 15));

	progress = new Progressbar(Point(2, 12), Size(GetWidth() - 4, 10), OR_HORZ, PB_NOTEXT, 0, 1000);
	progress->SetOrientation(OR_LOWERLEFT);

	video->downloadProgress.Connect(&Progressbar::SetValue, progress);
	video->finishDownload.Connect(&VideoListEntry::OnFinishDownload, this);

	Add(progress);
	Add(image);

	downloadFinished = False;

	contextMenu = NIL;

	SetTooltipText(video->GetVideoTitle());

	onChangeSize.Connect(&VideoListEntry::OnChangeSize, this);

	getContextMenu.Connect(&VideoListEntry::GetContextMenu, this);
}

BoCA::VideoListEntry::~VideoListEntry()
{
	if (contextMenu != NIL) DeleteObject(contextMenu);

	video->downloadProgress.Disconnect(&Progressbar::SetValue, progress);
	video->finishDownload.Disconnect(&VideoListEntry::OnFinishDownload, this);

	DeleteObject(image);
	DeleteObject(progress);

	if (IsRegistered() && container != NIL) container->Remove(this);
}

Int BoCA::VideoListEntry::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!IsVisible())	return Success();

	Surface	*surface	= GetDrawSurface();
	Point	 realPos	= GetRealPosition();

	switch (message)
	{
		case SP_SHOW:
		case SP_PAINT:
			surface->StartPaint(GetVisibleArea());

			surface->Box(Rect(realPos, GetSize()), Setup::ClientColor, Rect::Filled);

			image->Paint(SP_PAINT);

			if (progress->IsVisible())
			{
				progress->Paint(SP_PAINT);

				surface->Box(Rect(progress->GetRealPosition(), progress->GetSize()), Setup::ClientColor, Rect::Outlined);
			}

			surface->EndPaint();

			break;
		case SP_MOUSEIN:
			surface->StartPaint(GetVisibleArea());

			surface->Box(Rect(realPos,				Size(GetWidth(), 2)), Setup::GradientStartColor, Rect::Filled);
			surface->Box(Rect(realPos,				Size(2, GetHeight())), Setup::GradientStartColor, Rect::Filled);
			surface->Box(Rect(realPos + Point(GetWidth() - 2, 0),	Size(2, GetHeight())), Setup::GradientStartColor, Rect::Filled);
			surface->Box(Rect(realPos + Point(0, GetHeight() - 2),	Size(GetWidth(), 2)), Setup::GradientStartColor, Rect::Filled);

			surface->EndPaint();

			break;
		case SP_MOUSEOUT:
			surface->StartPaint(GetVisibleArea());

			surface->Box(Rect(realPos,				Size(GetWidth(), 2)), Setup::ClientColor, Rect::Filled);
			surface->Box(Rect(realPos,				Size(2, GetHeight())), Setup::ClientColor, Rect::Filled);
			surface->Box(Rect(realPos + Point(GetWidth() - 2, 0),	Size(2, GetHeight())), Setup::ClientColor, Rect::Filled);
			surface->Box(Rect(realPos + Point(0, GetHeight() - 2),	Size(GetWidth(), 2)), Setup::ClientColor, Rect::Filled);

			surface->EndPaint();

			break;
	}

	return Success();
}

Image *BoCA::VideoListEntry::CreateImageFromVideo()
{
	if (video == NIL) return NIL;

	video->QueryMetadata();

	/* Download video thumbnail.
	 */
	if (video->GetVideoThumbnailURL().StartsWith("http://"))
	{
		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(video->GetVideoThumbnailURL());
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

		Picture	 picture;

		picture.mime = "image/jpeg";
		picture.data = buffer;

		return new Image(picture.GetBitmap());
	}

	return new Image(Bitmap());
}

Void BoCA::VideoListEntry::OnFinishDownload()
{
	downloadFinished = True;

	progress->Hide();
	image->SetSize(GetSize() - Size(6, 6));
}

Void BoCA::VideoListEntry::OnChangeSize(const Size &newSize)
{
	if (progress->GetValue() < 1000) image->SetSize(newSize - Size(6, 15));
	else				 image->SetSize(newSize - Size(6, 6));
}

PopupMenu *BoCA::VideoListEntry::GetContextMenu()
{
	if (downloadFinished) return NIL;

	if (contextMenu == NIL) contextMenu = new PopupMenu();

	contextMenu->RemoveAllEntries();

	MenuEntry	*entryCancel = contextMenu->AddEntry("Cancel download");

	entryCancel->onAction.Connect(&Video::CancelDownload, video);

	return contextMenu;
}

Bool BoCA::VideoListEntry::IsTypeCompatible(Short compType) const
{
	if (compType == Object::classID || compType == Widget::classID || compType == ListEntry::classID)	return True;
	else													return False;
}
