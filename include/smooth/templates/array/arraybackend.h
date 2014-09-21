 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ARRAY_BACKEND
#define H_OBJSMOOTH_ARRAY_BACKEND

#include "arrayentry.h"
#include "../buffer.h"
#include "../../threads/rwlock.h"

#include <memory.h>
#include <string.h>

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

			mutable Bool		 lockingEnabled;
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

				if	(lastAccessedEntry < nOfEntries				  && entries[lastAccessedEntry    ]->GetIndex() == index) { Unlock(); return   lastAccessedEntry; }
				else if (lastAccessedEntry > 0 && lastAccessedEntry <= nOfEntries && entries[lastAccessedEntry - 1]->GetIndex() == index) { Unlock(); return --lastAccessedEntry; }
				else if (lastAccessedEntry + 1 < nOfEntries			  && entries[lastAccessedEntry + 1]->GetIndex() == index) { Unlock(); return ++lastAccessedEntry; }

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
				greatestIndex	  = 0;

				lastAccessedEntry = 0;

				lockingEnabled	  = False;
				lock		  = NIL;
			}

			ArrayBackend(const ArrayBackend<s> &oArray)
			{
				nOfEntries	  = 0;
				greatestIndex	  = 0;

				lastAccessedEntry = 0;

				lockingEnabled	  = False;
				lock		  = NIL;

				*this = oArray;
			}

			virtual	~ArrayBackend()
			{
				RemoveAll();

				if (lock != NIL)
				{
					delete lock;

					lock = NIL;
				}
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

				if (greatestIndex < index) greatestIndex = index;

				if (entries.Size() == nOfEntries) entries.Resize(8 > nOfEntries * 2 ? 8 : nOfEntries * 2);

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

				if (entries.Size() == nOfEntries) entries.Resize(8 > nOfEntries * 2 ? 8 : nOfEntries * 2);

				memmove(entries + position + 1, entries + position, (nOfEntries - position) * sizeof(ArrayEntry<s> *));

				ArrayEntry<s>	*entry = new ArrayEntry<s>(value);

				entry->SetIndex(index);

				entries[position] = entry;

				nOfEntries++;

				Unlock();

				return True;
			}

			Bool Switch(Int index1, Int index2)
			{
				if (index1 > greatestIndex ||
				    index2 > greatestIndex) return False;

				return SwitchNth(GetEntryNumberByIndex(index1), GetEntryNumberByIndex(index2));
			}

			Bool SwitchNth(Int n, Int m)
			{
				if (nOfEntries <= n || n < 0 ||
				    nOfEntries <= m || m < 0) return False;

				LockForWrite();

				ArrayEntry<s> *backup = entries[n];

				entries[n] = entries[m];
				entries[m] = backup;

				Unlock();

				return True;
			}

			Bool Remove(Int index)
			{
				if (index > greatestIndex) return False;

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
				if (nOfEntries == 0) return True;

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
					const s	&entry = entries[n]->GetValue();

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
					s	&entry = entries[n]->GetValueReference();

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
					const s	&entry = entries[n]->GetValueReference();

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
					Bool	 result = entries[n]->SetValue(value);

					lastAccessedEntry = n;

					Unlock();

					return result;
				}

				Unlock();

				return False;
			}

			Int GetNthIndex(Int n) const
			{
				LockForRead();

				if (nOfEntries > n && n >= 0)
				{
					Int	 index = entries[n]->GetIndex();

					Unlock();

					return index;
				}

				Unlock();

				return -1;
			}

			const s &GetFirst() const
			{
				LockForRead();

				if (nOfEntries > 0)
				{
					const s	&entry = entries[0]->GetValue();

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
					const s	&entry = entries[nOfEntries - 1]->GetValue();

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

				if (lastAccessedEntry < nOfEntries - 1)
				{
					const s	&entry = entries[++lastAccessedEntry]->GetValue();

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			const s &GetPrev() const
			{
				LockForRead();

				if (lastAccessedEntry > 0)
				{
					const s	&entry = entries[--lastAccessedEntry]->GetValue();

					Unlock();

					return entry;
				}

				Unlock();

				return nullValue;
			}

			inline Int Length() const
			{
				LockForRead();

				Int	 nOfEntriesTemp = nOfEntries;

				Unlock();

				return nOfEntriesTemp;
			}

			inline Bool EnableLocking() const
			{
				lockingEnabled = True;

				if (lock == NIL) lock = new Threads::RWLock();

				return True;
			}

			inline Bool DisableLocking() const
			{
				lockingEnabled = False;

				return True;
			}

			inline Void LockForRead() const
			{
				if (!lockingEnabled) return;

				lock->LockForRead();
			}

			inline Void LockForWrite() const
			{
				if (!lockingEnabled) return;

				lock->LockForWrite();
			}

			inline Void Unlock() const
			{
				if (!lockingEnabled) return;

				lock->Release();
			}
	};
};

template <class s> s S::ArrayBackend<s>::nullValue = ARRAY_NULLVALUE;

#endif
