 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_STREAMCOMPONENT
#define H_BOCA_STREAMCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT StreamComponent : public Component
		{
			protected:
				IO::Driver	*driver;

				Track		 track;
			public:
						 StreamComponent();
				virtual		~StreamComponent();

				/* Called to set information about output stream.
				 */
				virtual Bool	 SetAudioTrackInfo(const Track &);

				/* Returns true if the component is thread safe.
				 */
				virtual Bool	 IsThreadSafe() const;

				/* Set IO driver to be used by this component.
				 */
				Void		 SetDriver(IO::Driver *);

				/* Activate/deactivate filter.
				 */
				virtual Bool	 Activate();
				virtual Bool	 Deactivate();
		};
	};
};

#endif
