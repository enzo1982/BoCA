 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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

Bool BoCA::AS::PlaylistComponent::CanOpenFile(const String &file)
{
	return specs->func_CanOpenFile(component, file);
}

const Array<BoCA::Track> &BoCA::AS::PlaylistComponent::ReadPlaylist(const String &file)
{
	return *((const Array<Track> *) specs->func_ReadPlaylist(component, file));
}

Error BoCA::AS::PlaylistComponent::WritePlaylist(const String &file)
{
	return specs->func_WritePlaylist(component, file);
}
