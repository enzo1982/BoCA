 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_LAYER
#define H_OBJSMOOTH_LAYER

namespace smooth
{
	namespace GUI
	{
		class Layer;
	};
};

#include "widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Layer : public Widget
		{
			public:
				static const Int	 classID;

							 Layer(const String &name = NIL);
				virtual			~Layer();

				virtual Int		 Show();
				virtual Int		 Hide();

				virtual Int		 Paint(Int);

				virtual Int		 SetMetrics(const Point &, const Size &);
		};
	};
};

#endif
