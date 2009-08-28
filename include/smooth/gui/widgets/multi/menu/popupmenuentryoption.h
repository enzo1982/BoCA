 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POPUPMENU_ENTRY_OPTION
#define H_OBJSMOOTH_POPUPMENU_ENTRY_OPTION

namespace smooth
{
	namespace GUI
	{
		class PopupMenuEntryOption;
	};
};

#include "popupmenuentry.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI PopupMenuEntryOption : public PopupMenuEntry
		{
			public:
				static const Int	 classID;

							 PopupMenuEntryOption(const String &, Int *, Int);
				virtual			~PopupMenuEntryOption();

				virtual Int		 Paint(Int);

				virtual Bool		 IsTypeCompatible(Int) const;
			slots:
				Void			 OnClickEntry();
		};
	};
};

#endif
