 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_IMAGEBOX
#define H_OBJSMOOTH_IMAGEBOX

namespace smooth
{
	namespace GUI
	{
		class ImageBox;
		class Scrollbar;
	};
};

#include "../list/list.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ImageBox : public List
		{
			private:
				Scrollbar		*scrollbar;
				Int			 scrollbarPos;

				Int			 visibleEntriesChecksum;

				Int			 GetEntriesWidth() const;
			public:
				static const Int	 classID;

							 ImageBox(const Point &, const Size &);
				virtual			~ImageBox();

				virtual Int		 Paint(Int);

				virtual String		 ToString() const				{ return "an ImageBox"; }
			accessors:
				Rect			 GetVisibleArea() const;
			slots:
				Void			 OnScrollbarValueChange();
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
