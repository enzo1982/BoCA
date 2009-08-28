 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MESSAGEBOX
#define H_OBJSMOOTH_MESSAGEBOX

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

#ifndef __WIN32__
#	define MB_OK			0
#	define MB_OKCANCEL		1
#	define MB_ABORTRETRYIGNORE	2
#	define MB_YESNOCANCEL		3
#	define MB_YESNO			4
#	define MB_RETRYCANCEL		5

#	define IDOK			1
#	define IDCANCEL			2
#	define IDABORT			3
#	define IDRETRY			4
#	define IDIGNORE			5
#	define IDYES			6
#	define IDNO			7
#	define IDCLOSE			8

#	define IDI_HAND			(wchar_t *) 32513
#	define IDI_QUESTION		(wchar_t *) 32514
#	define IDI_EXCLAMATION		(wchar_t *) 32515
#	define IDI_ASTERISK		(wchar_t *) 32516
#	define IDI_WARNING		IDI_EXCLAMATION
#	define IDI_ERROR		IDI_HAND
#	define IDI_INFORMATION		IDI_ASTERISK
#endif

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
