 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_LISTBOXHEADER
#define H_OBJSMOOTH_LISTBOXHEADER

namespace smooth
{
	namespace GUI
	{
		class ListBoxHeader;
		class Hotspot;
	};
};

#include "../../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ListBoxHeader : public Widget
		{
			private:
				Int			 moveTab;
				Bool			 draggingTab;

				Point			 startMousePos;
			protected:
				Hotspot			*dragHotspot;

				Array<String>		 tabNames;
				Array<Int>		 tabWidths;
				Array<Int>		 tabOrientations;
				Array<Bool>		 tabChecked;
			public:
				static const Short	 classID;

							 ListBoxHeader(const Point &, const Size &);
				virtual			~ListBoxHeader();

				virtual Int		 EnableLocking(Bool = True);

				virtual Int		 Paint(Int);
				virtual Int		 Process(Int, Int, Int);

				Int			 AddTab(const String &, Int, Int);

				Int			 RemoveAllTabs();

				virtual String		 ToString() const		{ return "a ListBoxHeader"; }
			accessors:
				Int			 GetNOfTabs() const;
				Int			 GetNthTabOffset(Int) const;
				Int			 GetNthTabWidth(Int) const;
				Int			 GetNthTabOrientation(Int) const;
			slots:
				Void			 OnMouseDragStart(const Point &);
				Void			 OnMouseDrag(const Point &);
				Void			 OnMouseDragEnd(const Point &);

				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
