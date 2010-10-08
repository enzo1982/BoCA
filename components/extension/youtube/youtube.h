 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "layer.h"

BoCA_BEGIN_COMPONENT(YouTube)

namespace BoCA
{
	class YouTube : public CS::ExtensionComponent
	{
		private:
			ConfigLayer		*configLayer;

			LayerYouTube		*mainTabLayer;
		public:
			static const String	&GetComponentSpecs();

						 YouTube();
						~YouTube();

			ConfigLayer		*GetConfigurationLayer();
		callbacks:
			Layer			*GetMainTabLayer();
	};
};

BoCA_DEFINE_EXTENSION_COMPONENT(YouTube)

BoCA_END_COMPONENT(YouTube)
