 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DROPAREA
#define H_OBJSMOOTH_DROPAREA

namespace smooth
{
	namespace GUI
	{
		class DropArea;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI DropArea : public Widget
		{
			public:
				static const Short			 classID;

									 DropArea(const Point &, const Size &);
				virtual					~DropArea();

				virtual Int				 Hide();

				virtual Int				 Process(Int, Int, Int);
			signals:
				Signal1<Void, const String &>		 onDropFile;
				Signal1<Void, const Array<String> &>	 onDropFiles;
		};
	};
};

#endif
