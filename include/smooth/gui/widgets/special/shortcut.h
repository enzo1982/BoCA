 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SHORTCUT
#define H_OBJSMOOTH_SHORTCUT

namespace smooth
{
	namespace GUI
	{
		class Shortcut;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 SC_NONE	= 0;
		const Short	 SC_ALT		= 1;
		const Short	 SC_CTRL	= 2;
		const Short	 SC_CMD		= 4;
		const Short	 SC_SHIFT	= 8;

#ifndef __APPLE__
		const Short	 SC_CONTROL	= SC_CTRL;
#else
		const Short	 SC_CONTROL	= SC_CMD;
#endif

		class SMOOTHAPI Shortcut : public Widget
		{
			protected:
				Int			 key;
				Int			 param;

				Widget			*ref;
			public:
				static const Short	 classID;

							 Shortcut(Int, Int, Widget * = NIL, Int = 0);
				virtual			~Shortcut();

				virtual Int		 Process(Int, Int, Int);

				virtual String		 ToString() const;
			accessors:
				Int			 SetShortcut(Int, Int, Widget * = NIL, Int = 0);

				Int			 GetKey() const	{ return key; }
			signals:
				Signal1<Void, Int>	 onKeyDown;
		};
	};
};

#endif
