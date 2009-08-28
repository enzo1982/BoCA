 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

 /* This code is derived from the RSA Data Security,
  * Inc. MD5 Message-Digest Algorithm. */

#ifndef H_OBJSMOOTH_MD5
#define H_OBJSMOOTH_MD5

namespace smooth
{
	namespace Hash
	{
		class MD5;
	};
};

#include "../string.h"
#include "../../templates/buffer.h"

namespace smooth
{
	namespace Hash
	{
		class SMOOTHAPI MD5
		{
			private:
				UnsignedInt32			 state[4];

				Void				 Init();
				Void				 Transform(UnsignedByte *);
				Void				 Final();

				const Buffer<UnsignedByte>	&buffer;
			public:
								 MD5(const Buffer<UnsignedByte> &);
								~MD5();

				String				 Compute();
		};
	};
};

#endif
