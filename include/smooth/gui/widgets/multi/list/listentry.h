 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_LISTENTRY_
#define _H_OBJSMOOTH_LISTENTRY_

namespace smooth
{
	namespace GUI
	{
		class ListEntry;
		class Hotspot;
	};
};

#include "../../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ListEntry : public Widget
		{
			private:
				Bool				 marked;
				Bool				 selected;
			protected:
				Hotspot				*hotspot;
				Hotspot				*markHotspot;

				String				 GetNthTabText(Int);
			public:
				static const Int		 classID;

								 ListEntry(const String &);
				virtual				~ListEntry();

				virtual Int			 Paint(Int);
			accessors:
				Int				 SetMark(Bool);
				Bool				 IsMarked();

				Int				 Select();
				Int				 Deselect();
				Bool				 IsSelected();
			signals:
				static Signal2<Void, Int, Int>	 internalOnSelectEntry;

				Signal0<Void>			 onSelect;
				Signal0<Void>			 onDeselect;
			slots:
				Void				 InitDrag();

				Void				 OnToggleMark();
				Void				 OnSelectEntry();

				Void				 OnSelectOtherEntry(Int, Int);
		};
	};
};

#endif
