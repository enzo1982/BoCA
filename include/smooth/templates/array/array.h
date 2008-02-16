 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_ARRAY_
#define _H_OBJSMOOTH_ARRAY_

#include "arrayentry.h"
#include "arraybackend.h"

namespace smooth
{
	template <class t, class s = t> class Array : public ArrayBackend<s>
	{
		public:
					 Array()						{ }
			virtual		~Array()						{ }

			Int		 Add(const t &value)					{ return ArrayBackend<s>::Add((s) value); }
			Bool		 Add(const t &value, Int index)				{ return ArrayBackend<s>::Add((s) value, index); }

			Int		 InsertAfter(Int index, const t &value)			{ return ArrayBackend<s>::InsertAfter(index, (s) value); }
			Bool		 InsertAfter(Int index, const t &value, Int nIndex)	{ return ArrayBackend<s>::InsertAfter(index, (s) value, nIndex); }
			Int		 InsertAtPos(Int pos, const t &value)			{ return ArrayBackend<s>::InsertAtPos(pos, (s) value); }
			Bool		 InsertAtPos(Int pos, const t &value, Int nIndex)	{ return ArrayBackend<s>::InsertAtPos(pos, (s) value, nIndex); }

			const t		&Get(Int index) const					{ return (const t &) ArrayBackend<s>::Get(index); }
			Bool		 Set(Int index, const t &value)				{ return ArrayBackend<s>::Set(index, (s) value); }

			const t		&GetNth(Int n) const					{ return (const t &) ArrayBackend<s>::GetNth(n); }
			Bool		 SetNth(Int n, const t &value)				{ return ArrayBackend<s>::SetNth(n, (s) value); }

			const t		&GetFirst() const					{ return (const t &) ArrayBackend<s>::GetFirst(); }
			const t		&GetLast() const					{ return (const t &) ArrayBackend<s>::GetLast(); }
			const t		&GetNext() const					{ return (const t &) ArrayBackend<s>::GetNext(); }
			const t		&GetPrev() const					{ return (const t &) ArrayBackend<s>::GetPrev(); }

			Int		 LockForRead() const					{ return ArrayBackend<s>::LockForRead(); }
			Int		 LockForWrite() const					{ return ArrayBackend<s>::LockForWrite(); }

			Int		 Unlock() const						{ return ArrayBackend<s>::Unlock(); }
	};
};

#endif
