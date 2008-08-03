 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/communication/joblist.h>

BoCA::JobList *BoCA::JobList::instance = NIL;

BoCA::JobList::JobList()
{
}

BoCA::JobList::~JobList()
{
}

BoCA::JobList *BoCA::JobList::Get()
{
	if (instance == NIL)
	{
		instance = new JobList();
	}

	return instance;
}

Void BoCA::JobList::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}
