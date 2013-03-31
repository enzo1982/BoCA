 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FILEDLG_HAIKU
#define H_OBJSMOOTH_FILEDLG_HAIKU

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class FileSelection;
		};
	};
};

#include "filedlg.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SMOOTHAPI FileSelection : public FileSelectionBase
			{
				public:
					const Error	&ShowDialog();
			};
		};
	};
};

#endif
