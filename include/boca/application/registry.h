 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_REGISTRY
#define H_BOCA_REGISTRY

#include "component.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT Registry
		{
			private:
				static Registry		*registry;

				Array<ComponentSpecs *>	 componentSpecs;

							 Registry();
							~Registry();
			public:
				static Registry		&Get();
				static Bool		 Free();

				Int			 GetNumberOfComponents();
				Int			 GetNumberOfComponentsOfType(Int);

				const String		&GetComponentName(Int);
				const String		&GetComponentVersion(Int);
				const String		&GetComponentID(Int);
				Int			 GetComponentType(Int);
				const Array<Format *>	&GetComponentFormats(Int);

				Bool			 ComponentExists(const String &);

				Component		*CreateComponentByID(const String &);
				Bool			 DeleteComponent(Component *);
		};
	};
};

#endif
