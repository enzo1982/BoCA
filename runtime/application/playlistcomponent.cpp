 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/playlistcomponent.h>

BoCA::AS::PlaylistComponent::PlaylistComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::PlaylistComponent::~PlaylistComponent()
{
}

Void BoCA::AS::PlaylistComponent::SetTrackList(const Array<Track> &tracks)
{
	specs->func_SetTrackList(component, &tracks);
}

const Array<BoCA::Track> &BoCA::AS::PlaylistComponent::ReadPlaylist(const String &file)
{
	return *((const Array<Track> *) specs->func_ReadPlaylist(component, file));
}

Error BoCA::AS::PlaylistComponent::WritePlaylist(const String &file)
{
	return specs->func_WritePlaylist(component, file);
}
