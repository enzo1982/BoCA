 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_I18N_CHARSET_DETECTOR
#define H_OBJSMOOTH_I18N_CHARSET_DETECTOR

#include "../definitions.h"

namespace smooth
{
	namespace I18n
	{
		class SMOOTHAPI CharsetDetector
		{
			public:
						 CharsetDetector();
				virtual		~CharsetDetector();

				String		 DetectCharset(const char *);
		};
	};
};

#endif
