 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
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

#if defined __WIN32__
#	if defined SMOOTH_DLL && !defined WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif

#	include <winsock2.h>
#	include <windows.h>
#	include <wchar.h>

#	if defined SMOOTH_DLL
#		define SMOOTHAPI __declspec (dllexport)
#		define SMOOTHVAR extern __declspec (dllexport)
#	elif defined SMOOTH_PLUGIN_DLL && defined SMOOTH_STATIC
#		define SMOOTHAPI
#		define SMOOTHVAR extern

#		define SMOOTH_PLUGIN_API __declspec (dllexport)
#		define SMOOTH_PLUGIN_VAR extern __declspec (dllexport)

#		define SMOOTH_DLL
#	elif defined SMOOTH_STATIC
#		define SMOOTHAPI
#		define SMOOTHVAR extern

#		define SMOOTH_PLUGIN_API __declspec (dllimport)
#		define SMOOTH_PLUGIN_VAR __declspec (dllimport)

#		define SMOOTH_DLL
#	elif defined SMOOTH_PLUGIN_DLL
#		define SMOOTHAPI __declspec (dllimport)
#		define SMOOTHVAR __declspec (dllimport)

#		define SMOOTH_PLUGIN_API __declspec (dllexport)
#		define SMOOTH_PLUGIN_VAR extern __declspec (dllexport)
#	else
#		define SMOOTHAPI __declspec (dllimport)
#		define SMOOTHVAR __declspec (dllimport)

#		define SMOOTH_PLUGIN_API __declspec (dllimport)
#		define SMOOTH_PLUGIN_VAR __declspec (dllimport)
#	endif

#	if !defined WM_MOUSEWHEEL
#		define WM_MOUSEWHEEL 522
#	endif
#elif defined __WINE__
#	define __WIN32__

#	if defined SMOOTH_DLL && !defined WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif

#	include <winsock2.h>
#	include <windows.h>
#	include <wchar.h>

#	undef wchar_t
#	define wchar_t WCHAR

#	define SMOOTHAPI
#	define SMOOTHVAR extern

#	define SMOOTH_PLUGIN_API
#	define SMOOTH_PLUGIN_VAR extern

#	undef True
#	undef False
#	undef Bool
#	undef Success
#elif defined __QNX__
#	include <Ph.h>
#	include <Pt.h>
#	include <wchar.h>

#	define SMOOTHAPI
#	define SMOOTHVAR extern

#	define SMOOTH_PLUGIN_API
#	define SMOOTH_PLUGIN_VAR extern

#	define __declspec(x)
#else
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#	include <wchar.h>
#	include <linux/limits.h>

#	define SMOOTHAPI
#	define SMOOTHVAR extern

#	define SMOOTH_PLUGIN_API
#	define SMOOTH_PLUGIN_VAR extern

#	undef True
#	undef False
#	undef Bool
#	undef Success

#	define __declspec(x)

#	define MAX_PATH PATH_MAX

#	define VK_LEFT	  XK_Left
#	define VK_UP	  XK_Up
#	define VK_RIGHT	  XK_Right
#	define VK_DOWN	  XK_Down

#	define VK_HOME	  XK_Home
#	define VK_END	  XK_End
#	define VK_INSERT  XK_Insert
#	define VK_DELETE  XK_Delete
#	define VK_PRIOR	  XK_Prior
#	define VK_NEXT	  XK_Next

#	define VK_RETURN  XK_Return
#	define VK_BACK	  XK_BackSpace
#	define VK_TAB	  XK_Tab

#	define VK_SPACE	  XK_space

#	define VK_SHIFT	  XK_Shift_L
#	define VK_CONTROL XK_Control_L
#	define VK_ALT	  XK_Alt_L

#	define VK_ESCAPE  XK_Escape

#	define VK_F1	  XK_F1
#	define VK_F2	  XK_F2
#	define VK_F3	  XK_F3
#	define VK_F4	  XK_F4
#	define VK_F5	  XK_F5
#	define VK_F6	  XK_F6
#	define VK_F7	  XK_F7
#	define VK_F8	  XK_F8
#	define VK_F9	  XK_F9
#	define VK_F10	  XK_F10
#	define VK_F11	  XK_F11
#	define VK_F12	  XK_F12
#	define VK_F13	  XK_F13
#	define VK_F14	  XK_F14
#	define VK_F15	  XK_F15
#	define VK_F16	  XK_F16
#	define VK_F17	  XK_F17
#	define VK_F18	  XK_F18
#	define VK_F19	  XK_F19
#	define VK_F20	  XK_F20
#	define VK_F21	  XK_F21
#	define VK_F22	  XK_F22
#	define VK_F23	  XK_F23
#	define VK_F24	  XK_F24
#endif

#if defined SMOOTH_DLL
#	include <lisa.h>
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
	const Int Break		= 1;

	const Bool True		= (Bool) -1;
	const Bool False	= (Bool) 0;
};

#include "errors/error.h"
#include "errors/success.h"

#include "templates/array.h"
#include "misc/string.h"
#include "basic/setup.h"

#include "templates/pointer.h"

using namespace smooth::Errors;

namespace smooth
{
	Int	 Main();
	Int	 Main(const Array<String> &);

	Void	 AttachDLL(Void *);
	Void	 DetachDLL();
};

const S::Int SM_MOUSEMOVE		= 1024;
const S::Int SM_LBUTTONDOWN		= 1025;
const S::Int SM_LBUTTONUP		= 1026;
const S::Int SM_RBUTTONDOWN		= 1027;
const S::Int SM_RBUTTONUP		= 1028;
const S::Int SM_LBUTTONDBLCLK		= 1029;
const S::Int SM_RBUTTONDBLCLK		= 1030;

const S::Int SM_WINDOWTITLECHANGED	= 1031;
const S::Int SM_SHOWTOOLTIP		= 1032;
const S::Int SM_MOUSEWHEEL		= 1033;
const S::Int SM_PAINT			= 1034;

const S::Int SM_KEYDOWN			= 1035;
const S::Int SM_KEYUP			= 1036;
const S::Int SM_CHAR			= 1037;

const S::Int SM_TIMER			= 1534;

const S::Int OR_HORZ			= 1;
const S::Int OR_VERT			= 2;

const S::Int OR_TOP			= 4;
const S::Int OR_BOTTOM			= 8;

const S::Int OR_LEFT			= 16;
const S::Int OR_RIGHT			= 32;

const S::Int OR_CENTER			= 64;
const S::Int OR_FREE			= 128;

const S::Int OR_UPPERLEFT		= 20;
const S::Int OR_UPPERRIGHT		= 36;
const S::Int OR_LOWERLEFT		= 12;
const S::Int OR_LOWERRIGHT		= 40;

#endif
