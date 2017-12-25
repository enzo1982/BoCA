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

#ifndef H_BOCA_AS_CONVERTERCOMPONENT
#define H_BOCA_AS_CONVERTERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT ConverterComponent : public Component, public IO::Filter
		{
			protected:
				Bool			 errorState;
				String			 errorString;

				Track			 track;
			public:
							 ConverterComponent(ComponentSpecs *);
				virtual 		~ConverterComponent();

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual Bool		 Activate();
				virtual Bool		 Deactivate();

				virtual Int		 GetPackageSize() const;
				virtual Int		 SetDriver(IO::Driver *);

				virtual Bool		 GetErrorState() const;
				virtual const String	&GetErrorString() const;
		};
	};
};

#endif
