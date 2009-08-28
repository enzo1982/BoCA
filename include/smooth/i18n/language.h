 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_I18N_LANGUAGE
#define H_OBJSMOOTH_I18N_LANGUAGE

#include "../misc/string.h"

namespace smooth
{
	namespace I18n
	{
		class SMOOTHAPI Language
		{
			public:
				Array<String>	 strings;

				String		 name;
				String		 encoding;
				String		 magic;
				String		 author;
				String		 url;
				Bool		 rightToLeft;

						 Language();
						~Language();
		};
	};
};

#endif
