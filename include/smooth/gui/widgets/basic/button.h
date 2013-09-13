 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BUTTON
#define H_OBJSMOOTH_BUTTON

namespace smooth
{
	namespace GUI
	{
		class Button;
		class Hotspot;
	};
};

#include "../widget.h"
#include "../../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 BF_NORMAL	= 0;
		const Short	 BF_NOFRAME	= 1;

		class SMOOTHAPI Button : public Widget
		{
			protected:
				Bitmap			 bitmap;

				Hotspot			*hotspot;
			public:
				static const Short	 classID;

							 Button(const String &, const Bitmap &, const Point &, const Size &);
				virtual			~Button();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetBitmap(const Bitmap &);
				const Bitmap		&GetBitmap() const		{ return bitmap; }
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
