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

#ifndef H_BOCA_CONVERTERCOMPONENT
#define H_BOCA_CONVERTERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT ConverterComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;
			public:
						 ConverterComponent();
				virtual		~ConverterComponent();

				/* Called to set information about output stream.
				 */
				virtual Bool	 SetAudioTrackInfo(const Track &);

				/* Activate/deactivate filter.
				 */
				virtual Bool	 Activate();
				virtual Bool	 Deactivate();
		};
	};
};

#endif
