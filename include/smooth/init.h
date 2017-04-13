 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_INIT
#define H_OBJSMOOTH_INIT

#include "definitions.h"

namespace smooth
{
	Bool SMOOTHAPI	 Init();
	Void SMOOTHAPI	 Free();

#ifdef SMOOTH_DLL
	extern Bool	 initializing;

	Int		 AddInitFunction(Int (*)());
	Int		 AddFreeFunction(Int (*)());

	Void		 GetColors();
#endif
};

#endif
