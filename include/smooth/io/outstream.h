 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_OUTSTREAM
#define H_OBJSMOOTH_OUTSTREAM

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

#define OutputNumberIntel OutputNumber

namespace smooth
{
	namespace IO
	{
		const Short	 OS_APPEND	= 0;
		const Short	 OS_REPLACE	= 1;

		class SMOOTHAPI OutStream : public Stream
		{
			friend class InStream;
			private:
				InStream		*inStream;

				Bool			 InitPBD		();
				Bool			 CompletePBD		();

				Bool			 WriteData		();
			public:
							 OutStream		(Int, Driver *);
							 OutStream		(Int, const String &, Int = OS_APPEND);
							 OutStream		(Int, FILE *);
							 OutStream		(Int, Void *, Long);
							 OutStream		(Int, InStream *);
				virtual			~OutStream		();

				Bool			 OutputNumber		(Long, Int);
				Bool			 OutputNumberRaw	(Long, Int);
				Bool			 OutputNumberPDP	(Long, Int);
				Bool			 OutputNumberPBD	(Long, Int);

				Bool			 OutputString		(const String &);
				Bool			 OutputLine		(const String &);
				Bool			 OutputData		(const Void *, Int);

				Bool			 SetPackageSize		(Int);

				Bool			 AddFilter		(Filter *);
				Bool			 RemoveFilter		(Filter *);

				Bool			 Close			();

				Bool			 Seek			(Int64);
				Bool			 RelSeek		(Int64);

				Bool			 Flush			();

		};
	};
};

#endif
