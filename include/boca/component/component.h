 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_COMPONENT
#define H_BOCA_COMPONENT

#include "../common/config.h"
#include "../common/configlayer.h"

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
			private:
				const Config		*configuration;
			protected:
				Bool			 errorState;
				String			 errorString;
			public:
							 Component();
				virtual			~Component();

				virtual ConfigLayer	*GetConfigurationLayer();

				Bool			 GetErrorState() const	{ return errorState; }
				const String		&GetErrorString() const	{ return errorString; }

				const Config		*GetConfiguration() const;
				Bool			 SetConfiguration(const Config *);
		};
	};
};

#endif
