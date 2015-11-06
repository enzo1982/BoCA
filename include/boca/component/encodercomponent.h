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

#ifndef H_BOCA_ENCODERCOMPONENT
#define H_BOCA_ENCODERCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT EncoderComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;
			public:
						 EncoderComponent();
				virtual		~EncoderComponent();

				/* Called to set information about output stream.
				 */
				virtual Bool	 SetAudioTrackInfo(const Track &);

				/* Returns the configured output file extension.
				 */
				virtual String	 GetOutputFileExtension() const;

				/* Returns the number of passes needed by the encoder.
				 */
				virtual Int	 GetNumberOfPasses() const;

				/* Returns true if the encoder is thread safe.
				 */
				virtual Bool	 IsThreadSafe() const;

				/* Returns true if the output format is lossless.
				 */
				virtual Bool	 IsLossless() const;

				/* Activate/deactivate filter.
				 */
				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				/* Write data from buffer.
				 */
				virtual Int	 WriteData(Buffer<UnsignedByte> &) = 0;

				/* Start the next encoding pass.
				 */
				virtual Bool	 NextPass();
		};
	};
};

#endif
