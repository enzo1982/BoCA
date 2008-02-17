 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TREEVIEW_
#define _H_OBJSMOOTH_TREEVIEW_

namespace smooth
{
	namespace GUI
	{
		class TreeView;
	};
};

#include "../list/listbox.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI TreeView : public ListBox
		{
			public:
				static const Int	 classID;

							 TreeView(const Point &, const Size &);
				virtual			~TreeView();

				virtual Bool		 IsTypeCompatible(Int) const;
		};
	};
};

#endif
