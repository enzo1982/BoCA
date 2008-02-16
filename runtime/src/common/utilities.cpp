 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/utilities.h>

using namespace smooth::GUI::Dialogs;

Void BoCA::Utilities::WarningMessage(const String &message, const String &replace)
{
	QuickMessage(String(message).Replace("%1", replace), "Warning", MB_OK, IDI_EXCLAMATION);
}

Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace)
{
	QuickMessage(String(message).Replace("%1", replace), "Error", MB_OK, IDI_HAND);
}
