 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_LISTENTRYSEPARATOR_
#define _H_OBJSMOOTH_LISTENTRYSEPARATOR_

namespace smooth
{
	namespace GUI
	{
		class ListEntrySeparator;
	};
};

#include "listentry.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ListEntrySeparator : public ListEntry
		{
			public:
				static const Int	 classID;

							 ListEntrySeparator();
				virtual			~ListEntrySeparator();

				virtual Int		 Paint(Int);

				virtual String		 ToString() const	{ return "a ListEntrySeparator"; }

				virtual Bool		 IsTypeCompatible(Int) const;
		};
	};
};

#endif
