 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DEFINITIONS
#define H_BOCA_DEFINITIONS

#if defined __WIN32__
# if defined BOCA_CDK_BUILD
#  define BOCA_DLL_EXPORT __declspec (dllexport)
# else
#  define BOCA_DLL_EXPORT __declspec (dllimport)
# endif
#else
# define BOCA_DLL_EXPORT
#endif

#endif
