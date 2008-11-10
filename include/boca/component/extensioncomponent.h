 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_EXTENSIONCOMPONENT
#define H_BOCA_EXTENSIONCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT ExtensionComponent : public Component
		{
			public:
							 ExtensionComponent();
				virtual			~ExtensionComponent();
			signals:
				SignalR0<Layer *>	 getMainTabLayer;
				SignalR0<Layer *>	 getStatusBarLayer;
		};
	};
};

#endif
