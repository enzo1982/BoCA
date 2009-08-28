 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ARRAY
#define H_OBJSMOOTH_ARRAY

#include "arrayentry.h"
#include "arraybackend.h"

namespace smooth
{
	template <class t, class s = t> class Array : public ArrayBackend<s>
	{
		public:
					 Array()						{ }
			virtual		~Array()						{ }

			/* Methods for adding entries to the list.
			 */
			Int		 Add(const t &value)					{ return ArrayBackend<s>::Add((s) value); }
			Bool		 Add(const t &value, Int index)				{ return ArrayBackend<s>::Add((s) value, index); }

			/* Methods for inserting entries at defined positions.
			 */
			Int		 InsertAfter(Int index, const t &value)			{ return ArrayBackend<s>::InsertAfter(index, (s) value); }
			Bool		 InsertAfter(Int index, const t &value, Int nIndex)	{ return ArrayBackend<s>::InsertAfter(index, (s) value, nIndex); }
			Int		 InsertAtPos(Int pos, const t &value)			{ return ArrayBackend<s>::InsertAtPos(pos, (s) value); }
			Bool		 InsertAtPos(Int pos, const t &value, Int nIndex)	{ return ArrayBackend<s>::InsertAtPos(pos, (s) value, nIndex); }

			/* Getter and setter methods.
			 */
			const t		&Get(Int index) const					{ return (const t &) ArrayBackend<s>::Get(index); }
			Bool		 Set(Int index, const t &value)				{ return ArrayBackend<s>::Set(index, (s) value); }

			const t		&GetNth(Int n) const					{ return (const t &) ArrayBackend<s>::GetNth(n); }
			Bool		 SetNth(Int n, const t &value)				{ return ArrayBackend<s>::SetNth(n, (s) value); }

			/* Shortcut getter methods.
			 */
			const t		&GetFirst() const					{ return (const t &) ArrayBackend<s>::GetFirst(); }
			const t		&GetLast() const					{ return (const t &) ArrayBackend<s>::GetLast(); }
			const t		&GetNext() const					{ return (const t &) ArrayBackend<s>::GetNext(); }
			const t		&GetPrev() const					{ return (const t &) ArrayBackend<s>::GetPrev(); }

			/* Getter methods for non-const references.
			 */
			t		&GetReference(Int index)				{ return (t &) ArrayBackend<s>::GetReference(index); }
			t		&GetNthReference(Int n)					{ return (t &) ArrayBackend<s>::GetNthReference(n); }

			/* Locking and unlocking for synchronization.
			 */
			Int		 LockForRead() const					{ return ArrayBackend<s>::LockForRead(); }
			Int		 LockForWrite() const					{ return ArrayBackend<s>::LockForWrite(); }

			Int		 Unlock() const						{ return ArrayBackend<s>::Unlock(); }
	};
};

#endif
