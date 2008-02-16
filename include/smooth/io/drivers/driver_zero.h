 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DRIVER_ZERO_
#define _H_OBJSMOOTH_DRIVER_ZERO_

namespace smooth
{
	namespace IO
	{
		class DriverZero;
	};
};

#include "../../definitions.h"
#include "../driver.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverZero : public Driver
		{
			public:
				 DriverZero();
				~DriverZero();
		};
	};
};

#endif
