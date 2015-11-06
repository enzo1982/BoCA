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

#include <boca.h>

BoCA_BEGIN_COMPONENT(PlaylistM3U)

namespace BoCA
{
	class PlaylistM3U : public CS::PlaylistComponent
	{
		public:
			static const String	&GetComponentSpecs();

						 PlaylistM3U();
						~PlaylistM3U();

			Bool			 CanOpenFile(const String &);

			const Array<Track>	&ReadPlaylist(const String &);
			Error			 WritePlaylist(const String &);
	};
};

BoCA_DEFINE_PLAYLIST_COMPONENT(PlaylistM3U)

BoCA_END_COMPONENT(PlaylistM3U)
