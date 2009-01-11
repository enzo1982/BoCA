 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MULTIEDIT_
#define _H_OBJSMOOTH_MULTIEDIT_

namespace smooth
{
	namespace GUI
	{
		class MultiEdit;
		class Scrollbar;
		class Cursor;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI MultiEdit : public Widget
		{
			protected:
				Cursor				*cursor;

				Scrollbar			*scrollbar;
				Int				 scrollbarPos;
			public:
				static const Int		 classID;

								 MultiEdit(const String &, const Point &, const Size &, Int = 0);
				virtual				~MultiEdit();

				virtual Int			 Paint(Int);

				virtual Int			 SetText(const String &);
				virtual const String		&GetText() const;

				Int				 GetCursorPos();

				Int				 MarkAll();
			signals:
				Signal1<Void, const String &>	 onInput;
			slots:
				Void				 OnScroll();
				Void				 OnCursorScroll(Int, Int);

				Void				 OnChangeSize(const Size &);

				Int				 GetNOfLines();
				Int				 GetNOfInvisibleLines();
		};
	};
};

#endif
