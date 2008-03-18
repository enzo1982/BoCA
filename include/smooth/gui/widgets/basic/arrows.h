 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_ARROWS_
#define _H_OBJSMOOTH_ARROWS_

namespace smooth
{
	namespace GUI
	{
		class Arrows;
		class Hotspot;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Arrows : public Widget
		{
			private:
				Int			 dummyVariable;

				System::Timer		*timer;
				Int			 timerCount;
				Int			 timerDirection;
			protected:
				Int			*variable;

				Int			 startValue;
				Int 			 endValue;

				UnsignedInt		 stepSize;

				Hotspot			*arrow1Hotspot;
				Hotspot			*arrow2Hotspot;
			public:
				static const Int	 classID;

							 Arrows(const Point &, const Size &, Int = OR_HORZ, Int * = NIL, Int = 0, Int = 100);
				virtual			~Arrows();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetRange(Int, Int);

				Int			 SetValue(Int);
				Int			 GetValue();

				/* Control step size for arrows and mouse wheel action.
				 */
				Int			 SetStepSize(UnsignedInt);
				UnsignedInt		 GetStepSize() const;
			signals:
				Signal2<Void, Int, Int>	 onRangeChange;
				Signal1<Void, Int>	 onValueChange;
			slots:
				Void			 OnChangeSize(const Size &);

				Void			 OnMouseDownPlus();
				Void			 OnMouseDownMinus();

				Void			 OnMouseRelease();

				Void			 OnTimer();
		};
	};
};

#endif
