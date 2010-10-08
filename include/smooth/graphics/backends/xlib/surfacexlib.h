 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SURFACEXLIB
#define H_OBJSMOOTH_SURFACEXLIB

namespace smooth
{
	namespace GUI
	{
		class SurfaceXLib;
	};
};

#include "../surfacebackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 SURFACE_XLIB	= 2;

		class SurfaceXLib : public SurfaceBackend
		{
			private:
				Display	*display;
			protected:
				Window	 window;
				Window	 rWindow;

				Pixmap	 bitmap;
			public:
					 SurfaceXLib(Void * = NIL, const Size & = Size());
					~SurfaceXLib();

				Int	 PaintRect(const Rect &);

				Int	 StartPaint(const Rect &);
				Int	 EndPaint();

				Void	*GetSystemSurface() const;

				Int	 SetPixel(const Point &, const Color &);

				Int	 Line(const Point &, const Point &, const Color &);
				Int	 Box(const Rect &, const Color &, Int, const Size &);

				Int	 SetText(const String &, const Rect &, const Font &, Bool);

				Int	 BlitFromBitmap(const Bitmap &, const Rect &, const Rect &);
				Int	 BlitToBitmap(const Rect &, const Bitmap &, const Rect &);
		};
	};
};

#endif
