 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SPLASHSCREEN
#define H_OBJSMOOTH_SPLASHSCREEN

namespace smooth
{
	namespace System
	{
		class Timer;
	};

	namespace GUI
	{
		namespace Dialogs
		{
			class SplashScreenApp;
		};

		class Window;
	};
};

#include "dialog.h"
#include "../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SplashScreen : public Dialog
			{
				private:
					Int		 time;
					Bitmap		 bitmap;

					Window		*splashscreen;
					System::Timer	*timer;
				slots:
					Void		 SplashPaintProc();
					Bool		 SplashKillProc();

					Void		 TimerProc();
				public:
							 SplashScreen(const Bitmap &, Int);
					virtual		~SplashScreen();

					const Error	&ShowDialog();
			};
		};
	};
};

#endif
