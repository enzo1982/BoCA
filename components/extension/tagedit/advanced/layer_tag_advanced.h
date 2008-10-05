 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_LAYER_TAG_ADVANCED_
#define _H_TAGEDIT_LAYER_TAG_ADVANCED_

#include <smooth.h>
#include <boca.h>

#include "../tagspec.h"

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class LayerTagAdvanced : public Layer
	{
		private:
			Text			*text_type;
			ComboBox		*combo_type;

			ListBox			*list_fields;

			Layer			*layer_field;

			Array<TagSpec *>	 tagTypes;

			Int			 LoadTagTypes();
			Int			 FreeTagTypes();
		public:
						 LayerTagAdvanced();
						~LayerTagAdvanced();
		slots:
			Void			 OnSelectTrack(const Track &);
			Void			 OnSelectNone();

			Void			 OnSelectTagType();
			Void			 OnSelectField();

			Void			 OnChangeSize(const Size &);
	};
};

#endif
