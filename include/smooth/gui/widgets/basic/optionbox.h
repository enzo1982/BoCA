 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_OPTIONBOX_
#define _H_OBJSMOOTH_OPTIONBOX_

namespace smooth
{
	namespace GUI
	{
		class OptionBox;
		class Hotspot;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI OptionBox : public Widget
		{
			protected:
				Int			*variable;
				Int			 code;
				Bool			 state;

				Hotspot			*hotspot;
			public:
				static const Int	 classID;

							 OptionBox(const String &, const Point &, const Size &, Int *, Int);
				virtual			~OptionBox();

				virtual Int		 Paint(Int);
			signals:
				static Signal0<Void>	 internalCheckValues;
			slots:
				Void			 InternalCheckValues();

				Void			 OnLeftButtonClick();
		};
	};
};

#endif
