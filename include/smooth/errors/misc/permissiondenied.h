 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ERROR_PERMISSIONDENIED
#define H_OBJSMOOTH_ERROR_PERMISSIONDENIED

namespace smooth
{
	namespace Errors
	{
		class PermissionDenied;
	};
};

#include "../error.h"

namespace smooth
{
	namespace Errors
	{
		class SMOOTHAPI PermissionDenied : public Error
		{
			public:
						 PermissionDenied();

				virtual String	 ToString() const;
		};
	};
};

#endif
