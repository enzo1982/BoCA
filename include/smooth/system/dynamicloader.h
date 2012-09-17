 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DYNAMICLOADER
#define H_OBJSMOOTH_DYNAMICLOADER

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
				Void			*handle;

				const Array<String>	&GetLibraryDirectories();
				Void			 ParseDirectoryList(const String &, Array<String> &);
			public:
				static const Short	 classID;

							 DynamicLoader(const String &);
				virtual			~DynamicLoader();

				Void		        *GetFunctionAddress(const String &) const;
			accessors:
				Void			*GetSystemModuleHandle() const;
		};
	};
};

#endif
