 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_PLAYLISTCOMPONENT
#define H_BOCA_AS_PLAYLISTCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT PlaylistComponent : public Component
		{
			public:
								 PlaylistComponent(ComponentSpecs *);
				virtual				~PlaylistComponent();

				virtual Void			 SetTrackList(const Array<Track> &);

				virtual Bool			 CanOpenFile(const String &);

				virtual const Array<Track>	&ReadPlaylist(const String &);
				virtual Error			 WritePlaylist(const String &);
		};
	};
};

#endif