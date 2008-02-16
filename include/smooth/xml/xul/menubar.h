 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_XULMENUBAR_
#define _H_OBJSMOOTH_XULMENUBAR_

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Menubar;
			class PopupMenu;
		};

		class Node;
	};

	namespace GUI
	{
		class Menubar;
	};
};

#include "widget.h"

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Menubar : public Widget
			{
				private:
					GUI::Menubar			*menubar;

					Array<PopupMenu *, Void *>	 entries;
				public:
									 Menubar(Node *);
					virtual				~Menubar();

					GUI::Widget			*GetWidget() const;
			};
		};
	};
};

#endif
