 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ARRAY_ENTRY
#define H_OBJSMOOTH_ARRAY_ENTRY

#include "../../definitions.h"

namespace smooth
{
	template <class s> class ArrayEntry
	{
		private:
			s		 value;
			Int		 index;
		public:
					 ArrayEntry(const s &iValue)	{ value = iValue; index = 0; }
			virtual		~ArrayEntry()			{ }
		accessors:
			inline const s	&GetValue() const		{ return value; }
			Bool		 SetValue(const s &nValue)	{ value = nValue; return True; }

			inline Int	 GetIndex() const		{ return index; }
			Bool		 SetIndex(Int nIndex)		{ index = nIndex; return True; }

			inline s	&GetValueReference()		{ return value; }
	};
};

#endif
