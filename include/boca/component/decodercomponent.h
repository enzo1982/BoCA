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

#ifndef H_BOCA_DECODERCOMPONENT
#define H_BOCA_DECODERCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT DecoderComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;

				Int64		 inBytes;
			public:
						 DecoderComponent();
				virtual		~DecoderComponent();

				/* Stream information functions.
				 */
				virtual Bool	 CanOpenStream(const String &) = 0;
				virtual Error	 GetStreamInfo(const String &, Track &) = 0;

				/* Called to set information about input stream.
				 */
				virtual Bool	 SetAudioTrackInfo(const Track &);

				/* Activate/deactivate filter.
				 */
				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				/* Seek to specified sample position.
				 *
				 * Notes: - May be called only once directly after Activate().
				 *	  - Default implementation simply returns false to signal failure.
				 */
				virtual Bool	 Seek(Int64);

				/* Read data into buffer.
				 */
				virtual Int	 ReadData(Buffer<UnsignedByte> &) = 0;
			accessors:
				Int64		 GetInBytes() const { return inBytes; }
		};
	};
};

#endif
