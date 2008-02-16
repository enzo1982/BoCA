 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DEFS_
#define _H_OBJSMOOTH_DEFS_

#ifdef __GNUC__
#	ifndef __int64
#		define __int64 long long
#	endif
#endif

#if defined __WIN32__
#	if defined __SMOOTH_DLL__ && !defined WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif

#	include <winsock2.h>
#	include <windows.h>
#	include <wchar.h>

#	if defined __SMOOTH_DLL__
#		define SMOOTHAPI __declspec (dllexport)
#		define SMOOTHVAR extern __declspec (dllexport)
#	elif defined __SMOOTH_PLUGIN_DLL__ && defined __SMOOTH_STATIC__
#		define SMOOTHAPI
#		define SMOOTHVAR extern

#		define SMOOTH_PLUGIN_API __declspec (dllexport)
#		define SMOOTH_PLUGIN_VAR extern __declspec (dllexport)

#		define __SMOOTH_DLL__
#	elif defined __SMOOTH_STATIC__
#		define SMOOTHAPI
#		define SMOOTHVAR extern

#		define SMOOTH_PLUGIN_API __declspec (dllimport)
#		define SMOOTH_PLUGIN_VAR __declspec (dllimport)

#		define __SMOOTH_DLL__
#	elif defined __SMOOTH_PLUGIN_DLL__
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

#	if defined __SMOOTH_DLL__ && !defined WIN32_LEAN_AND_MEAN
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
#endif

#if defined __SMOOTH_DLL__
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

const S::Int SM_EXECUTEPEEK		= 1534;
const S::Int SM_TIMER			= 1535;

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
