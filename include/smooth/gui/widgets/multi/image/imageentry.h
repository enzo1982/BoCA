 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_IMAGEENTRY_
#define _H_OBJSMOOTH_IMAGEENTRY_

namespace smooth
{
	namespace GUI
	{
		class Image;
		class ImageEntry;
	};
};

#include "../list/list.h"
#include "../list/listentry.h"
#include "../../../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ImageEntry : public ListEntry
		{
			protected:
				Image			*image;
			public:
				static const Int	 classID;

							 ImageEntry(const Bitmap &, const Size &);
				virtual			~ImageEntry();

				virtual Int		 Paint(Int);

				virtual Bool		 IsTypeCompatible(Int) const;

				virtual String		 ToString() const				{ return "an ImageEntry"; }
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
