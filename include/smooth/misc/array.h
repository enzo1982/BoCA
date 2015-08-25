 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../definitions.h"

#ifndef H_OBJSMOOTH_INDEX_ARRAY
#define H_OBJSMOOTH_INDEX_ARRAY

namespace smooth
{
	class IndexArray;
};

#include "../templates/buffer.h"
#include "../threads/rwlock.h"

namespace smooth
{
	class SMOOTHAPI IndexArray
	{
		protected:
			Int			 nOfEntries;
			Int			 greatestIndex;

			Bool			 sorted;

			mutable Int		 lastAccessedEntry;

			Buffer<Int>		 indices;

			mutable Bool		 lockingEnabled;
			mutable Threads::RWLock	*lock;

			Bool			 IndexAvailable(Int) const;
			Int			 GetEntryNumberByIndex(Int) const;
		public:
						 IndexArray();
			virtual			~IndexArray();

			Bool			 InsertAtPos(Int, Int);

			Bool			 Move(Int, Int);
			virtual Bool		 MoveNth(Int, Int);

			Bool			 Remove(Int);
			virtual Bool		 RemoveNth(Int);
			virtual Bool		 RemoveAll();

			inline Int		 Length() const		{ return nOfEntries; }

			Int			 GetNthIndex(Int) const;

			Bool			 EnableLocking() const;
			Bool			 DisableLocking() const;

			inline Void		 LockForRead() const	{ if (lockingEnabled) lock->LockForRead(); }
			inline Void		 LockForWrite() const	{ if (lockingEnabled) lock->LockForWrite(); }

			inline Void		 Unlock() const		{ if (lockingEnabled) lock->Release(); }
	};
};

#endif
