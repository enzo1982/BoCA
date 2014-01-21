 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DEFS
#define H_OBJSMOOTH_DEFS

#ifdef __GNUC__
#	ifndef __int64
#		define __int64 long long
#	endif
#endif

#include <wchar.h>
#include <memory.h>
#include <stddef.h>

#if defined __WIN32__
#	include "definitions.win32.h"
#else
#	include "definitions.unix.h"
#endif

#define NIL	(0)

#define callbacks	public
#define signals		public
#define slots		public
#define accessors	public
#define constants	public

#define abstract
#define sealed

#include "types/generic.h"

#include "types/bool.h"
#include "types/float.h"
#include "types/int.h"
#include "types/void.h"

namespace S = smooth;

namespace smooth
{
	const Short	 Break	= 1;

	const Bool	 True	= (Bool) -1;
	const Bool	 False	= (Bool) 0;
};

#include "errors/error.h"
#include "errors/success.h"

#include "templates/array.h"
#include "misc/string.h"
#include "basic/setup.h"

using namespace smooth::Errors;

namespace smooth
{
	enum Messages
	{
		SM_MOUSEMOVE		= 1024,
		SM_LBUTTONDOWN,
		SM_LBUTTONUP,
		SM_RBUTTONDOWN,
		SM_RBUTTONUP,
		SM_MBUTTONDOWN,
		SM_MBUTTONUP,
		SM_LBUTTONDBLCLK,
		SM_RBUTTONDBLCLK,
		SM_MBUTTONDBLCLK,

		SM_WINDOWTITLECHANGED,
		SM_SHOWTOOLTIP,
		SM_MOUSEWHEEL,
		SM_PAINT,

		SM_KEYDOWN,
		SM_KEYUP,
		SM_CHAR,

		SM_WINDOWMETRICS,
		SM_GETFOCUS,
		SM_LOSEFOCUS,

		SM_DROPFILES,

		SM_TIMER		= 1534
	};
};

const S::Short	 OR_HORZ		= 1;
const S::Short	 OR_VERT		= 2;

const S::Short	 OR_TOP			= 4;
const S::Short	 OR_BOTTOM		= 8;

const S::Short	 OR_LEFT		= 16;
const S::Short	 OR_RIGHT		= 32;

const S::Short	 OR_CENTER		= 64;
const S::Short	 OR_FREE		= 128;

const S::Short	 OR_UPPERLEFT		= OR_TOP    | OR_LEFT;
const S::Short	 OR_UPPERRIGHT		= OR_TOP    | OR_RIGHT;
const S::Short	 OR_LOWERLEFT		= OR_BOTTOM | OR_LEFT;
const S::Short	 OR_LOWERRIGHT		= OR_BOTTOM | OR_RIGHT;

#endif
