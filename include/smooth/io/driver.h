 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DRIVER
#define H_OBJSMOOTH_DRIVER

namespace smooth
{
	namespace IO
	{
		class Driver;
	};
};

#include "../definitions.h"
#include "io.h"

namespace smooth
{
	namespace IO
	{
		class SMOOTHAPI Driver
		{
			protected:
				Int64			 size;
				Int64			 position;

				Int			 lastError;

				String			 streamID;
			public:
							 Driver();
				virtual			~Driver();

				Int			 GetLastError() const;

				virtual Int		 ReadData(UnsignedByte *, Int);
				virtual Int		 WriteData(UnsignedByte *, Int);

				virtual Int64		 Seek(Int64);
				virtual Bool		 Flush();

				virtual Int64		 GetSize() const;
				virtual Int64		 GetPos() const;

				virtual const String	&GetStreamID() const;
		};
	};
};

#endif
