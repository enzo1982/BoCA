 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_IMAGELOADER_PNG
#define H_OBJSMOOTH_IMAGELOADER_PNG

namespace smooth
{
	namespace GUI
	{
		class ImageLoaderPNG;
	};
};

#include "imageloader.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ImageLoaderPNG : public ImageLoader
		{
			public:
						 ImageLoaderPNG(const String &);
						 ImageLoaderPNG(const Buffer<UnsignedByte> &);
						~ImageLoaderPNG();

				const Bitmap	&Load();
		};
	};
};

#endif
