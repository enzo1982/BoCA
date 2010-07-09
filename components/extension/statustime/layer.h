 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_LENGTHSTATUS_LAYER
#define H_LENGTHSTATUS_LAYER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include "lengthdisplay.h"

namespace BoCA
{
	class LayerLengthStatus : public Layer
	{
		private:
			Array<Track>	 tracks;
			Array<Track>	 tracks_selected;
			Array<Track>	 tracks_unselected;

			LengthDisplay	*display_all;
			LengthDisplay	*display_selected;
			LengthDisplay	*display_unselected;

			Void		 UpdateLengthDisplays();

			const String	&GetTotalLengthString(const Array<Track> &);
		slots:
			Void		 OnApplicationAddTrack(const Track &);
			Void		 OnApplicationRemoveTrack(const Track &);
			Void		 OnApplicationMarkTrack(const Track &);
			Void		 OnApplicationUnmarkTrack(const Track &);

			Void		 OnApplicationRemoveAllTracks();
		public:
					 LayerLengthStatus();
					~LayerLengthStatus();
	};
};

#endif
