 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SURFACE
#define H_OBJSMOOTH_SURFACE

namespace smooth
{
	namespace GUI
	{
		class Surface;
		class SurfaceBackend;
	};

	namespace Threads
	{
		class Mutex;
	};
};

#include "../definitions.h"
#include "forms/rect.h"
#include "../misc/string.h"
#include "font.h"
#include "bitmap.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Surface
		{
			private:
				SurfaceBackend	*backend;
				Threads::Mutex	*mutex;
			public:
						 Surface(Void * = NIL, const Size & = Size());
				virtual		~Surface();

				Int		 GetSurfaceType() const;

				Int		 SetSize(const Size &);
				const Size	&GetSize() const;

				Int		 SetRightToLeft(Bool);

				Int		 PaintRect(const Rect &);

				Int		 StartPaint(const Rect &);
				Int		 EndPaint();

				Void		*GetSystemSurface() const;

				Int		 GetSurfaceDPI() const;

				Int		 SetPixel(const Point &, const Color &);

				Int		 Line(const Point &, const Point &, const Color &);
				Int		 Frame(const Rect &, Int);
				Int		 Box(const Rect &, const Color &, Int, const Size & = Size(0, 0));

				Int		 SetText(const String &, const Rect &, const Font &, Bool = False);

				Int		 Gradient(const Rect &, const Color &, const Color &, Int);
				Int		 Bar(const Point &, const Point &, Int);

				Int		 BlitFromBitmap(const Bitmap &, const Rect &, const Rect &);
				Int		 BlitToBitmap(const Rect &, const Bitmap &, const Rect &);
		};

		const Int	 FRAME_UP	= 0;
		const Int	 FRAME_DOWN	= 1;
	};
};

#endif
