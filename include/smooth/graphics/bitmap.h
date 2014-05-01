 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BITMAP
#define H_OBJSMOOTH_BITMAP

namespace smooth
{
	namespace GUI
	{
		class Surface;
		class Bitmap;
		class BitmapBackend;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "forms/size.h"
#include "forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Bitmap
		{
			private:
				BitmapBackend		*backend;
			public:
							 Bitmap(Void * = NIL);
							 Bitmap(const Size &, Int = -1);
							 Bitmap(const int);
							 Bitmap(const Bitmap &);

							~Bitmap();

				Short			 GetBitmapType() const;

				const Size		&GetSize() const;
				Byte			 GetDepth() const;

				UnsignedByte		*GetBytes() const;
				Byte			 GetBitsPerPixel() const;
				Byte			 GetLineAlignment() const;

				Bool			 CreateBitmap(const Size &, Int = -1);
				Bool			 DeleteBitmap();

				Bool			 SetSystemBitmap(Void *);
				Void			*GetSystemBitmap() const;

				Int			 GrayscaleBitmap();
				Int			 InvertColors();
				Int			 ReplaceColor(const Color &, const Color &);
				Int			 SetBackgroundColor(const Color &);

				Bitmap			 Scale(const Size &) const;

				Int			 BlitFromSurface(Surface *, const Rect &, const Rect &);
				Int			 BlitToSurface(const Rect &, Surface *, const Rect &) const;

				Bool			 SetPixel(const Point &, const Color &);
				Color			 GetPixel(const Point &) const;

				Bitmap &operator	 =(const int);
				Bitmap &operator	 =(const Bitmap &);

				Bool operator		 ==(const int) const;
				Bool operator		 !=(const int) const;
		};
	};
};

#endif
