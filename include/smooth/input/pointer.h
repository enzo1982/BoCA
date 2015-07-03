 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
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
		class PointerBackend;
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
			enums:
				/* Cursor constants.
				 */
				enum CursorType
				{
					CursorArrow = 0,
					CursorTextEdit,
					CursorHand,
					CursorHSize,
					CursorVSize,

					NumCursorTypes
				};

			private:
				static PointerBackend		*backend;

				static const GUI::Window	*pointerWindow;
				static GUI::Point		 mousePosition;

								 Pointer();
								 Pointer(const Pointer &);
			public:
				static Int			 Initialize();
				static Int			 Free();

				/* Cursor functions.
				 */
				static Bool			 SetCursor(const GUI::Window *, CursorType);

				/* Pointer position functions.
				 */
				static const GUI::Point		&GetPosition();
				static const GUI::Window	*GetPointerWindow();

				static Void			 UpdatePosition(const GUI::Window *, Int, Int);
		};
	};
};

#endif
