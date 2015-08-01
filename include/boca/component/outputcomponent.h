 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_OUTPUTCOMPONENT
#define H_BOCA_OUTPUTCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT OutputComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;
			public:
						 OutputComponent();
				virtual		~OutputComponent();

				/* Called to set information about output stream.
				 */
				virtual Bool	 SetAudioTrackInfo(const Track &);

				/* Activate/deactivate filter.
				 */
				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				/* Write data from buffer.
				 */
				virtual Int	 WriteData(Buffer<UnsignedByte> &, Int) = 0;

				/* Play till the end of available data, then stop.
				 */
				virtual Bool	 Finish();

				/* Returns how many bytes may be written.
				 */
				virtual Int	 CanWrite();

				/* Set pause/check if playing.
				 */
				virtual Int	 SetPause(Bool);
				virtual Bool	 IsPlaying();
		};
	};
};

#endif
