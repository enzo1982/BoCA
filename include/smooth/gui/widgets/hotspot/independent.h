 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_INDEPENDENTHOTSPOT_
#define _H_OBJSMOOTH_INDEPENDENTHOTSPOT_

namespace smooth
{
	namespace GUI
	{
		class IndependentHotspot;
	};
};

#include "hotspot.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI IndependentHotspot : public Hotspot
		{
			public:
				static const Int	 classID;

							 IndependentHotspot(const Point &, const Size &);
				virtual			~IndependentHotspot();
			accessors:
				virtual Rect		 GetVisibleArea() const;
		};
	};
};

#endif
