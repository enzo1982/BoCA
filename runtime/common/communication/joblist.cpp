 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/communication/joblist.h>

BoCA::JobList *BoCA::JobList::instance = NIL;

BoCA::JobList::JobList()
{
	locked = False;
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

Int BoCA::JobList::Lock()
{
	if (IsLocked()) return Error();

	locked = True;

	return Success();
}

Int BoCA::JobList::Unlock()
{
	if (!IsLocked()) return Error();

	locked = False;

	return Success();
}

Bool BoCA::JobList::IsLocked()
{
	return locked;
}
