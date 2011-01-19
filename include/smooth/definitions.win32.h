 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DEFS_WIN32
#define H_OBJSMOOTH_DEFS_WIN32

#if defined SMOOTH_DLL
#	define SMOOTHAPI __declspec (dllexport)
#	define SMOOTHVAR extern __declspec (dllexport)
#elif defined SMOOTH_PLUGIN_DLL && defined SMOOTH_STATIC
#	define SMOOTHAPI
#	define SMOOTHVAR extern

#	define SMOOTH_PLUGIN_API __declspec (dllexport)
#	define SMOOTH_PLUGIN_VAR extern __declspec (dllexport)

#	define SMOOTH_DLL
#elif defined SMOOTH_STATIC
#	define SMOOTHAPI
#	define SMOOTHVAR extern

#	define SMOOTH_PLUGIN_API __declspec (dllimport)
#	define SMOOTH_PLUGIN_VAR __declspec (dllimport)

#	define SMOOTH_DLL
#elif defined SMOOTH_PLUGIN_DLL
#	define SMOOTHAPI __declspec (dllimport)
#	define SMOOTHVAR __declspec (dllimport)

#	define SMOOTH_PLUGIN_API __declspec (dllexport)
#	define SMOOTH_PLUGIN_VAR extern __declspec (dllexport)
#else
#	define SMOOTHAPI __declspec (dllimport)
#	define SMOOTHVAR __declspec (dllimport)

#	define SMOOTH_PLUGIN_API __declspec (dllimport)
#	define SMOOTH_PLUGIN_VAR __declspec (dllimport)
#endif

#endif
