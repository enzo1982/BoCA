 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
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

				Error		 GetCPUID();
			public:
						 CPU();
			accessors:
				Int		 GetNumCores()		{ return numCores; }
				Int		 GetNumLogicalCPUs()	{ return numLogicalCPUs; }

				Bool		 HasMMX()		{ return hasMMX; }
				Bool		 HasMMXExt()		{ return hasMMXExt; }

				Bool		 HasSSE()		{ return hasSSE; }
				Bool		 HasSSE2()		{ return hasSSE2; }
				Bool		 HasSSE3()		{ return hasSSE3; }
				Bool		 HasSSSE3()		{ return hasSSSE3; }
				Bool		 HasSSE4_1()		{ return hasSSE4_1; }
				Bool		 HasSSE4_2()		{ return hasSSE4_2; }
				Bool		 HasSSE4a()		{ return hasSSE4a; }
				Bool		 HasSSE5()		{ return hasSSE5; }

				Bool		 HasAVX()		{ return hasAVX; }

				Bool		 Has3DNow()		{ return has3DNow; }
				Bool		 Has3DNowExt()		{ return has3DNowExt; }

				Bool		 HasHT()		{ return hasHT; }
				Bool		 HasNX()		{ return hasNX; }

				Bool		 HasVMX()		{ return hasVMX; }
				Bool		 HasSVM()		{ return hasSVM; }
		};
	};
};

#endif
