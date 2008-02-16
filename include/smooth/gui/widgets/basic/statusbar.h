 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_STATUSBAR_
#define _H_OBJSMOOTH_STATUSBAR_

namespace smooth
{
	namespace GUI
	{
		class Statusbar;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Statusbar : public Widget
		{
			public:
				static const Int	 classID;

							 Statusbar(const String &);
				virtual			~Statusbar();

				Int			 Paint(Int);
			accessors:
				Int			 SetText(const String &);
		};
	};
};

#endif
