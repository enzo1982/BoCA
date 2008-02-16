 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BASE64_
#define _H_OBJSMOOTH_BASE64_

namespace smooth
{
	namespace Encoding
	{
		class Base64;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Encoding
	{
		class SMOOTHAPI Base64
		{
			private:
				Buffer<UnsignedByte>	&buffer;
			public:
							 Base64(Buffer<UnsignedByte> &);
							~Base64();

				String			 Encode(Int = -1) const;
				Int			 Decode(const String &);
		};
	};
};

#endif
