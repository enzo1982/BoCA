 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_XULWIDGET_
#define _H_OBJSMOOTH_XULWIDGET_

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Widget;
		};

		class Node;
	};
};

#include "../../gui/widgets/widget.h"

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class SMOOTHAPI Widget
			{
				protected:
					String			 id;

					Int			 orient;

					Int			 left;
					Int			 top;

					Int			 width;
					Int			 height;

					Int			 minwidth;
					Int			 minheight;

					Int			 maxwidth;
					Int			 maxheight;

					Int			 flex;

					Int			 defaultWidth;
					Int			 defaultHeight;

					String			 statustext;
					String			 tooltiptext;

					String			 GetXMLAttributeValue(Node *, const String &) const;
				public:
								 Widget(Node *);
					virtual			~Widget();

					virtual GUI::Widget	*GetWidget() const;

					Int			 SetMetrics(const GUI::Point &, const GUI::Size &);

					Int			 GetWidth() const		{ return width; }
					Int			 GetHeight() const		{ return height; }

					Int			 GetDefaultWidth() const	{ return defaultWidth; }
					Int			 GetDefaultHeight() const	{ return defaultHeight; }

					static const Int	 HORIZONTAL;
					static const Int	 VERTICAL;
			};
		};
	};
};

#endif
