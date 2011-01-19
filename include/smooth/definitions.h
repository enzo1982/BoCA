 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
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
	enum Key
	{
		SK_OTHER	= 0,

		SK_BACK		= 0x08,
		SK_TAB,

		SK_RETURN	= 0x0D,

		SK_SHIFT	= 0x10,
		SK_CONTROL,
		SK_ALT,

		SK_ESCAPE	= 0x1B,

		SK_SPACE	= 0x20,

		SK_PRIOR,
		SK_NEXT,
		SK_END,
		SK_HOME,

		SK_LEFT,
		SK_UP,
		SK_RIGHT,
		SK_DOWN,

		SK_INSERT	= 0x2D,
		SK_DELETE,

		SK_0		= 0x30,
		SK_1,
		SK_2,
		SK_3,
		SK_4,
		SK_5,
		SK_6,
		SK_7,
		SK_8,
		SK_9,

		SK_A		= 0x41,
		SK_B,
		SK_C,
		SK_D,
		SK_E,
		SK_F,
		SK_G,
		SK_H,
		SK_I,
		SK_J,
		SK_K,
		SK_L,
		SK_M,
		SK_N,
		SK_O,
		SK_P,
		SK_Q,
		SK_R,
		SK_S,
		SK_T,
		SK_U,
		SK_V,
		SK_W,
		SK_X,
		SK_Y,
		SK_Z,

		SK_F1		= 0x70,
		SK_F2,
		SK_F3,
		SK_F4,
		SK_F5,
		SK_F6,
		SK_F7,
		SK_F8,
		SK_F9,
		SK_F10,
		SK_F11,
		SK_F12,
		SK_F13,
		SK_F14,
		SK_F15,
		SK_F16,
		SK_F17,
		SK_F18,
		SK_F19,
		SK_F20,
		SK_F21,
		SK_F22,
		SK_F23,
		SK_F24
	};

	enum Message
	{
		SM_MOUSEMOVE		= 1024,
		SM_LBUTTONDOWN,
		SM_LBUTTONUP,
		SM_RBUTTONDOWN,
		SM_RBUTTONUP,
		SM_LBUTTONDBLCLK,
		SM_RBUTTONDBLCLK,

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
