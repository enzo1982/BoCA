 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../array.h"

namespace smooth
{
	class SMOOTHAPI NonBlocking
	{
		protected:
			static Array<Caller *, Void *>	 callers;
		public:
			static Int CleanUp()
			{
				for (Int i = 0; i < callers.Length(); i++) delete callers.GetNth(i);

				callers.RemoveAll();

				return Success();
			}
	};
};
