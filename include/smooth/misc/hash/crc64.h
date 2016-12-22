 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CRC64
#define H_OBJSMOOTH_CRC64

namespace smooth
{
	namespace Hash
	{
		class CRC64;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Hash
	{
		class SMOOTHAPI CRC64
		{
			private:
				static UnsignedInt64	 table[256];
				static Bool		 initialized;

				static Void		 InitTable();

							 CRC64();
							~CRC64();
			public:
				static UnsignedInt64	 Compute(const Buffer<UnsignedByte> &);
		};
	};
};

#endif
