 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_HOTSPOTSIMPLEBUTTON
#define H_OBJSMOOTH_HOTSPOTSIMPLEBUTTON

namespace smooth
{
	namespace GUI
	{
		class HotspotSimpleButton;
	};
};

#include "hotspot.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI HotspotSimpleButton : public Hotspot
		{
			public:
				static const Short	 classID;

							 HotspotSimpleButton(const Point &, const Size &);
				virtual			~HotspotSimpleButton();

				virtual Int		 Paint(Int);
		};
	};
};

#endif
