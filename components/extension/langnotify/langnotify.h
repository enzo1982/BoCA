 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

#include "dialog.h"

BoCA_BEGIN_COMPONENT(LanguageNotifier)

namespace BoCA
{
	class LanguageNotifier : public CS::ExtensionComponent
	{
		private:
			LanguageNotifierDialog	*dialog;
		public:
			static const String	&GetComponentSpecs();

						 LanguageNotifier();
						~LanguageNotifier();
	};
};

BoCA_DEFINE_EXTENSION_COMPONENT(LanguageNotifier)

BoCA_END_COMPONENT(LanguageNotifier)
