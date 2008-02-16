 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_INSTREAM_
#define _H_OBJSMOOTH_INSTREAM_

namespace smooth
{
	namespace IO
	{
		class InStream;
		class OutStream;
		class Driver;
		class Filter;
	};
};

#include "../definitions.h"
#include "stream.h"

#define InputNumberIntel InputNumber

namespace smooth
{
	namespace IO
	{
		const Int	 IS_NORMAL	= 2;
		const Int	 IS_READONLY	= 3;

		class SMOOTHAPI InStream : public Stream
		{
			friend class OutStream;
			private:
				OutStream		*outStream;

				Bool			 InitPBD		();
				Bool			 CompletePBD		();

				Bool			 ReadData		();
			public:
				Long			 InputNumber		(Int);
				Long			 InputNumberRaw		(Int);
				Long			 InputNumberPDP		(Int);
				Long			 InputNumberPBD		(Int);

				String			 InputString		(Int);
				String			 InputLine		();
				Void			*InputData		(Void *, Int);

				Bool			 SetPackageSize		(Int);

				Bool			 AddFilter		(Filter *);
				Bool			 RemoveFilter		(Filter *);

				Bool			 Close			();

				Bool			 Seek			(Int64);
				Bool			 RelSeek		(Int64);

							 InStream		(Int, Driver *);
							 InStream		(Int, const String &, Int = IS_NORMAL);
							 InStream		(Int, FILE *);
							 InStream		(Int, Void *, Long);
							 InStream		(Int, OutStream *);
							~InStream		();
		};
	};
};

#endif
