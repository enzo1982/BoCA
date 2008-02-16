 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FONTDLG_
#define _H_OBJSMOOTH_FONTDLG_

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class FontSelection;
		};
	};
};

#include "dialog.h"
#include "../../graphics/font.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SMOOTHAPI FontSelection : public Dialog
			{
				private:
					Font		 font;
				public:
							 FontSelection();
					virtual		~FontSelection();

					const Error	&ShowDialog();
				accessors:
					const Font	&GetFont() const;
			};
		};
	};
};

#endif
