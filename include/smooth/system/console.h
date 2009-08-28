 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CONSOLE
#define H_OBJSMOOTH_CONSOLE

namespace smooth
{
	namespace System
	{
		class Console;
	};
};

#include "../definitions.h"
#include "../misc/string.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI Console
		{
			private:
						 Console();
						 Console(const Console &);
			public:
				static Int	 OutputString(const String &);
				static Int	 OutputLine(const String &);

				static Void	 WaitForKey();
			accessors:
				static Int	 SetTitle(const String &);
		};
	};
};

#endif
