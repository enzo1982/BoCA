 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SURFACEBACKEND
#define H_OBJSMOOTH_SURFACEBACKEND

namespace smooth
{
	namespace GUI
	{
		class SurfaceBackend;
	};
};

#include "../font.h"
#include "../color.h"
#include "../bitmap.h"
#include "../modifiers/fontsize.h"
#include "../modifiers/righttoleft.h"
#include "../../misc/string.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 SURFACE_NONE = 0;

		class SurfaceBackend
		{
			private:
				static SurfaceBackend	*(*backend_creator)(Void *, const Size &);
			protected:
				Short			 type;
				Short			 painting;

				Size			 size;
				Rect			 paintRect;

				FontSizeModifier	 fontSize;
				RightToLeftModifier	 rightToLeft;
			public:
				static Int		 SetBackend(SurfaceBackend *(*)(Void *, const Size &));

				static SurfaceBackend	*CreateBackendInstance(Void * = NIL, const Size & = Size());

							 SurfaceBackend(Void * = NIL, const Size & = Size());
				virtual			~SurfaceBackend();

				Short			 GetSurfaceType() const;

				virtual Int		 SetSize(const Size &);
				virtual const Size	&GetSize() const;

				Int			 SetRightToLeft(Bool);

				virtual Int		 PaintRect(const Rect &);

				virtual Int		 StartPaint(const Rect &);
				virtual Int		 EndPaint();

				virtual Void		*GetSystemSurface() const;

				virtual Short		 GetSurfaceDPI() const;

				virtual Int		 SetPixel(const Point &, const Color &);

				virtual Int		 Line(const Point &, const Point &, const Color &);
				virtual Int		 Frame(const Rect &, Short);
				virtual Int		 Box(const Rect &, const Color &, Int, const Size &);

				virtual Int		 SetText(const String &, const Rect &, const Font &, Bool);

				virtual Int		 Gradient(const Rect &, const Color &, const Color &, Int);
				virtual Int		 Bar(const Point &, const Point &, Int);

				virtual Int		 BlitFromBitmap(const Bitmap &, const Rect &, const Rect &);
				virtual Int		 BlitToBitmap(const Rect &, const Bitmap &, const Rect &);
		};
	};
};

#endif
