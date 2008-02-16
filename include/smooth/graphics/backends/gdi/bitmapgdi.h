 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BITMAPGDI_
#define _H_OBJSMOOTH_BITMAPGDI_

namespace smooth
{
	namespace GUI
	{
		class BitmapGDI;
	};
};

#include "../bitmapbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 BITMAP_GDI = 1;

		class BitmapGDI : public BitmapBackend
		{
			private:
				HBITMAP			 bitmap;
			public:
							 BitmapGDI(Void * = NIL);
							 BitmapGDI(Int, Int, Int);
							 BitmapGDI(const int);
							 BitmapGDI(const BitmapGDI &);

							~BitmapGDI();

				Bool			 CreateBitmap(Int, Int, Int);
				Bool			 DeleteBitmap();

				Bool			 SetSystemBitmap(Void *);
				Void			*GetSystemBitmap() const;

				BitmapBackend &operator	 =(const BitmapBackend &);

				Bool operator		 ==(const int) const;
				Bool operator		 !=(const int) const;
		};
	};
};

#endif
