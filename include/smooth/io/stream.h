 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
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
		abstract class SMOOTHAPI Stream
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

				Bool			 bitstreamActive;
				Bool			 keepBits;
				Int			 bitLength;
				Bool			 bitBuffer[128];

				Driver			*driver;
				Filter			*filter;

				mutable Short		 lastError;
			public:
				static Bool		 SetDefaultPackageSize	(Int);

							 Stream();
				virtual			~Stream();

				virtual Bool		 SetFilter(Filter *) = 0;
				virtual Bool		 RemoveFilter()	     = 0;

				virtual Bool		 Close()	     = 0;

				virtual Bool		 Seek(Int64)	     = 0;
				virtual Bool		 RelSeek(Int64)	     = 0;
			accessors:
				Short			 GetStreamType() const	{ return streamType; }
				Short			 GetLastError() const	{ return lastError; }

				Int64			 Size() const		{ if (streamType == STREAM_NONE) { lastError = IO_ERROR_NOTOPEN; return -1; } return size; }
				Int64			 GetPos() const		{ if (streamType == STREAM_NONE) { lastError = IO_ERROR_NOTOPEN; return -1; } return currentFilePos; }

		};
	};
};

#endif
