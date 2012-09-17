 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MATH
#define H_OBJSMOOTH_MATH

namespace smooth
{
	class Math;
};

#include "../definitions.h"

namespace smooth
{
	class SMOOTHAPI Math
	{
		private:
						 Math();
						 Math(const Math &);
		public:
			static const Float	 Pi;
			static const Float	 e;

			static Float		 Abs(Float);
			static Int64		 Sign(Float);
			static Int64		 Round(Float f)			{ return (Int64) (f + (f < 0 ? -0.5 : 0.5)); }
			static Float		 Fract(Float);
			static Int64		 Floor(Float);
			static Int64		 Ceil(Float);
			static Float		 Mod(Float, Float);

			static Float		 Min(Float f1, Float f2)	{ return (f1 < f2 ? f1 : f2); }
			static Float		 Max(Float f1, Float f2)	{ return (f1 > f2 ? f1 : f2); }

			static Float		 Pow(Float, Float);
			static Float		 Sqrt(Float);
			static Float		 Log(Float);
			static Float		 Log2(Float);
			static Float		 Log10(Float);
			static Float		 Exp(Float);

			static Float		 Sin(Float);
			static Float		 Cos(Float);
			static Float		 Tan(Float);

			static Float		 Sinh(Float);
			static Float		 Cosh(Float);
			static Float		 Tanh(Float);

			static Float		 Asin(Float);
			static Float		 Acos(Float);
			static Float		 Atan(Float);
			static Float		 Atan2(Float, Float);

			static Int		 Abs(Int);
			static Int		 Min(Int i1, Int i2)		{ return (i1 < i2 ? i1 : i2); }
			static Int		 Max(Int i1, Int i2)		{ return (i1 > i2 ? i1 : i2); }
			static Int		 Mod(Int, Int);
			static Float		 Pow(Int, Int);

			static Int64		 Abs(Int64);
			static Int64		 Min(Int64 i1, Int64 i2)	{ return (i1 < i2 ? i1 : i2); }
			static Int64		 Max(Int64 i1, Int64 i2)	{ return (i1 > i2 ? i1 : i2); }
			static Int64		 Mod(Int64, Int64);
			static Float		 Pow(Int64, Int64);
	};
};

#endif
