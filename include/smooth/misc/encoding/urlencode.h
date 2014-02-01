 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_URLENCODE
#define H_OBJSMOOTH_URLENCODE

namespace smooth
{
	namespace Encoding
	{
		class URLEncode;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Encoding
	{
		class SMOOTHAPI URLEncode
		{
			public:
				static String	 Encode(const String &, Bool = True);
				static String	 Decode(const String &);
		};
	};
};

#endif
