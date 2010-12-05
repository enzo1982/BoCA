 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POINTER
#define H_OBJSMOOTH_POINTER

namespace smooth
{
	namespace GUI
	{
		class Window;
	};

	namespace Input
	{
		class Pointer;
	};
};

#include "../definitions.h"
#include "../graphics/forms/point.h"

namespace smooth
{
	namespace Input
	{
		class SMOOTHAPI Pointer
		{
			private:
				static GUI::Point	 mousePosition;

							 Pointer();
							 Pointer(const Pointer &);
			public:
				/* Cursor constants and functions.
				 */
				static const Int	 CursorArrow	= 0;
				static const Int	 CursorTextEdit	= 1;
				static const Int	 CursorHand	= 2;
				static const Int	 CursorHSize	= 3;
				static const Int	 CursorVSize	= 4;

				static Bool		 SetCursor(const GUI::Window *, Int);

				/* Pointer position functions.
				 */
				static const GUI::Point	&GetPosition();

				static Void		 UpdatePosition(Int, Int);
		};
	};
};

#endif
