 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_LISTBOX_
#define _H_OBJSMOOTH_LISTBOX_

namespace smooth
{
	namespace GUI
	{
		class ListBox;
		class Scrollbar;
	};
};

#include "list.h"
#include "listboxheader.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 LF_NORMAL		= 0;
		const Int	 LF_HIDESCROLLBAR	= 1;
		const Int	 LF_ALLOWRESELECT	= 2;
		const Int	 LF_MULTICHECKBOX	= 4;
		const Int	 LF_HIDEHEADER		= 8;
		const Int	 LF_ALLOWREORDER	= 16;

		class SMOOTHAPI ListBox : public List
		{
			private:
				Scrollbar		*scrollbar;
				Int			 scrollbarPos;

				ListBoxHeader		*header;

				Int			 visibleEntriesChecksum;
			public:
				static const Int	 classID;

							 ListBox(const Point &, const Size &);
				virtual			~ListBox();

				virtual Int		 Paint(Int);

				Int			 AddTab(const String &tabName, Int tabWidth = 0, Int tabOrientation = OR_LEFT)	{ return header->AddTab(tabName, tabWidth, tabOrientation); }

				Int			 RemoveAllTabs()								{ return header->RemoveAllTabs(); }

				Int			 DragSelectedEntry(Bool);
			accessors:
				Int			 GetNOfTabs() const								{ return header->GetNOfTabs(); }
				Int			 GetNthTabOffset(Int n)	const							{ return header->GetNthTabOffset(n); }
				Int			 GetNthTabWidth(Int n) const							{ return header->GetNthTabWidth(n); }
				Int			 GetNthTabOrientation(Int n) const						{ return header->GetNthTabOrientation(n); }
			slots:
				Void			 OnScrollbarValueChange();
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
