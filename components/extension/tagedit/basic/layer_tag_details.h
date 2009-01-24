 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_LAYER_TAG_DETAILS_
#define _H_TAGEDIT_LAYER_TAG_DETAILS_

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class LayerTagDetails : public Layer
	{
		private:
			GroupBox			*group_details;

			Text				*text_band;
			EditBox				*edit_band;

			Text				*text_conductor;
			EditBox				*edit_conductor;

			Text				*text_remix;
			EditBox				*edit_remix;

			Text				*text_composer;
			EditBox				*edit_composer;

			Text				*text_textwriter;
			EditBox				*edit_textwriter;

			GroupBox			*group_publisher;

			Text				*text_publisher;
			EditBox				*edit_publisher;

			Text				*text_isrc;
			EditBox				*edit_isrc;

			GroupBox			*group_original;

			Text				*text_oartist;
			EditBox				*edit_oartist;

			Text				*text_oalbum;
			EditBox				*edit_oalbum;

			Text				*text_otextwriter;
			EditBox				*edit_otextwriter;

			Text				*text_oyear;
			EditBox				*edit_oyear;

			Track				 track;
		signals:
			Signal1<Void, const Track &>	 onModifyTrack;
		slots:
			Void				 OnChangeSize(const Size &);

			Void				 OnSelectTrack(const Track &);
			Void				 OnSelectNone();

			Void				 OnModifyTrack();
		public:
							 LayerTagDetails();
							~LayerTagDetails();
	};
};

#endif
