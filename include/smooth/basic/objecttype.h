 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_OBJECTTYPE_
#define _H_OBJSMOOTH_OBJECTTYPE_

namespace smooth
{
	class Object;
	class ObjectType;
};

#include "../definitions.h"

namespace smooth
{
	class SMOOTHAPI ObjectType
	{
		private:
			Int			 type;
			Object			*object;
		public:
						 ObjectType(Object *);
						 ObjectType(const ObjectType &);

			operator		 Int() const		{ return type; }
			Int operator		 =(Int newType)		{ type = newType; return type; }

			Bool operator		 ==(Int) const;
			inline Bool operator	 !=(Int objType) const	{ return !(*this == objType); }
	};
};

#endif
