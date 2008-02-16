 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FILEDLG_
#define _H_OBJSMOOTH_FILEDLG_

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

#include "dialog.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			// File dialog mode constants - SFM_OPEN is default

			const Int	 SFM_OPEN	= 0;
			const Int	 SFM_SAVE	= 1;

			// File dialog flags - SFD_FILEMUSTEXIST is always set for SFM_OPEN mode

			const Int	 SFD_ALLOWMULTISELECT	= 512;
			const Int	 SFD_FILEMUSTEXIST	= 4096;
			const Int	 SFD_CONFIRMOVERWRITE	= 2;

			class SMOOTHAPI FileSelection : public Dialog
			{
				private:
					Array<String>	 filterNames;
					Array<String>	 filters;
					Array<String>	 files;

					Int		 flags;
					Int		 mode;

					String		 defExt;
					String		 defFile;
				public:
							 FileSelection();
					virtual		~FileSelection();

					const Error	&ShowDialog();

					Int		 SetMode(Int);
					Int		 SetFlags(Int);
					Int		 SetDefaultExtension(const String &);

					Int		 AddFilter(const String &, const String &);

					Int		 SetFileName(const String &);
					String		 GetFileName();

					Int		 GetNumberOfFiles();
					String		 GetNthFileName(Int);
			};
		};
	};
};

#endif
