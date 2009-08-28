 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../definitions.h"

#ifndef H_OBJSMOOTH_OBJECT
#define H_OBJSMOOTH_OBJECT

namespace smooth
{
	class Object;

	template <class t> class Pointer;
	template <class t> class PointerProxy;

	namespace System
	{
		class Timer;
	};

	namespace Threads
	{
		class Mutex;
	};
};

#include "objecttype.h"

namespace smooth
{
	abstract class SMOOTHAPI Object
	{
		friend class Signal;
		friend class Pointer<class t>;
		friend class PointerProxy<class t>;
		private:
			static Int			 nextClassID;
			static Int			 nextObjectHandle;

			static Array<Object *, Void *>	 objects;
			static Array<Object *, Void *>	 deleteable;

			static System::Timer		*cleanupTimer;

			Int				 handle;
			String				 name;

			Bool				 isDeleteable;
			mutable Int			 isObjectInUse;

			Int				 refCount;
			Threads::Mutex			*objMutex;
		protected:
			ObjectType			 type;

			Int				 flags;

			Int				 EnterProtectedRegion() const		{ return ++isObjectInUse; }
			Int				 LeaveProtectedRegion() const		{ return --isObjectInUse; }

			/* Called by DeleteObject when an object is
			 * prepared for deletion.
			 */
			virtual Void			 EnqueueForDeletion()			{ }
		public:
			static const Int		 classID;

			static Int			 RequestClassID();
			static Int			 RequestObjectHandle();

			static Int			 GetNOfObjects();
			static Object			*GetNthObject(Int);

			static Object			*GetObject(Int, Int = Object::classID);
			static Object			*GetObject(const String &);

			static Int			 DeleteObject(Object *);

							 Object();
			virtual				~Object();

			Int				 GetHandle() const			{ return handle; }

			Int				 SetName(const String &);
			const String			&GetName() const			{ return name; }

			Int				 SetFlags(Int nFlags)			{ flags = nFlags; return Errors::Success(); }
			Int				 GetFlags() const			{ return flags; }

			virtual String			 ToString() const			{ return "an Object"; }

			operator			 String() const				{ return ToString(); }

			const ObjectType		&GetObjectType() const			{ return type; }
			virtual inline Bool		 IsTypeCompatible(Int objType) const	{ return (objType == classID); }

			Bool				 IsObjectInUse() const			{ return isObjectInUse > 0; }
		slots:
			static Void			 ObjectCleanup();
	};
};

#endif
