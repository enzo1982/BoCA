 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_NUMBER
#define H_OBJSMOOTH_NUMBER

namespace smooth
{
	class Number;
};

#include "../definitions.h"

namespace smooth
{
	class SMOOTHAPI Number
	{
		private:
			Int64			 intValue;
			Float			 floatValue;
		public:
						 Number(const Int64 = NIL);
						 Number(const Float);
						 Number(const Number &);

						~Number();

			static Number		 FromIntString(const String &);
			static Number		 FromFloatString(const String &);
			static Number		 FromHexString(const String &);

			String			 ToIntString() const;
			String			 ToFloatString() const;
			String			 ToHexString(Int = -1) const;

			operator		 Int64 () const;
			operator		 Float () const;

			Number &operator	 =(const Int64);
			Number &operator	 =(const Float);
			Number &operator	 =(const Number &);

			Bool operator		 ==(const Int64) const;
			Bool operator		 ==(const Float) const;
			Bool operator		 ==(const Number &) const;

			Bool operator		 !=(const Int64) const;
			Bool operator		 !=(const Float) const;
			Bool operator		 !=(const Number &) const;
	};
};

#endif
