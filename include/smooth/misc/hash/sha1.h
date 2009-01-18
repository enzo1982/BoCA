 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SHA1_
#define _H_OBJSMOOTH_SHA1_

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
				UnsignedInt32			 state[5];

				Void				 Init();
				Void				 Transform(UnsignedByte *);
				Void				 Final();

				const Buffer<UnsignedByte>	&buffer;
			public:
								 SHA1(const Buffer<UnsignedByte> &);
								~SHA1();

				String				 Compute();
		};
	};
};

#endif
