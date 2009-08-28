 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CRC32
#define H_OBJSMOOTH_CRC32

namespace smooth
{
	namespace Hash
	{
		class CRC32;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Hash
	{
		class SMOOTHAPI CRC32
		{
			private:
				static Int			 table[256];
				static Bool			 initialized;

				static Void			 InitTable();
				static UnsignedLong		 Reflect(UnsignedLong, char);

				const Buffer<UnsignedByte>	&buffer;
			public:
								 CRC32(const Buffer<UnsignedByte> &);
								~CRC32();

				Int				 Compute() const;
		};
	};
};

#endif
