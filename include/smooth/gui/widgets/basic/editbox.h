 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_EDITBOX
#define H_OBJSMOOTH_EDITBOX

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

#include "../special/cursor.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 EDB_ALPHANUMERIC	= 0;
		const Short	 EDB_NUMERIC		= 1;
		const Short	 EDB_ASTERISK		= 2;

		class SMOOTHAPI EditBox : public Widget
		{
			protected:
				Cursor				*cursor;

				List				*dropDownList;
				ComboBox			*comboBox;
			public:
				static const Short		 classID;

								 EditBox(const String &, const Point &, const Size &, Int = 0);
				virtual				~EditBox();

				virtual Int			 Paint(Int);

				virtual Int			 SetText(const String &nText)	{ return cursor->SetText(nText); }
				virtual const String		&GetText() const		{ return cursor->GetText(); }

				Int				 SetDropDownList(List *);

				Int				 MarkAll()			{ return cursor->MarkAll(); }
			accessors:
				Bool				 IsFocussed() const		{ return cursor->IsFocussed(); }

				Int				 SetCursorPos(Int nPos)		{ return cursor->SetCursorPos(nPos); }
				Int				 GetCursorPos() const		{ return cursor->GetCursorPos(); }

				Int				 GetUnscaledTextWidth() const	{ return cursor->GetUnscaledTextWidth(); }
				Int				 GetScaledTextWidth() const	{ return cursor->GetScaledTextHeight(); }

				Int				 GetUnscaledTextHeight() const	{ return cursor->GetUnscaledTextWidth(); }
				Int				 GetScaledTextHeight() const	{ return cursor->GetScaledTextHeight(); }

				const Size			&GetUnscaledTextSize() const	{ return cursor->GetUnscaledTextSize(); }
				const Size			&GetScaledTextSize() const	{ return cursor->GetScaledTextSize(); }
			signals:
				Signal1<Void, const String &>	 onInput;
				Signal1<Void, const String &>	 onEnter;

				Signal1<Void, ListEntry *>	 onSelectEntry;
			slots:
				Void				 OnSelectEntry(ListEntry *);

				Void				 OnChangeSize(const Size &);
		};
	};
};

#endif
