 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_STATUSBAR
#define H_OBJSMOOTH_STATUSBAR

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
				static const Short	 classID;

							 Statusbar(const String &);
				virtual			~Statusbar();

				Int			 Paint(Int);
			accessors:
				Int			 SetText(const String &);
		};
	};
};

#endif
