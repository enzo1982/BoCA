 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TREE
#define H_OBJSMOOTH_TREE

namespace smooth
{
	namespace GUI
	{
		class Tree;
	};
};

#include "../list/list.h"
#include "../list/listentry.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Tree : public ListEntry
		{
			protected:
				List				 list;
				Hotspot				*headHotspot;

				Void				 CalculateHeight();

				Void				 PaintText(const Color &, Bool);
			public:
				static const Int		 classID;

								 Tree(const String &);
				virtual				~Tree();

				ListEntry			*AddEntry(const String &entry)	{ return list.AddEntry(entry); }

				Int				 Add(Widget *);
				Int				 Remove(Widget *);

				Int				 RemoveAllEntries()		{ return list.RemoveAllEntries(); }

				Int				 Open()				{ return SetMark(True); }
				Int				 Close()			{ return SetMark(False); }

				virtual Int			 Paint(Int);

				virtual Bool			 IsTypeCompatible(Int) const;
			accessors:
				Int				 Length() const			{ return list.Length(); }
				ListEntry			*GetNthEntry(Int n) const	{ return list.GetNthEntry(n); };

				Int				 SelectEntry(ListEntry *entry)	{ return list.SelectEntry(entry); }
				ListEntry			*GetSelectedEntry() const	{ return list.GetSelectedEntry(); };

				Int				 SelectNthEntry(Int n)		{ return list.SelectNthEntry(n); }
				Int				 GetSelectedEntryNumber() const	{ return list.GetSelectedEntryNumber(); }
			signals:
				Signal1<Void, ListEntry *>	 onSelectEntry;

				Signal0<Void>			 onOpen;
				Signal0<Void>			 onClose;
			slots:
				Void				 OnChangeSize(const Size &);

				Void				 OnToggleMark(Bool);

				Void				 OnSelectEntry(Int, Int);

				Void				 OnMouseOver();
				Void				 OnMouseOut();
		};
	};
};

#endif
