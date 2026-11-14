 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_MUXERCOMPONENT
#define H_BOCA_AS_MUXERCOMPONENT

#include "component.h"
#include "../common/muxer.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT MuxerComponent : public Component
		{
			public:
							 MuxerComponent(ComponentSpecs *);
				virtual			~MuxerComponent();

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual Void		 SetDriver(IO::Driver *);

				virtual Bool		 Open(MuxerMode);
				virtual Bool		 Close();

				virtual const Format	&GetFormatInfo() const;

				virtual MuxerCodec	 GetCodec() const;
				virtual Bool		 SetCodec(MuxerCodec);

				virtual Bool		 GetCodecConfig(Buffer<UnsignedByte> &) const;
				virtual Bool		 SetCodecConfig(const Buffer<UnsignedByte> &);

				virtual Int		 GetTimeScale() const;
				virtual Bool		 SetTimeScale(Int);

				virtual Bool		 GetGaplessInfo(Int &, Int &, Int64 &) const;
				virtual Bool		 SetGaplessInfo(Int, Int, Int64);

				virtual Int64		 GetNumberOfPackets();
				virtual Int64		 GetDuration();

				virtual Int		 ReadPacket(Buffer<UnsignedByte> &, Int64 &, Int &);
				virtual Int		 WritePacket(const UnsignedByte *, Int, Int64, Int);

				virtual Int64		 SeekPacket(Int64);

				virtual Bool		 OptimizeFile(const String &);
		};
	};
};

#endif
