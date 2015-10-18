 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_FORMAT
#define H_BOCA_FORMAT

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	enum ByteOrder
	{
		BYTE_NATIVE = 0,
		BYTE_RAW,
		BYTE_INTEL,

		NUM_BYTE_ORDERS
	};

	namespace Channel
	{
		typedef UnsignedByte	 Layout[];

		const UnsignedByte	 FrontLeft     =  0;
		const UnsignedByte	 FrontRight    =  1;
		const UnsignedByte	 FrontCenter   =  2;
		const UnsignedByte	 LFE	       =  3;
		const UnsignedByte	 RearLeft      =  4;
		const UnsignedByte	 RearRight     =  5;
		const UnsignedByte	 RearCenter    =  6;
		const UnsignedByte	 SideLeft      =  7;
		const UnsignedByte	 SideRight     =  8;
		const UnsignedByte	 TopFrontLeft  =  9;
		const UnsignedByte	 TopFrontRight = 10;

		const Layout		 Default_2_0 = { FrontLeft, FrontRight };
		const Layout		 Default_2_1 = { FrontLeft, FrontRight, LFE };
		const Layout		 Default_3_0 = { FrontLeft, FrontRight, FrontCenter };
		const Layout		 Default_3_1 = { FrontLeft, FrontRight, FrontCenter, LFE };
		const Layout		 Default_4_0 = { FrontLeft, FrontRight, RearLeft, RearRight };
		const Layout		 Default_4_1 = { FrontLeft, FrontRight, LFE, RearLeft, RearRight };
		const Layout		 Default_5_0 = { FrontLeft, FrontRight, FrontCenter, RearLeft, RearRight };
		const Layout		 Default_5_1 = { FrontLeft, FrontRight, FrontCenter, LFE, RearLeft, RearRight };
		const Layout		 Default_6_1 = { FrontLeft, FrontRight, FrontCenter, LFE, RearCenter, SideLeft, SideRight };
		const Layout		 Default_7_1 = { FrontLeft, FrontRight, FrontCenter, LFE, RearLeft, RearRight, SideLeft, SideRight };
		const Layout		 Default_9_1 = { FrontLeft, FrontRight, FrontCenter, LFE, RearLeft, RearRight, SideLeft, SideRight, TopFrontLeft, TopFrontRight };

		const Layout		 AAC_3_0     = { FrontCenter, FrontLeft, FrontRight };
		const Layout		 AAC_5_0     = { FrontCenter, FrontLeft, FrontRight, RearLeft, RearRight };
		const Layout		 AAC_5_1     = { FrontCenter, FrontLeft, FrontRight, RearLeft, RearRight, LFE };
		const Layout		 AAC_6_1     = { FrontCenter, FrontLeft, FrontRight, SideLeft, SideRight, RearCenter, LFE };
		const Layout		 AAC_7_1     = { FrontCenter, FrontLeft, FrontRight, SideLeft, SideRight, RearLeft, RearRight, LFE };

		const Layout		 AIFF_3_1    = { FrontLeft, FrontCenter, FrontRight, LFE };
		const Layout		 AIFF_5_1    = { FrontLeft, RearLeft, FrontCenter, FrontRight, RearRight, LFE };

		const Layout		 Vorbis_3_0  = { FrontLeft, FrontCenter, FrontRight };
		const Layout		 Vorbis_5_0  = { FrontLeft, FrontCenter, FrontRight, RearLeft, RearRight };
		const Layout		 Vorbis_5_1  = { FrontLeft, FrontCenter, FrontRight, RearLeft, RearRight, LFE };
		const Layout		 Vorbis_6_1  = { FrontLeft, FrontCenter, FrontRight, SideLeft, SideRight, RearCenter, LFE };
		const Layout		 Vorbis_7_1  = { FrontLeft, FrontCenter, FrontRight, SideLeft, SideRight, RearLeft, RearRight, LFE };

		const Layout		 ALSA_5_0    = { FrontLeft, FrontRight, RearLeft, RearRight, FrontCenter };
		const Layout		 ALSA_5_1    = { FrontLeft, FrontRight, RearLeft, RearRight, FrontCenter, LFE };
	};

	class BOCA_DLL_EXPORT Format
	{
		public:
			/* Audio format information:
			 */
			Int		 rate;

			S::Byte		 channels;
			S::Byte		 bits;

			ByteOrder	 order;

			/* Class constructor / destructor:
			 */
					 Format();
					~Format();
	};
};

#endif
