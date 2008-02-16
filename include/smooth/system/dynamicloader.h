 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_DYNAMICLOADER_
#define _H_OBJSMOOTH_DYNAMICLOADER_

namespace smooth
{
	namespace System
	{
		class DynamicLoader;
	};
};

#include "../basic/object.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI DynamicLoader : public Object
		{
			private:
#ifdef __WIN32__
				HINSTANCE		 handle;
#else
				Void			*handle;
#endif
			public:
				static const Int	 classID;

							 DynamicLoader(const String &);
				virtual			~DynamicLoader();

				Void		        *GetFunctionAddress(const String &) const;
			accessors:
				Void			*GetSystemModuleHandle() const;
		};
	};
};

#endif
