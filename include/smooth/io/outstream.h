 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_OUTSTREAM_
#define _H_OBJSMOOTH_OUTSTREAM_

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
		const Int	 OS_APPEND	= 0;
		const Int	 OS_OVERWRITE	= 1;

		class SMOOTHAPI OutStream : public Stream
		{
			friend class InStream;
			private:
				InStream		*inStream;

				Bool			 InitPBD		();
				Bool			 CompletePBD		();

				Bool			 WriteData		();
			public:
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

							 OutStream		(Int, Driver *);
							 OutStream		(Int, const String &, Int = OS_APPEND);
							 OutStream		(Int, FILE *);
							 OutStream		(Int, Void *, Long);
							 OutStream		(Int, InStream *);
							~OutStream		();
		};
	};
};

#endif
