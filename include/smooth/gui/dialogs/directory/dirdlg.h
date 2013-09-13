 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
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
			class DirSelectionBase;
		};
	};
};

#include "../dialog.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			abstract class SMOOTHAPI DirSelectionBase : public Dialog
			{
				protected:
					String			 directory;
				public:
								 DirSelectionBase();
					virtual			~DirSelectionBase();

					virtual const Error	&ShowDialog() = 0;
				accessors:
					Int			 SetDirName(const String &);
					const String		&GetDirName() const;
			};
		};
	};
};

#endif
