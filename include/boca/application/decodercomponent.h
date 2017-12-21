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

#ifndef H_BOCA_AS_DECODERCOMPONENT
#define H_BOCA_AS_DECODERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT DecoderComponent : public Component, public IO::Filter
		{
			public:
						 DecoderComponent(ComponentSpecs *);
				virtual		~DecoderComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);

				virtual Bool	 CanOpenStream(const String &);
				virtual Error	 GetStreamInfo(const String &, Track &);

				virtual Int64	 GetInBytes() const;

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Bool	 Seek(Int64);

				virtual Int	 ReadData(Buffer<UnsignedByte> &);

				virtual Int	 GetPackageSize() const;
				virtual Int	 SetDriver(IO::Driver *);

		};
	};
};

#endif
