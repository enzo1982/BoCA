 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FORM_
#define _H_OBJSMOOTH_FORM_

namespace smooth
{
	namespace GUI
	{
		class Form;
		class Surface;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Form
		{
			protected:
				Color		 color;
			public:
						 Form()				{ color = 0; }
				virtual		~Form()				{ }

				Int		 SetColor(const Color &nColor)	{ color = nColor; return Success(); }
				const Color	&GetColor() const		{ return color; }

				virtual Int	 Draw(GUI::Surface *);
		};
	};
};

#endif
