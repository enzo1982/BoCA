 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_ARRAY_BACKEND_
#define _H_OBJSMOOTH_ARRAY_BACKEND_

#include "arrayentry.h"
#include "../buffer.h"
#include "../../threads/rwlock.h"

namespace smooth
{
	template <class s> class ArrayBackend
	{
		private:
			static s		 nullValue;

			Int			 nOfEntries;
			Int			 greatestIndex;

			mutable Int		 lastAccessedEntry;

			Buffer<ArrayEntry<s> *>	 entries;

			mutable Threads::RWLock	*lock;

			Bool IndexAvailable(Int index) const
			{
				if (index > greatestIndex) return True;

				if (GetEntryNumberByIndex(index) == -1) return True;
				else					return False;
			}

			Int GetEntryNumberByIndex(Int index) const
			{
				if (nOfEntries == 0) return -1;

				LockForRead();

				if (lastAccessedEntry + 1 < nOfEntries)
				{
					if (entries[lastAccessedEntry + 1]->GetIndex() == index) { Unlock(); return ++lastAccessedEntry; }
				}

				if (lastAccessedEntry > 0)
				{
					if (entries[lastAccessedEntry - 1]->GetIndex() == index) { Unlock(); return --lastAccessedEntry; }
				}

				for (Int i = 0; i < nOfEntries; i++)
				{
					if (entries[i]->GetIndex() == index)
					{
						lastAccessedEntry = i;

						Unlock();

						return lastAccessedEntry;
					}
				}

				Unlock();

				return -1;
			}

		public:
			ArrayBackend()
			{
				nOfEntries	  = 0;
				lastAccessedEntry = 0;

				lock = NIL;
			}

			virtual	~ArrayBackend()
			{
				RemoveAll();

				if (lock != NIL) delete lock;
			}

			Int Add(const s &value)
			{
				Int	 index = ++greatestIndex;

				if (Add(value, index))	return index;
				else			return -1;
			}

			Bool Add(const s &value, Int index)
			{
				if (!IndexAvailable(index)) return False;

				LockForWrite();

				if (greatestIndex < index) greatestIndex = index;

				if (entries.Size() == nOfEntries) entries.Resize(nOfEntries + 10);

				ArrayEntry<s>	*entry = new ArrayEntry<s>(value);

				entry->SetIndex(index);

				entries[nOfEntries++] = entry;

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
				if (nOfEntries < position && position >= 0) return False;

				if (!IndexAvailable(index)) return False;

				LockForWrite();

				if (greatestIndex < index) greatestIndex = index;

				if (entries.Size() == nOfEntries) entries.Resize(nOfEntries + 10);

				memmove(entries + position + 1, entries + position, (nOfEntries - position) * sizeof(ArrayEntry<s> *));

				ArrayEntry<s>	*entry = new ArrayEntry<s>(value);

				entry->SetIndex(index);

				entries[position] = entry;

				nOfEntries++;

				Unlock();

				return True;
			}

			Bool Remove(Int index)
			{
				return RemoveNth(GetEntryNumberByIndex(index));
			}

			Bool RemoveNth(Int n)
			{
				if (nOfEntries <= n || n < 0) return False;

				LockForWrite();

				delete entries[n];

				memmove(entries + n, entries + n + 1, (nOfEntries - n - 1) * sizeof(ArrayEntry<s> *));

				nOfEntries--;

				Unlock();

				return True;
			}

			Bool RemoveAll()
			{
				LockForWrite();

				for (Int i = 0; i < nOfEntries; i++) delete entries[i];

				nOfEntries		= 0;
				greatestIndex	 	= 0;

				lastAccessedEntry	= 0;

				Unlock();

				return True;
			}

			const s &Get(Int index) const
			{
				return GetNth(GetEntryNumberByIndex(index));
			}

			Bool Set(Int index, const s &value)
			{
				return SetNth(GetEntryNumberByIndex(index), value);
			}

			const s &GetNth(Int n) const
			{
				if (nOfEntries > n && n >= 0)
				{
					lastAccessedEntry = n;

					return entries[n]->GetValue();
				}

				return nullValue;
			}

			Bool SetNth(Int n, const s &value)
			{
				if (nOfEntries > n && n >= 0)
				{
					lastAccessedEntry = n;

					return entries[n]->SetValue(value);
				}

				return False;
			}

			Int GetNthIndex(Int n) const
			{
				if (nOfEntries > n && n >= 0)
				{
					return entries[n]->GetIndex();
				}

				return -1;
			}

			const s &GetFirst() const
			{
				if (nOfEntries > 0)
				{
					lastAccessedEntry = 0;

					return entries[0]->GetValue();
				}

				return nullValue;
			}

			const s &GetLast() const
			{
				if (nOfEntries > 0)
				{
					lastAccessedEntry = nOfEntries - 1;

					return entries[nOfEntries - 1]->GetValue();
				}

				return nullValue;
			}

			const s &GetNext() const
			{
				if (lastAccessedEntry < nOfEntries - 1)
				{
					lastAccessedEntry++;

					return entries[lastAccessedEntry]->GetValue();
				}

				return nullValue;
			}

			const s &GetPrev() const
			{
				if (lastAccessedEntry > 0)
				{
					lastAccessedEntry--;

					return entries[lastAccessedEntry]->GetValue();
				}

				return nullValue;
			}

			inline Int Length() const
			{
				return nOfEntries;
			}

			Int LockForRead() const
			{
				if (lock == NIL) lock = new Threads::RWLock();

				return lock->LockForRead();
			}

			Int LockForWrite() const
			{
				if (lock == NIL) lock = new Threads::RWLock();

				return lock->LockForWrite();
			}

			Int Unlock() const
			{
				if (lock == NIL) return 0;

				return lock->Release();
			}
	};
};

template <class s> s S::ArrayBackend<s>::nullValue = ARRAY_NULLVALUE;

#endif
