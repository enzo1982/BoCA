 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_VIDEOLIST
#define H_YOUTUBE_VIDEOLIST

#include <smooth.h>
#include <boca.h>

using namespace smooth;

using namespace BoCA;

namespace BoCA
{
	class VideoList : public List
	{
		private:
			Scrollbar	*scrollbar;
			Int		 scrollbarPos;

			Int		 visibleEntriesChecksum;

			Int		 GetEntriesWidth() const;
		public:
					 VideoList(const Point &, const Size &);
			virtual		~VideoList();

			virtual Int	 Paint(Int);

			Bool		 ScrollToEndOfList();
		accessors:
			Rect		 GetVisibleArea() const;
		slots:
			Void		 OnScrollbarValueChange();
			Void		 OnChangeSize(const Size &);
	};
};

#endif
