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

BoCA_BEGIN_COMPONENT(ExternalConfig)

namespace BoCA
{
	class ExternalConfig : public CS::Component
	{
		private:
			ConfigLayer		*configLayer;
		public:
			static const String	&GetComponentSpecs();

						 ExternalConfig();
						~ExternalConfig();

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_END_COMPONENT(ExternalConfig)
