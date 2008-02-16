 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_POINTER_
#define _H_OBJSMOOTH_POINTER_

#include "../../basic/object.h"
#include "proxy.h"

// TODO: Add checks for object == NIL to dereference operators

namespace smooth
{
	template <class t> class Pointer
	{
		private:
			Object				*object;

			Void				 CountUp()					{ if (object == NIL) return; object->refCount++; }
			Void				 CountDown()					{ if (object == NIL) return; object->refCount--; if (object->refCount == 0) Object::DeleteObject(object); }

		public:
							 Pointer()					{ object = NIL; }
			explicit			 Pointer(t *iObject)				{ object = iObject;	    CountUp(); }
							 Pointer(const Pointer &oPointer)		{ object = oPointer.object; CountUp(); }

							~Pointer()					{ CountDown(); }

			t				&operator  *() const				{ return (t &) *object; }
			PointerProxy<t>			 operator ->() const				{ return PointerProxy<t>((t *) object); }

			Pointer				&operator  =(const Pointer &oPointer)		{ CountDown(); object = oPointer.object; CountUp(); return *this; }

			friend Bool			 operator ==(const Pointer &l, const t *r)	{ return l.object == r; }
			friend Bool			 operator ==(const t *l, const Pointer &r)	{ return l == r.object; }
			friend Bool			 operator !=(const Pointer &l, const t *r)	{ return l.object != r; }
			friend Bool			 operator !=(const t *l, const Pointer &r)	{ return l != r.object; }

			template <class u> friend Bool	 operator ==(const Pointer &l, const u *r)	{ return l.object == r; }
			template <class u> friend Bool	 operator ==(const u *l, const Pointer &r)	{ return l == r.object; }
			template <class u> friend Bool	 operator !=(const Pointer &l, const u *r)	{ return l.object != r; }
			template <class u> friend Bool	 operator !=(const u *l, const Pointer &r)	{ return l != r.object; }

			template <class u> Bool		 operator ==(const Pointer<u> &r) const		{ return object == r.object; }
			template <class u> Bool		 operator !=(const Pointer<u> &r) const		{ return object != r.object; }
	};
};

#endif
