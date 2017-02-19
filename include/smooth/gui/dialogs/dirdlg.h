 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DIRDLG
#define H_OBJSMOOTH_DIRDLG

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class DirSelection;
		};
	};
};

#include "dialog.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SMOOTHAPI DirSelection : public Dialog
			{
				protected:
					String		 directory;
				public:
							 DirSelection();
					virtual		~DirSelection();

					const Error	&ShowDialog();
				accessors:
					Int		 SetDirName(const String &);
					const String	&GetDirName() const;
			};
		};
	};
};

#endif
