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
			Array<Track>		 tracks;
			Int64			 seconds;
			Int			 approx;
			Int			 unknown;

			Array<Track>		 tracks_selected;
			Int64			 seconds_selected;
			Int			 approx_selected;
			Int			 unknown_selected;

			Array<Track>		 tracks_unselected;
			Int64			 seconds_unselected;
			Int			 approx_unselected;
			Int			 unknown_unselected;

			LengthDisplay		*display_all;
			LengthDisplay		*display_selected;
			LengthDisplay		*display_unselected;

			Void			 UpdateLengthDisplays();

			Void			 AddTrack(const Track &, Int64 &, Int &, Int &);
			Void			 RemoveTrack(const Track &, Int64 &, Int &, Int &);

			const String		&GetLengthString(Int64, Int, Int);
		slots:
			Void			 OnApplicationAddTrack(const Track &);
			Void			 OnApplicationRemoveTrack(const Track &);
			Void			 OnApplicationMarkTrack(const Track &);
			Void			 OnApplicationUnmarkTrack(const Track &);

			Void			 OnApplicationRemoveAllTracks();
		public:
						 LayerLengthStatus();
						~LayerLengthStatus();
	};
};

#endif
