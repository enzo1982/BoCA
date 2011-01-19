 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
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
#include "misc/string.h"

namespace smooth
{
	SMOOTHVAR String	 szCmdLine;

	Void SMOOTHAPI	 Init();
	Void SMOOTHAPI	 Free();

#ifdef SMOOTH_DLL
	extern Bool	 initializing;

	Void	 GetColors();
	Void	 GetDefaultFont();
#endif
};

#endif
