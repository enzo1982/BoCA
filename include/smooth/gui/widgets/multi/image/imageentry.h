 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_IMAGEENTRY
#define H_OBJSMOOTH_IMAGEENTRY

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
				static const Short	 classID;

							 ImageEntry(const Bitmap &, const Size &);
				virtual			~ImageEntry();

				virtual Int		 Paint(Int);

				virtual Bool		 IsTypeCompatible(Short) const;

				virtual String		 ToString() const				{ return "an ImageEntry"; }
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
