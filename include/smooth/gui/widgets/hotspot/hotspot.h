 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_HOTSPOT_
#define _H_OBJSMOOTH_HOTSPOT_

namespace smooth
{
	namespace GUI
	{
		class Hotspot;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Hotspot : public Widget
		{
			public:
				static const Int	 classID;

							 Hotspot(const Point &, const Size &);
				virtual			~Hotspot();

				virtual Int		 Show();
				virtual Int		 Hide();

				virtual Int		 Activate();
				virtual Int		 Deactivate();
 		};
	};
};

#endif
