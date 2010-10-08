 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_STREAM
#define H_OBJSMOOTH_STREAM

namespace smooth
{
	namespace IO
	{
		class Stream;
		class Driver;
		class Filter;
	};
};

#include "../definitions.h"
#include "../templates/buffer.h"
#include "io.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI Stream
		{
			protected:
				static Int		 defaultPackageSize;

				Short			 streamType;
				Bool			 crosslinked;
				Bool			 closefile;

				Int64			 size;
				Int64			 origsize;

				Buffer<UnsignedByte>	 dataBuffer;

				Int			 packageSize;
				Int			 stdpacksize;
				Int			 origpacksize;
				Bool			 allowpackset;

				Int64			 currentFilePos;
				Int			 currentBufferPos;
				Int64			 origfilepos;

				Bool			 pbdActive;
				Bool			 keepPbd;
				Int			 pbdLength;
				Bool			 pbdBuffer[128];

				Driver			*driver;
				Array<Filter *, Void *>	 filters;

				mutable Short		 lastError;
			public:
				static Bool		 SetDefaultPackageSize	(Int);

				Int64			 Size() const		{ if (streamType == STREAM_NONE) { lastError = IO_ERROR_NOTOPEN; return -1; } return size; }
				Int64			 GetPos() const		{ if (streamType == STREAM_NONE) { lastError = IO_ERROR_NOTOPEN; return -1; } return currentFilePos; }

				Short			 GetStreamType() const	{ return streamType; }
				Short			 GetLastError() const	{ return lastError; }

							 Stream			();
							~Stream			();
		};
	};
};

#endif
