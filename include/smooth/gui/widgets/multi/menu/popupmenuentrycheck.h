 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POPUPMENU_ENTRY_CHECK
#define H_OBJSMOOTH_POPUPMENU_ENTRY_CHECK

namespace smooth
{
	namespace GUI
	{
		class PopupMenuEntryCheck;
	};
};

#include "popupmenuentry.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI PopupMenuEntryCheck : public PopupMenuEntry
		{
			public:
				static const Short	 classID;

							 PopupMenuEntryCheck(const String &, Bool *);
				virtual			~PopupMenuEntryCheck();

				virtual Int		 Paint(Int);

				virtual Bool		 IsTypeCompatible(Short) const;
			slots:
				Void			 OnClickEntry();
		};
	};
};

#endif
