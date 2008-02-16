 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DRIVER_HTTPS_
#define _H_OBJSMOOTH_DRIVER_HTTPS_

namespace smooth
{
	namespace IO
	{
		class DriverANSI;
	};
};

#include "../../definitions.h"
#include "../driver.h"

#if !defined __WIN32__ || defined __CYGWIN32__
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <netdb.h>
#endif

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI DriverHTTPS : public Driver
		{
			private:
				unsigned int	 stream;
				Bool		 closeStream;

				Void		 CloseSocket();
			public:
						 DriverHTTPS(const String &, Int, const String &, Int, const String & = NIL, const String & = NIL);
						~DriverHTTPS();

				Int		 ReadData(UnsignedByte *, Int);
				Int		 WriteData(UnsignedByte *, Int);
		};
	};
};

#endif
