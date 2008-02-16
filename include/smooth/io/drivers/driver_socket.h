 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DRIVER_SOCKET_
#define _H_OBJSMOOTH_DRIVER_SOCKET_

namespace smooth
{
	namespace IO
	{
		class DriverSocket;
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
		class SMOOTHAPI DriverSocket : public Driver
		{
			private:
				unsigned int	 stream;
				Bool		 closeStream;

				UnsignedLong	 mode;
				Int		 timeout;

				Void		 CloseSocket();
			public:
						 DriverSocket(const String &, Int);
						 DriverSocket(unsigned int);
						~DriverSocket();

				Int		 ReadData(UnsignedByte *, Int);
				Int		 WriteData(UnsignedByte *, Int);

				Bool		 SetMode(Int);
				Bool		 SetTimeout(Int);
		};

		const UnsignedLong MODE_SOCKET_BLOCKING		= 0;
		const UnsignedLong MODE_SOCKET_NONBLOCKING	= 1;
	};
};

#endif
