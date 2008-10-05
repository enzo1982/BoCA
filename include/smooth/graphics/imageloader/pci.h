 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_IMAGELOADER_PCI_
#define _H_OBJSMOOTH_IMAGELOADER_PCI_

namespace smooth
{
	namespace GUI
	{
		class ImageLoaderPCI;
	};
};

#include "imageloader.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ImageLoaderPCI : public ImageLoader
		{
			public:
						 ImageLoaderPCI(const String &);
						 ImageLoaderPCI(const Buffer<UnsignedByte> &);
						~ImageLoaderPCI();

				const Bitmap	&Load();
		};
	};
};

#endif
