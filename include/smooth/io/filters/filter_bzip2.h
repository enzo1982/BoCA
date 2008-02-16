 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FILTER_BZIP2_
#define _H_OBJSMOOTH_FILTER_BZIP2_

namespace smooth
{
	namespace IO
	{
		class FilterBZip2;
	};
};

#include "../../definitions.h"
#include "../filter.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI FilterBZip2 : public Filter
		{
			public:
					 FilterBZip2();
					~FilterBZip2();

				Int	 WriteData(Buffer<UnsignedByte> &, Int);
				Int	 ReadData(Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
