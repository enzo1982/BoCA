 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_INSTREAM
#define H_OBJSMOOTH_INSTREAM

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

#include <stdio.h>

#include "../definitions.h"
#include "stream.h"

#define InputNumberIntel InputNumber

namespace smooth
{
	namespace IO
	{
		const Short	 IS_READ	= 2;
		const Short	 IS_WRITE	= 4;

		class SMOOTHAPI InStream : public Stream
		{
			friend class OutStream;

			private:
				OutStream		*outStream;

				Bool			 InitBitstream		();
				Bool			 CompleteBitstream	();

				Bool			 ReadData		();
			public:
							 InStream		(Int, Driver *);
							 InStream		(Int, const String &, Int = IS_READ);
							 InStream		(Int, FILE *);
							 InStream		(Int, Void *, Long);
							 InStream		(Int, OutStream *);
				virtual			~InStream		();

				Int64			 InputNumber		(Int);
				Int64			 InputNumberRaw		(Int);

				Int64			 InputBits		(Int);

				String			 InputString		(Int);
				String			 InputLine		();
				Int			 InputData		(Void *, Int);

				Bool			 SetPackageSize		(Int);

				Bool			 AddFilter		(Filter *);
				Bool			 RemoveFilter		(Filter *);

				Bool			 Close			();

				Bool			 Seek			(Int64);
				Bool			 RelSeek		(Int64);
		};
	};
};

#endif
