 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(PlaylistCueSheet)

namespace BoCA
{
	class PlaylistCueSheet : public CS::PlaylistComponent
	{
		private:
			String			 GetFileType(const String &);
		public:
			static const String	&GetComponentSpecs();

						 PlaylistCueSheet();
						~PlaylistCueSheet();

			Error			 WritePlaylist(const String &);
	};
};

BoCA_DEFINE_PLAYLIST_COMPONENT(PlaylistCueSheet)

BoCA_END_COMPONENT(PlaylistCueSheet)
