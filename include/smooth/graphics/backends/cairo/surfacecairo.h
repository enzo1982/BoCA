 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SURFACECAIRO_
#define _H_OBJSMOOTH_SURFACECAIRO_

namespace smooth
{
	namespace GUI
	{
		class SurfaceCairo;
	};
};

#include "../surfacebackend.h"

#include <cairo/cairo.h>

namespace smooth
{
	namespace GUI
	{
		const Int	 SURFACE_CAIRO	= 3;

		class SurfaceCairo : public SurfaceBackend
		{
			private:
				static Int			 surfaceDPI;
			protected:
				Size				 allocSize;
#ifdef __WIN32__
				HWND				 window;
				HDC				 gdi_dc;

				Array<HDC>			 paint_contexts;
				Array<HBITMAP>			 paint_bitmaps;
#else
				Display				*display;
				Visual				*visual;

				Window				 window;
				XWindowAttributes		 windowAttributes;

				Array<Pixmap>			 paint_bitmaps;
#endif
				Array<Rect *, Void *>		 paint_rects;

				cairo_surface_t			*surface;
				cairo_t				*context;

				Array<cairo_surface_t *, Void *> cairo_surfaces;
				Array<cairo_t *, Void *>	 cairo_contexts;

				Void				 CreateCairoContext();
				Void				 DestroyCairoContext();
			public:
								 SurfaceCairo(Void * = NIL, const Size & = Size());
								~SurfaceCairo();

				Int				 SetSize(const Size &);
				const Size			&GetSize() const;

				Int				 PaintRect(const Rect &);

				Int				 StartPaint(const Rect &);
				Int				 EndPaint();

				Void				*GetSystemSurface() const;

				Int				 GetSurfaceDPI() const;

				Int				 SetPixel(const Point &, const Color &);

				Int				 Line(const Point &, const Point &, const Color &);
				Int				 Box(const Rect &, const Color &, Int, const Size &);

				Int				 SetText(const String &, const Rect &, const Font &, Bool);

				Int				 Gradient(const Rect &, const Color &, const Color &, Int);

				Int				 BlitFromBitmap(const Bitmap &, const Rect &, const Rect &);
				Int				 BlitToBitmap(const Rect &, const Bitmap &, const Rect &);
		};
	};
};

#endif
