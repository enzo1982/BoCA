 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/component/playlistcomponent.h>

BoCA::CS::PlaylistComponent::PlaylistComponent()
{
}

BoCA::CS::PlaylistComponent::~PlaylistComponent()
{
}

Void BoCA::CS::PlaylistComponent::SetTrackList(const Array<Track> &tracks)
{
	trackList = tracks;
}

Bool BoCA::CS::PlaylistComponent::CanOpenFile(const String &file)
{
	return False;
}

const Array<BoCA::Track> &BoCA::CS::PlaylistComponent::ReadPlaylist(const String &file)
{
	trackList.RemoveAll();

	return trackList;
}

Error BoCA::CS::PlaylistComponent::WritePlaylist(const String &file)
{
	return Error();
}
