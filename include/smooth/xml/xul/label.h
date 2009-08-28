 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XULLABEL
#define H_OBJSMOOTH_XULLABEL

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Label;
		};

		class Node;
	};

	namespace GUI
	{
		class Text;
	};
};

#include "widget.h"

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Label : public Widget
			{
				private:
					GUI::Text	*text;
				public:
							 Label(Node *);
					virtual		~Label();

					GUI::Widget	*GetWidget() const;
			};
		};
	};
};

#endif
