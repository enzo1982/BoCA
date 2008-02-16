 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_COMBOBOX_
#define _H_OBJSMOOTH_COMBOBOX_

namespace smooth
{
	namespace GUI
	{
		class ComboBox;
		class ToolWindow;
		class ListBox;
		class Hotspot;
	};
};

#include "list.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 CB_NORMAL	= 0;
		const Int	 CB_HOTSPOTONLY	= 1;

		class SMOOTHAPI ComboBox : public List
		{
			private:
				ToolWindow		*toolWindow;
				ListBox			*listBox;

				ListEntry		*prevSelectedEntry;
			protected:
				Hotspot			*hotspot;
				Hotspot			*buttonHotspot;
			public:
				static const Int	 classID;

							 ComboBox(const Point &, const Size &);
				virtual			~ComboBox();

				virtual Int		 Paint(Int);
			slots:
				Void			 OpenListBox();
				Void			 CloseListBox();

				Void			 OnSelectEntry(ListEntry *);
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
