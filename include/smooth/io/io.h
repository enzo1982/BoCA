 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_IO_
#define _H_OBJSMOOTH_IO_

#include "../definitions.h"

namespace smooth
{
	namespace IO
	{
		const Int	 STREAM_NONE		= 0;
		const Int	 STREAM_STREAM		= 1;
		const Int	 STREAM_DRIVER		= 2;
		const Int	 STREAM_FILE		= 3;
		const Int	 STREAM_ANSI		= 4;
		const Int	 STREAM_BUFFER		= 5;

		const Int	 IO_ERROR_OK		= 0;	// everything went well
		const Int	 IO_ERROR_INTERNAL	= 1;	// an internal error
		const Int	 IO_ERROR_BADPARAM	= 2;	// a function has been called with a bad parameter
		const Int	 IO_ERROR_UNEXPECTED	= 3;	// some function returned an unexpected value
		const Int	 IO_ERROR_UNKNOWN	= 4;	// an unknown error occurred
		const Int	 IO_ERROR_NOTOPEN	= 5;	// the connected stream is not open or no stream is connected
		const Int	 IO_ERROR_OPNOTAVAIL	= 6;	// the requested operation is not available on the connected stream
		const Int	 IO_ERROR_WRONGOS	= 7;	// the requested functionality is not available under the current operating system
		const Int	 IO_ERROR_NODATA	= 8;	// no data is available in the queue
	};
};

#define IOGetByte(x, y) (y > 3)  ? 0 : ((x >> (8 * y)) & 255)
#define IOGetBit(x, y)  (y > 31) ? 0 : ((x >>      y ) & 1)

#endif
