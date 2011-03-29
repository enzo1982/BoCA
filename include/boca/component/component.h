 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_COMPONENT
#define H_BOCA_COMPONENT

#include <smooth.h>
#include "../common/configlayer.h"
#include "../core/definitions.h"

#ifdef CS
#	undef CS
#endif

using namespace smooth;
using namespace smooth::GUI;

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT Component
		{
			protected:
				Bool			 errorState;
				String			 errorString;
			public:
							 Component();
				virtual			~Component();

				virtual ConfigLayer	*GetConfigurationLayer();

				Bool			 GetErrorState()	{ return errorState; }
				const String		&GetErrorString()	{ return errorString; }
		};
	};
};

#endif
