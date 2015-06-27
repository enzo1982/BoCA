 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SHA1
#define H_OBJSMOOTH_SHA1

namespace smooth
{
	namespace Hash
	{
		class SHA1;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Hash
	{
		class SMOOTHAPI SHA1
		{
			private:
				UnsignedInt32		 state[5];

				UnsignedInt64		 size;
				Buffer<UnsignedByte>	 buffer;

				Void			 Transform(UnsignedByte *);
			public:
				static String		 Compute(const Buffer<UnsignedByte> &);

							 SHA1();
							~SHA1();

				Bool			 Reset();
				Bool			 Feed(const Buffer<UnsignedByte> &);
				String			 Finish();
		};
	};
};

#endif
