 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_EXTENSIONCOMPONENT
#define H_BOCA_EXTENSIONCOMPONENT

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
