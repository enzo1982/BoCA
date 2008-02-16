 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_XULRENDERER_
#define _H_OBJSMOOTH_XULRENDERER_

namespace smooth
{
	namespace XML
	{
		namespace XUL
		{
			class Renderer;
			class Window;
		};

		class Document;
		class Node;
	};

	namespace GUI
	{
		class Window;
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
			class SMOOTHAPI Renderer : public Widget
			{
				private:
					Window		*window;
				protected:
					Int		 RenderXUL(Document *);
				public:
							 Renderer();
					virtual		~Renderer();

					Int		 LoadXUL(const String &);
					Int		 CloseXUL();

					GUI::Widget	*GetWidget() const;
			};
		};
	};
};

#endif
