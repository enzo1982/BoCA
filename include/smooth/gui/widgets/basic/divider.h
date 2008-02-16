 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DIVIDER_
#define _H_OBJSMOOTH_DIVIDER_

namespace smooth
{
	namespace GUI
	{
		class Divider;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Divider : public Widget
		{
			protected:
				Int			 position;
			public:
				static const Int	 classID;

							 Divider(Int, Int = OR_HORZ | OR_TOP);
				virtual			~Divider();

				virtual Int		 Paint(Int);

				virtual Bool		 IsAffected(const Rect &) const;
			accessors:
				Int			 SetPos(Int);
				Int			 GetPos();
		};
	};
};

#endif
