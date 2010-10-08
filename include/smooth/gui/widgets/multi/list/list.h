 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_LIST
#define H_OBJSMOOTH_LIST

namespace smooth
{
	namespace GUI
	{
		class List;
	};
};

#include "listentry.h"
#include "listentryseparator.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI List : public Widget
		{
			private:
				Array<Bool>			 createdEntry;
				Array<ListEntry *, Void *>	 elementOrder;
			public:
				static const Short		 classID;

								 List();
				virtual				~List();

				ListEntry			*AddEntry(const String &);
				ListEntrySeparator		*AddSeparator();

				Int				 Add(Widget *);
				Int				 Remove(Widget *);

				Int				 RemoveAllEntries();

				Int				 SwitchEntries(Int, Int);

				virtual String			 ToString() const		{ return "a List"; }
			accessors:
				Int				 Length() const;

				ListEntry			*GetSelectedEntry() const;
				ListEntry			*GetEntry(const String &) const;
				ListEntry			*GetNthEntry(Int) const;

				Int				 GetSelectedEntryNumber() const;
				Int				 GetEntryNumber(const String &) const;

				Int				 SelectEntry(ListEntry *);
				Int				 SelectEntry(const String &);
				Int				 SelectNthEntry(Int);
			signals:
				Signal1<Void, ListEntry *>	 onSelectEntry;
				Signal1<Void, ListEntry *>	 onMarkEntry;

				Signal0<Void>			 onChangeEntryOrder;
		};
	};
};

#endif
