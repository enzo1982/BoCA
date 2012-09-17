 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
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

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			namespace Message
			{
				namespace Buttons
				{
					const Int		 Ok		  = 0;
					const Int		 OkCancel	  = 1;
					const Int		 AbortRetryIgnore = 2;
					const Int		 YesNoCancel	  = 3;
					const Int		 YesNo		  = 4;
					const Int		 RetryCancel	  = 5;
				};

				namespace Button
				{
					const Int		 Ok		  = 1;
					const Int		 Cancel		  = 2;
					const Int		 Abort		  = 3;
					const Int		 Retry		  = 4;
					const Int		 Ignore 	  = 5;
					const Int		 Yes		  = 6;
					const Int		 No		  = 7;
					const Int		 Close		  = 8;
				};

				namespace Icon
				{
					const wchar_t * const	 Error		  = (wchar_t *) 32513;
					const wchar_t * const	 Question	  = (wchar_t *) 32514;
					const wchar_t * const	 Warning	  = (wchar_t *) 32515;
					const wchar_t * const	 Information	  = (wchar_t *) 32516;
					const wchar_t * const	 Hand		  = Error;
					const wchar_t * const	 Exclamation	  = Warning;
					const wchar_t * const	 Asterisk	  = Information;
				};
			};

			class SMOOTHAPI MessageDlg : public Dialog
			{
				private:
					static Int		 nOfMessageBoxes;

					static Bool		 defaultRightToLeft;

					Int			 lines;
					String			 line[256];
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
								 MessageDlg(const String &, const String &, Int, const wchar_t *, const String & = NIL, Bool * = NIL);
					virtual			~MessageDlg();

					const Error		&ShowDialog();
				accessors:
					Int			 GetButtonCode() const;

					Void			 SetRightToLeft(Bool);
					Bool			 IsRightToLeft() const;

					static Void		 SetDefaultRightToLeft(Bool);
					static Bool		 IsDefaultRightToLeft();
			};

			SMOOTHAPI Int	 QuickMessage(const String &, const String &, Int, const char *);
			SMOOTHAPI Int	 QuickMessage(const String &, const String &, Int, const wchar_t *);
		}
	}
};

#endif
