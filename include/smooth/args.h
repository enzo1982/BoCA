 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ARGS
#define H_OBJSMOOTH_ARGS

#include "main.h"
#include "misc/args.h"

namespace smooth
{
	static Int	 Main(const Array<String> &);
};

using namespace smooth;

Int smooth::Main()
{
	return Main(ArgumentsParser(szCmdLine).GetArguments());
}

#endif
