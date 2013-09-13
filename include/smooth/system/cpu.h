 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CPU
#define H_OBJSMOOTH_CPU

namespace smooth
{
	namespace System
	{
		class CPU;
	};
};

#include "../definitions.h"
#include "../misc/string.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI CPU
		{
			private:
				static Int	 numCores;
				static Int	 numLogicalCPUs;

				static Bool	 hasMMX;
				static Bool	 hasMMXExt;

				static Bool	 hasSSE;
				static Bool	 hasSSE2;
				static Bool	 hasSSE3;
				static Bool	 hasSSSE3;
				static Bool	 hasSSE4_1;
				static Bool	 hasSSE4_2;
				static Bool	 hasSSE4a;
				static Bool	 hasSSE5;

				static Bool	 hasAVX;

				static Bool	 has3DNow;
				static Bool	 has3DNowExt;

				static Bool	 hasHT;
				static Bool	 hasNX;

				static Bool	 hasVMX;
				static Bool	 hasSVM;

				Error		 GetCPUID() const;
			public:
						 CPU();
			accessors:
				Int		 GetNumCores() const		{ return numCores; }
				Int		 GetNumLogicalCPUs() const	{ return numLogicalCPUs; }

				Bool		 HasMMX() const			{ return hasMMX; }
				Bool		 HasMMXExt() const		{ return hasMMXExt; }

				Bool		 HasSSE() const			{ return hasSSE; }
				Bool		 HasSSE2() const		{ return hasSSE2; }
				Bool		 HasSSE3() const		{ return hasSSE3; }
				Bool		 HasSSSE3() const		{ return hasSSSE3; }
				Bool		 HasSSE4_1() const		{ return hasSSE4_1; }
				Bool		 HasSSE4_2() const		{ return hasSSE4_2; }
				Bool		 HasSSE4a() const		{ return hasSSE4a; }
				Bool		 HasSSE5() const		{ return hasSSE5; }

				Bool		 HasAVX() const			{ return hasAVX; }

				Bool		 Has3DNow() const		{ return has3DNow; }
				Bool		 Has3DNowExt() const		{ return has3DNowExt; }

				Bool		 HasHT() const			{ return hasHT; }
				Bool		 HasNX() const			{ return hasNX; }

				Bool		 HasVMX() const			{ return hasVMX; }
				Bool		 HasSVM() const			{ return hasSVM; }
		};
	};
};

#endif
