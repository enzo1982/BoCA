 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_IMAGELOADER_
#define _H_OBJSMOOTH_IMAGELOADER_

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
		abstract class SMOOTHAPI ImageLoader
		{
			protected:
				String			 fileName;

				Bitmap			 bitmap;
			public:
							 ImageLoader(const String &);
				virtual			~ImageLoader();

				virtual const Bitmap	&Load() = 0;

				static Bitmap		 Load(const String &);
		};
	};
};

#endif
