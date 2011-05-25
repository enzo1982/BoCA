 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../definitions.h"
#include "../threads/access.h"

#ifndef H_OBJSMOOTH_OBJECT
#define H_OBJSMOOTH_OBJECT

namespace smooth
{
	class Object;

	namespace System
	{
		class Timer;
	};
};

#include "objecttype.h"

namespace smooth
{
	abstract class SMOOTHAPI Object
	{
		friend class Signal;

		private:
			static Short			 nextClassID;
			static Int			 nextObjectHandle;

			static Array<Object *, Void *>	 objects;
			static Array<Object *, Void *>	 deleteable;

			static System::Timer		*cleanupTimer;

			Int				 handle;
			String				 name;

			Bool				 lockingEnabled;

			Bool				 isDeleteable;
			mutable Int			 isObjectInUse;
		protected:
			ObjectType			 type;

			Int				 flags;

			Int				 EnterProtectedRegion() const		{ return Threads::Access::Increment(isObjectInUse); }
			Int				 LeaveProtectedRegion() const		{ return Threads::Access::Decrement(isObjectInUse); }

			/* Called by DeleteObject when an object is
			 * prepared for deletion.
			 */
			virtual Void			 EnqueueForDeletion()			{ }
		public:
			static const Short		 classID;

			static Short			 RequestClassID();
			static Int			 RequestObjectHandle();

			static Int			 GetNOfObjects();
			static Object			*GetNthObject(Int);

			static Object			*GetObject(Int, Short = Object::classID);
			static Object			*GetObject(const String &);

			static Int			 DeleteObject(Object *);

							 Object();
			virtual				~Object();

			virtual	Int			 EnableLocking(Bool = True);

			virtual String			 ToString() const			{ return "an Object"; }

			operator			 String() const				{ return ToString(); }
		accessors:
			Int				 GetHandle() const			{ return handle; }

			Int				 SetName(const String &);
			const String			&GetName() const			{ return name; }

			Int				 SetFlags(Int nFlags)			{ flags = nFlags; return Errors::Success(); }
			Int				 GetFlags() const			{ return flags; }

			Bool				 IsLockingEnabled() const		{ return lockingEnabled; }

			const ObjectType		&GetObjectType() const			{ return type; }
			virtual inline Bool		 IsTypeCompatible(Short objType) const	{ return (objType == classID); }

			Bool				 IsObjectInUse() const			{ return isObjectInUse > 0; }
		slots:
			static Void			 ObjectCleanup();
	};
};

#endif
