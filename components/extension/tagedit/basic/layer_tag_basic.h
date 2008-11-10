 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
			Text				*text_artist;
			EditBox				*edit_artist;

			Text				*text_title;
			EditBox				*edit_title;

			Text				*text_album;
			EditBox				*edit_album;

			Text				*text_comment;
			MultiEdit			*edit_comment;

			Image				*image_cover;

			Track				 track;

			Void				 LoadCoverImage();
			Void				 FreeCoverImage();
		signals:
			Signal1<Void, const Track &>	 onModifyTrack;
		slots:
			Void				 OnSelectTrack(const Track &);
			Void				 OnSelectNone();

			Void				 OnModifyTrack();
		public:
							 LayerTagBasic();
							~LayerTagBasic();
	};
};

#endif
