 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
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
				/* Delete all callers that have finished their work.
				 */
				for (Int i = 0; i < callers.Length(); i++)
				{
					Caller	*caller = callers.GetNth(i);

					if (!caller->IsActive())
					{
						delete callers.GetNth(i);

						callers.RemoveNth(i);

						i--;
					}
				}

				return Success();
			}

			virtual ~NonBlocking()
			{
			}
	};
};
