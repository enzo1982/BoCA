 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SYSTEM
#define H_OBJSMOOTH_SYSTEM

namespace smooth
{
	namespace System
	{
		class System;
	};
};

#include "../definitions.h"
#include "../misc/string.h"

namespace smooth
{
	namespace System
	{
		enum PersonalFilesType
		{
			PersonalFilesGeneric = 0,

			PersonalFilesDocuments,
			PersonalFilesPictures,
			PersonalFilesMusic,
			PersonalFilesMovies,
			PersonalFilesDownloads,

			NumPersonalFilesTypes
		};

		class SMOOTHAPI System
		{
			private:
				static Int		 nextGUID;

							 System();
							 System(const System &);
			public:
				static Int		 RequestGUID();

				static String		 GetVersionString();
				static String		 GetAPIVersion();

				static UnsignedInt64	 Clock();
				static Bool		 Sleep(UnsignedInt);

				static Bool		 OpenURL(const String &);

				static String		 GetWindowsRootDirectory();
				static String		 GetPersonalFilesDirectory(PersonalFilesType = PersonalFilesGeneric);
				static String		 GetProgramFilesDirectory();
				static String		 GetApplicationDataDirectory();
				static String		 GetTempDirectory();
		};
	};
};

#endif
