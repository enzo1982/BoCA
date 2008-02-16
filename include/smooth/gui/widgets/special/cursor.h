 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_CURSOR_
#define _H_OBJSMOOTH_CURSOR_

namespace smooth
{
	namespace GUI
	{
		class Cursor;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 CF_MULTILINE	= 1;

		class SMOOTHAPI Cursor : public Widget
		{
			private:
				System::Timer			*timer;

				Int				 promptPos;
				Bool				 promptVisible;

				Bool				 marking;
				Int				 markStart;
				Int				 markEnd;

				Int				 visibleOffset;

				Int				 scrollPos;
				Int				 maxScrollPos;

				Array<Int>			 lineIndices;
			protected:
				Int				 maxSize;

				Void				 MarkText(Int, Int);

				Void				 InsertText(const String &);
				Void				 DeleteSelectedText();

				Int				 DrawWidget();
				Void				 ShowCursor(Bool);
			public:
				static const Int		 classID;

								 Cursor(const Point &, const Size &);
				virtual				~Cursor();

				virtual Int			 Paint(Int);
				virtual Int			 Process(Int, Int, Int);

				Int				 MarkAll();

				Int				 Scroll(Int);
			accessors:
				Int				 SetCursorPos(Int);
				Int				 GetCursorPos();

				Int				 SetMaxSize(Int);
				Int				 GetMaxSize();

				virtual Int			 SetText(const String &);
			signals:
				Signal1<Void, const String &>	 onInput;
				Signal1<Void, const String &>	 onEnter;

				Signal2<Void, Int, Int>		 onScroll;
			slots:
				Void				 OnTimer();

				Void				 OnGetFocus();
				Void				 OnGetFocusByKeyboard();
				Void				 OnLoseFocus();

				Void				 OnSpecialKey(Int);
				Void				 OnInput(Int, Int);
		};
	};
};

#endif
