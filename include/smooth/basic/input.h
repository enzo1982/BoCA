 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_INPUT
#define H_OBJSMOOTH_INPUT

namespace smooth
{
	class Input;
};

#include "../definitions.h"
#include "../graphics/forms/point.h"

namespace smooth
{
	class SMOOTHAPI Input
	{
		private:
			static GUI::Point	 mousePosition;

						 Input();
						 Input(const Input &);
		public:
			static const GUI::Point	&GetMousePosition();
	};
};

#endif
