 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_CHECKBOX_
#define _H_OBJSMOOTH_CHECKBOX_

namespace smooth
{
	namespace GUI
	{
		class CheckBox;
		class Hotspot;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI CheckBox : public Widget
		{
			protected:
				Bool			*variable;
				Bool			 state;

				Hotspot			*hotspot;
			public:
				static const Int	 classID;

							 CheckBox(const String &, const Point &, const Size &, Bool *);
				virtual			~CheckBox();

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
