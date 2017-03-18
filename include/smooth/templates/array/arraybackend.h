 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ARRAY_BACKEND
#define H_OBJSMOOTH_ARRAY_BACKEND

#include "../../misc/array.h"

#include <memory.h>
#include <string.h>

#include <new>

namespace smooth
{
	template <class s> class ArrayEntry
	{
		public:
			s	 value;

				 ArrayEntry(const s &iValue) : value(iValue) { }
	};

	template <class s> class ArrayBackend : public IndexArray
	{
		private:
			static s		 nullValue;

			Buffer<ArrayEntry<s> >	 entries;
		public:
			ArrayBackend()
			{
			}

			ArrayBackend(const ArrayBackend<s> &oArray) : IndexArray()
			{
				*this = oArray;
			}

			virtual	~ArrayBackend()
			{
				RemoveAll();
			}

			ArrayBackend<s> &operator =(const ArrayBackend<s> &oArray)
			{
				if (&oArray == this) return *this;

				for (Int i = 0; i < oArray.Length(); i++) Add(oArray.GetNthReference(i), oArray.GetNthIndex(i));

				return *this;
			}

			Int Add(const s &value)
			{
				Int	 index = greatestIndex + 1;

				if (Add(value, index))	return index;
				else			return -1;
			}

			Bool Add(const s &value, Int index)
			{
				if (!IndexAvailable(index)) return False;

				LockForWrite();

				if (entries.Size() == nOfEntries) entries.Resize(8 > nOfEntries * 1.25 ? 8 : nOfEntries * 1.25);

				new (entries + nOfEntries) ArrayEntry<s>(value);

				IndexArray::InsertAtPos(nOfEntries, index);

				Unlock();

				return True;
			}

			Int InsertAfter(Int prevIndex, const s &value)
			{
				Int	 index = greatestIndex + 1;

				if (InsertAfter(prevIndex, value, index)) return index;
				else					  return -1;
			}

			Bool InsertAfter(Int prevIndex, const s &value, Int index)
			{
				return InsertAtPos(GetEntryNumberByIndex(prevIndex) + 1, value, index);
			}

			Int InsertAtPos(Int position, const s &value)
			{
				Int	 index = greatestIndex + 1;

				if (InsertAtPos(position, value, index)) return index;
				else					 return -1;
			}

			Bool InsertAtPos(Int position, const s &value, Int index)
			{
				if (position > nOfEntries || position < 0) return False;

				if (!IndexAvailable(index)) return False;

				LockForWrite();

				if (entries.Size() == nOfEntries) entries.Resize(8 > nOfEntries * 1.25 ? 8 : nOfEntries * 1.25);

				memmove((void *) (entries + position + 1), (void *) (entries + position), (nOfEntries - position) * sizeof(ArrayEntry<s>));

				new (entries + position) ArrayEntry<s>(value);

				IndexArray::InsertAtPos(position, index);

				Unlock();

				return True;
			}

			Bool MoveNth(Int n, Int m)
			{
				if (nOfEntries <= n || n < 0 ||
				    nOfEntries <= m || m < 0) return False;

				LockForWrite();

				UnsignedByte	 backupEntry[sizeof(ArrayEntry<s>)];

				memcpy(backupEntry, (void *) (entries + n), sizeof(ArrayEntry<s>));

				if (m < n) memmove((void *) (entries + m + 1), (void *) (entries + m), (n - m) * sizeof(ArrayEntry<s>));
				else	   memmove((void *) (entries + n), (void *) (entries + n + 1), (m - n) * sizeof(ArrayEntry<s>));

				memcpy((void *) (entries + m), backupEntry, sizeof(ArrayEntry<s>));

				IndexArray::MoveNth(n, m);

				Unlock();

				return True;
			}

			Bool RemoveNth(Int n)
			{
				if (nOfEntries <= n || n < 0) return False;

				if (nOfEntries == 1) return RemoveAll();

				LockForWrite();

				(entries + n)->~ArrayEntry<s>();

				memmove((void *) (entries + n), (void *) (entries + n + 1), (nOfEntries - n - 1) * sizeof(ArrayEntry<s>));

				IndexArray::RemoveNth(n);

				Unlock();

				return True;
			}

			Bool RemoveAll()
			{
				if (nOfEntries == 0) return True;

				LockForWrite();

				for (Int i = 0; i < nOfEntries; i++) (entries + i)->~ArrayEntry<s>();

				entries.Free();

				IndexArray::RemoveAll();

				Unlock();

				return True;
			}

			const s &Get(Int index) const
			{
				return GetNth(GetEntryNumberByIndex(index));
			}

			s &GetReference(Int index)
			{
				return GetNthReference(GetEntryNumberByIndex(index));
			}

			const s &GetReference(Int index) const
			{
				return GetNthReference(GetEntryNumberByIndex(index));
			}

			Bool Set(Int index, const s &value)
			{
				return SetNth(GetEntryNumberByIndex(index), value);
			}

			const s &GetNth(Int n) const
			{
				LockForRead();

				if (nOfEntries > n && n >= 0)
				{
					const s	&entry = (entries + n)->value;

					lastAccessedEntry = n;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			s &GetNthReference(Int n)
			{
				LockForRead();

				if (nOfEntries > n && n >= 0)
				{
					s	&entry = (entries + n)->value;

					lastAccessedEntry = n;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			const s &GetNthReference(Int n) const
			{
				LockForRead();

				if (nOfEntries > n && n >= 0)
				{
					const s	&entry = (entries + n)->value;

					lastAccessedEntry = n;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			Bool SetNth(Int n, const s &value)
			{
				LockForWrite();

				if (nOfEntries > n && n >= 0)
				{
					(entries + n)->value = value;

					lastAccessedEntry = n;

					Unlock();

					return True;
				}

				Unlock();

				return False;
			}

			const s &GetFirst() const
			{
				LockForRead();

				if (nOfEntries > 0)
				{
					const s	&entry = (entries + 0)->value;

					lastAccessedEntry = 0;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			const s &GetLast() const
			{
				LockForRead();

				if (nOfEntries > 0)
				{
					const s	&entry = (entries + nOfEntries - 1)->value;

					lastAccessedEntry = nOfEntries - 1;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			const s &GetNext() const
			{
				LockForRead();

				Int	 lastAccessed = lastAccessedEntry;

				if (lastAccessed < nOfEntries - 1)
				{
					const s	&entry = (entries + ++lastAccessed)->value;

					lastAccessedEntry = lastAccessed;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			const s &GetPrev() const
			{
				LockForRead();

				Int	 lastAccessed = lastAccessedEntry;

				if (lastAccessed > 0)
				{
					const s	&entry = (entries + --lastAccessed)->value;

					lastAccessedEntry = lastAccessed;

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}
	};
};

template <class s> s S::ArrayBackend<s>::nullValue = (s) 0;

#endif
