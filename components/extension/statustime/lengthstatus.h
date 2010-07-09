 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "layer.h"

BoCA_BEGIN_COMPONENT(LengthStatus)

namespace BoCA
{
	class LengthStatus : public CS::ExtensionComponent
	{
		private:
			LayerLengthStatus	*statusBarLayer;
		public:
			static const String	&GetComponentSpecs();

						 LengthStatus();
						~LengthStatus();
	};
};

BoCA_DEFINE_EXTENSION_COMPONENT(LengthStatus)

BoCA_END_COMPONENT(LengthStatus)
