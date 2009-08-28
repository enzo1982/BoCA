 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
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
		const Int	 SC_NONE	= 0;
		const Int	 SC_ALT		= 1;
		const Int	 SC_CTRL	= 2;
		const Int	 SC_SHIFT	= 4;

		class SMOOTHAPI Shortcut : public Widget
		{
			protected:
				Int			 key;
				Int			 param;

				Widget			*ref;
			public:
				static const Int	 classID;

							 Shortcut(Int, Int, Widget * = NIL, Int = 0);
				virtual			~Shortcut();

				virtual Int		 Process(Int, Int, Int);

				virtual String		 ToString() const;
			accessors:
				Int			 SetShortcut(Int, Int, Widget * = NIL, Int = 0);

				Int			 GetKey();
			signals:
				Signal1<Void, Int>	 onKeyDown;
		};
	};
};

#endif
