 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XULLOADER
#define H_OBJSMOOTH_XULLOADER

namespace smooth
{
	namespace GUI
	{
		class XULLoader;
	};

	namespace XML
	{
		namespace XUL
		{
			class Renderer;
		};
	};
};

#include "application.h"

namespace smooth
{
	namespace GUI
	{
		class XULLoader : public Application
		{
			private:
				XML::XUL::Renderer	*xulRenderer;
			public:
							 XULLoader(const String &);
							~XULLoader();
		};

#ifdef __WIN32__
		extern "C"
		{
#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
			SMOOTHAPI int WINAPI LoadXUL(HWND, HINSTANCE, LPSTR, int) __attribute__((force_align_arg_pointer));
#else
			SMOOTHAPI int WINAPI LoadXUL(HWND, HINSTANCE, LPSTR, int);
#endif
		}
#endif
	};
};

#endif
