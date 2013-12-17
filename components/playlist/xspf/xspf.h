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

BoCA_BEGIN_COMPONENT(PlaylistXSPF)

namespace BoCA
{
	class PlaylistXSPF : public CS::PlaylistComponent
	{
		friend class PlaylistXSPFCallback;

		private:
			String			 EncodeURI(const String &);
			String			 DecodeURI(const String &);
		public:
			static const String	&GetComponentSpecs();

						 PlaylistXSPF();
						~PlaylistXSPF();

			Bool			 CanOpenFile(const String &);

			const Array<Track>	&ReadPlaylist(const String &);
			Error			 WritePlaylist(const String &);
	};
};

BoCA_DEFINE_PLAYLIST_COMPONENT(PlaylistXSPF)

BoCA_END_COMPONENT(PlaylistXSPF)