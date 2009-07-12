 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_EXTENSIONCOMPONENT
#define H_BOCA_AS_EXTENSIONCOMPONENT

#include "component.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT ExtensionComponent : public Component
		{
			private:
				Layer			*GetMainTabLayer();
				Layer			*GetStatusBarLayer();
			public:
							 ExtensionComponent(ComponentSpecs *);
				virtual			~ExtensionComponent();
			signals:
				SignalR0<Layer *>	 getMainTabLayer;
				SignalR0<Layer *>	 getStatusBarLayer;
		};
	};
};

#endif
