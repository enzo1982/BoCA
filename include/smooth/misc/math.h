 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MATH_
#define _H_OBJSMOOTH_MATH_

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
			static Int64		 Round(Float);
			static Float		 Fract(Float);
			static Int64		 Floor(Float);
			static Int64		 Ceil(Float);
			static Float		 Mod(Float, Float);

			static Float		 Min(Float, Float);
			static Float		 Max(Float, Float);

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
			static Int		 Min(Int, Int);
			static Int		 Max(Int, Int);
			static Int		 Mod(Int, Int);
			static Float		 Pow(Int, Int);

			static Int64		 Abs(Int64);
			static Int64		 Min(Int64, Int64);
			static Int64		 Max(Int64, Int64);
			static Int64		 Mod(Int64, Int64);
			static Float		 Pow(Int64, Int64);
	};
};

#endif
