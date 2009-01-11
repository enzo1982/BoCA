 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_EDITBOX_
#define _H_OBJSMOOTH_EDITBOX_

namespace smooth
{
	namespace GUI
	{
		class EditBox;
		class Cursor;
		class List;
		class ListEntry;
		class ComboBox;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		const Int EDB_ALPHANUMERIC	= 0;
		const Int EDB_NUMERIC		= 1;
		const Int EDB_ASTERISK		= 2;

		class SMOOTHAPI EditBox : public Widget
		{
			protected:
				Cursor				*cursor;

				List				*dropDownList;
				ComboBox			*comboBox;
			public:
				static const Int		 classID;

								 EditBox(const String &, const Point &, const Size &, Int = 0);
				virtual				~EditBox();

				virtual Int			 Paint(Int);

				virtual Int			 SetText(const String &);
				virtual const String		&GetText() const;

				Int				 SetDropDownList(List *);

				Int				 SetCursorPos(Int);
				Int				 GetCursorPos();

				Int				 MarkAll();
			signals:
				Signal1<Void, const String &>	 onInput;
				Signal1<Void, const String &>	 onEnter;
			slots:
				Void				 OnSelectListEntry(ListEntry *);

				Void				 OnChangeSize(const Size &);
		};
	};
};

#endif
