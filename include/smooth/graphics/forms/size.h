 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SIZE_
#define _H_OBJSMOOTH_SIZE_

namespace smooth
{
	namespace GUI
	{
		class Size;
	};
};

#include "form.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Size : public Form
		{
			public:
#ifdef __WIN32__
				operator	 SIZE() const;
				Size &operator	 =(const SIZE &);
#endif
				Int		 cx;
				Int		 cy;

						 Size()			{ cx = 0; cy = 0; }
						 Size(Int iCX, Int iCY)	{ cx = iCX; cy = iCY; }

				Size operator	 +(const Size &) const;
				Size operator	 -(const Size &) const;

				Size operator	 *(const Float) const;
				Size operator	 /(const Float) const;

				Bool operator	 ==(const Size &) const;
				Bool operator	 !=(const Size &) const;
		};
	};
};

#endif
