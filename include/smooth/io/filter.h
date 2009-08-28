 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FILTER
#define H_OBJSMOOTH_FILTER

namespace smooth
{
	namespace IO
	{
		class Filter;
		class Driver;
	};
};

#include "../definitions.h"
#include "../templates/buffer.h"
#include "io.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI Filter
		{
			protected:
				Int		 packageSize;

				Driver		*driver;
			public:
						 Filter();
				virtual		~Filter();

				Int		 SetPackageSize(Int nPackageSize)	{ packageSize = nPackageSize; return Success(); }
				Int		 GetPackageSize() const			{ return packageSize; }

				Int		 SetDriver(Driver *nDriver)		{ driver = nDriver; return Success(); }
				Driver		*GetDriver() const			{ return driver; }

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 WriteData(Buffer<UnsignedByte> &, Int);
				virtual Int	 ReadData(Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
