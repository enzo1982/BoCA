 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BITMAPBACKEND
#define H_OBJSMOOTH_BITMAPBACKEND

namespace smooth
{
	namespace GUI
	{
		class Surface;
		class BitmapBackend;
	};
};

#include "../../definitions.h"
#include "../forms/size.h"
#include "../forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 BITMAP_NONE = 0;

		class BitmapBackend
		{
			private:
				static BitmapBackend		*(*backend_creator_pV)(Void *);
				static BitmapBackend		*(*backend_creator_III)(Int, Int, Int);
				static BitmapBackend		*(*backend_creator_cI)(const int);
				static BitmapBackend		*(*backend_creator_crS)(const BitmapBackend &);
			protected:
				Int				 type;

				Size				 size;
				Int				 depth;

				Void				*bytes;
				Int				 align;
			public:
				static Int			 SetBackend(BitmapBackend *(*)(Void *));
				static Int			 SetBackend(BitmapBackend *(*)(Int, Int, Int));
				static Int			 SetBackend(BitmapBackend *(*)(const int));
				static Int			 SetBackend(BitmapBackend *(*)(const BitmapBackend &));

				static BitmapBackend		*CreateBackendInstance(Void *);
				static BitmapBackend		*CreateBackendInstance(Int, Int, Int);
				static BitmapBackend		*CreateBackendInstance(const int);
				static BitmapBackend		*CreateBackendInstance(const BitmapBackend &);

								 BitmapBackend(Void * = NIL);
								 BitmapBackend(Int, Int, Int);
								 BitmapBackend(const int);
								 BitmapBackend(const BitmapBackend &);

				virtual				~BitmapBackend();

				Int				 GetBitmapType() const;

				const Size			&GetSize() const;
				Int				 GetDepth() const;

				UnsignedByte			*GetBytes() const;
				Int				 GetLineAlignment() const;

				virtual Bool			 CreateBitmap(Int, Int, Int);
				virtual Bool			 DeleteBitmap();

				virtual Bool			 SetSystemBitmap(Void *);
				virtual Void			*GetSystemBitmap() const;

				virtual Int			 GrayscaleBitmap();
				virtual Int			 InvertColors();
				virtual Int			 ReplaceColor(const Color &, const Color &);

				virtual Bool			 SetPixel(const Point &, const Color &);
				virtual Color			 GetPixel(const Point &) const;

				virtual BitmapBackend &operator	 =(const int);
				virtual BitmapBackend &operator	 =(const BitmapBackend &);

				virtual Bool operator		 ==(const int) const;
				virtual Bool operator		 !=(const int) const;
		};
	};
};

#endif
