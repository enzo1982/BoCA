 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CHECKBOX
#define H_OBJSMOOTH_CHECKBOX

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
			private:
				Bool			 dummyVariable;
			protected:
				Bool			*variable;
				Bool			 state;

				Hotspot			*hotspot;
			public:
				static const Short	 classID;

							 CheckBox(const String &, const Point &, const Size &, Bool *);
				virtual			~CheckBox();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetChecked(Bool);
				Bool			 IsChecked() const	{ return *variable; }
			signals:
				static Signal0<Void>	 internalCheckValues;
			slots:
				Void			 InternalCheckValues();

				Void			 OnLeftButtonClick();
		};
	};
};

#endif
