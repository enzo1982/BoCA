 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BUTTON_
#define _H_OBJSMOOTH_BUTTON_

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
		const Int	 BF_NORMAL	= 0;
		const Int	 BF_NOFRAME	= 1;

		class SMOOTHAPI Button : public Widget
		{
			protected:
				Bitmap			 bitmap;

				Hotspot			*hotspot;
			public:
				static const Int	 classID;

							 Button(const String &, const Bitmap &, const Point &, const Size &);
				virtual			~Button();

				virtual Int		 Paint(Int);
			accessors:
				const Bitmap		&GetBitmap();
				Int			 SetBitmap(const Bitmap &);
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
