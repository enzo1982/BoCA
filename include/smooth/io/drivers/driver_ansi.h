 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DRIVER_ANSI_
#define _H_OBJSMOOTH_DRIVER_ANSI_

namespace smooth
{
	namespace IO
	{
		class DriverANSI;
	};
};

#include "../../definitions.h"
#include "../driver.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverANSI : public Driver
		{
			private:
				FILE	*stream;
				Bool	 closeStream;
			public:
					 DriverANSI(const String &, Int);
					 DriverANSI(FILE *);
					~DriverANSI();

				Int	 ReadData(UnsignedByte *, Int);
				Int	 WriteData(UnsignedByte *, Int);

				Int64	 Seek(Int64);

				Int64	 GetSize() const;
				Int64	 GetPos() const;
		};
	};
};

#endif
