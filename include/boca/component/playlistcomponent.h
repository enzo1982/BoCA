 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_PLAYLISTCOMPONENT
#define H_BOCA_PLAYLISTCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT PlaylistComponent : public Component
		{
			protected:
				Array<Track>			 trackList;
			public:
								 PlaylistComponent();
				virtual				~PlaylistComponent();

				virtual Void			 SetTrackList(const Array<Track> &);

				virtual Bool			 CanOpenFile(const String &);

				virtual const Array<Track>	&ReadPlaylist(const String &);
				virtual Error			 WritePlaylist(const String &);
		};
	};
};

#endif
