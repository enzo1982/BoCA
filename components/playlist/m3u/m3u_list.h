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

BoCA_BEGIN_COMPONENT(M3UPlaylist)

namespace BoCA
{
	class M3UPlaylist : public CS::PlaylistComponent
	{
		public:
			static const String	&GetComponentSpecs();

						 M3UPlaylist();
						~M3UPlaylist();

			Bool			 CanOpenFile(const String &);

			const Array<Track>	&ReadPlaylist(const String &);
			Error			 WritePlaylist(const String &);
	};
};

BoCA_DEFINE_PLAYLIST_COMPONENT(M3UPlaylist)

BoCA_END_COMPONENT(M3UPlaylist)
