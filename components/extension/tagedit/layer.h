 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_LAYER_
#define _H_TAGEDIT_LAYER_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include "basic/layer_tag_basic.h"
#include "basic/layer_tag_details.h"

#include "advanced/layer_tag_advanced.h"

namespace BoCA
{
	class LayerTags : public Layer
	{
		private:
			Array<Track>			 tracks;

			Text				*text_tracks;
			ListBox				*list_tracks;

			TabWidget			*tab_mode;

			LayerTagBasic			*layer_basic;
			LayerTagDetails			*layer_details;

			LayerTagAdvanced		*layer_advanced;
		signals:
			Signal1<Void, const Track &>	 onSelectTrack;
			Signal0<Void>			 onSelectNone;
		slots:
			Void				 OnApplicationAddTrack(const Track &);
			Void				 OnApplicationModifyTrack(const Track &);
			Void				 OnApplicationRemoveTrack(const Track &);
			Void				 OnApplicationSelectTrack(const Track &);

			Void				 OnSelectTrack();
			Void				 OnModifyTrack(const Track &);

			Void				 OnChangeSize(const Size &);
		public:
							 LayerTags();
							~LayerTags();
	};
};

#endif
