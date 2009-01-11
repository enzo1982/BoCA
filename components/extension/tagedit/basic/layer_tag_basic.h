 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_LAYER_TAG_BASIC_
#define _H_TAGEDIT_LAYER_TAG_BASIC_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class LayerTagBasic : public Layer
	{
		private:
			GroupBox			*group_info;

			Text				*text_artist;
			EditBox				*edit_artist;

			Text				*text_title;
			EditBox				*edit_title;

			Text				*text_album;
			EditBox				*edit_album;

			Text				*text_comment;
			MultiEdit			*edit_comment;

			GroupBox			*group_cover;

			ImageBox			*image_covers;
			Image				*image_cover_big;

			Button				*button_cover_load;
			Button				*button_cover_remove;

			ComboBox			*combo_cover_type;

			Track				 track;

			Void				 LoadCoverImages();
			Void				 FreeCoverImages();
		signals:
			Signal1<Void, const Track &>	 onModifyTrack;
		slots:
			Void				 OnChangeSize(const Size &);

			Void				 LoadCover();
			Void				 RemoveCover();

			Void				 SelectCover(ListEntry *);

			Void				 OnSelectTrack(const Track &);
			Void				 OnSelectNone();

			Void				 OnModifyTrack();
		public:
							 LayerTagBasic();
							~LayerTagBasic();
	};
};

#endif
