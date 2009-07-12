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

#include "chooser/chooser.h"
#include "editor/editor.h"

namespace BoCA
{
	class LayerTags : public Layer
	{
		private:
			Array<Chooser *>		 choosers;
			Array<Editor *>			 editors;

			TabWidget			*tab_mode;
			TabWidget			*tab_editor;
		signals:
			Signal1<Void, const Track &>	 onSelectTrack;
			Signal1<Void, const Track &>	 onSelectAlbum;
			Signal0<Void>			 onSelectNone;
		slots:
			Void				 OnModifyTrack(const Track &);

			Void				 OnChangeSize(const Size &);
		public:
							 LayerTags();
							~LayerTags();
	};
};

#endif
