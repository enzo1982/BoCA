 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_IMAGELOADER
#define H_OBJSMOOTH_IMAGELOADER

namespace smooth
{
	namespace GUI
	{
		class ImageLoader;
	};
};

#include "../bitmap.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 IMAGE_FORMAT_PCI	= 0;
		const Int	 IMAGE_FORMAT_PNG	= 1;
		const Int	 IMAGE_FORMAT_JPEG	= 2;

		abstract class SMOOTHAPI ImageLoader
		{
			protected:
				String			 fileName;
				Buffer<UnsignedByte>	 buffer;

				Bool			 gotFileName;
				Bool			 gotBuffer;

				Bitmap			 bitmap;
			public:
							 ImageLoader(const String &);
							 ImageLoader(const Buffer<UnsignedByte> &);
				virtual			~ImageLoader();

				virtual const Bitmap	&Load() = 0;

				static Bitmap		 Load(const String &);
				static Bitmap		 Load(const Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
