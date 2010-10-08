 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TIMERWIN32
#define H_OBJSMOOTH_TIMERWIN32

namespace smooth
{
	namespace System
	{
		class TimerWin32;
	};
};

#include "../timerbackend.h"

namespace smooth
{
	namespace System
	{
		const Short	 TIMER_WIN32 = 1;

		class TimerWin32 : public TimerBackend
		{
			private:
				Int			 timerid;

				static void WINAPI	 TimerProc(HWND, UINT, UINT_PTR, DWORD);
			public:
							 TimerWin32();
							~TimerWin32();

				Int			 Start(Int);
				Int			 Stop();

				Int			 GetID() const;
		};
	};
};

#endif
