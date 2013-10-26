 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_COMBOBOX
#define H_OBJSMOOTH_COMBOBOX

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
		const Short	 CB_NORMAL	= 0;
		const Short	 CB_HOTSPOTONLY	= 1;

		class SMOOTHAPI ComboBox : public List
		{
			private:
				ToolWindow		*toolWindow;
				ListBox			*listBox;

				UnsignedInt64		 listBoxClosed;

				ListEntry		*prevSelectedEntry;
			protected:
				Hotspot			*hotspot;
				Hotspot			*buttonHotspot;
			public:
				static const Short	 classID;

							 ComboBox(const Point &, const Size &);
				virtual			~ComboBox();

				virtual Int		 Paint(Int);

				virtual String		 ToString() const		{ return "a ComboBox"; }
			slots:
				Void			 OpenListBox();
				Void			 CloseListBox();

				Void			 OnSelectEntry(ListEntry *);
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
