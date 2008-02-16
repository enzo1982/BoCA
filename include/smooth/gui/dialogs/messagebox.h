 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MESSAGEBOX_
#define _H_OBJSMOOTH_MESSAGEBOX_

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class MessageDlg;
		}

		class Window;
		class Titlebar;
		class Layer;
		class Button;
		class CheckBox;
		class Image;
	};
};

#include "dialog.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SMOOTHAPI MessageDlg : public Dialog
			{
				private:
					static Int		 nOfMessageBoxes;

					Int			 lines;
					String			 line[256];
					wchar_t			*msgicon;
					Int			 buttons;

					Window			*msgbox;
					Titlebar		*titlebar;
					Layer			*lay;
					Image			*icon;

					Array<String>		 buttonLabels;
					Array<Button *, Void *>	 buttonWidgets;

					Int			 buttonCode;

					CheckBox		*checkbox;

					Bool			*cVar;
				slots:
					Void			 MessagePaintProc();
					Bool			 MessageKillProc();

					Void			 MessageButton(Int);

					Void			 MessageButton0();
					Void			 MessageButton1();
					Void			 MessageButton2();
				public:
								 MessageDlg(const String &, const String &, Int, wchar_t *, const String & = NIL, Bool * = NIL);
					virtual			~MessageDlg();

					const Error		&ShowDialog();
				accessors:
					Int			 GetButtonCode();
			};

			SMOOTHAPI Int	 QuickMessage(const String &, const String &, Int, char *);
			SMOOTHAPI Int	 QuickMessage(const String &, const String &, Int, wchar_t *);
		}
	}
};

#endif
