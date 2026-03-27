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

#include <boca.h>

namespace BoCA
{
	class VbrTag
	{
		public:
			static const UnsignedInt	 FLAG_FRAMES  = 0x01;
			static const UnsignedInt	 FLAG_BYTES   = 0x02;
			static const UnsignedInt	 FLAG_TOC     = 0x04;
			static const UnsignedInt	 FLAG_QUALITY = 0x08;
			static const UnsignedInt	 FLAG_GAPLESS = 0x10;

							 VbrTag();

			Int				 offset;   // header offset in frame
			UnsignedInt			 flags;    // available information flags
			UnsignedInt			 frames;   // total bit stream frames
			UnsignedInt			 bytes;    // total bit stream bytes
			Int				 quality;  // VBR quality indicator
			UnsignedInt			 delay;    // delay before first valid audio sample
			UnsignedInt			 padding;  // padding samples in last frame
			UnsignedByte			 toc[100]; // TOC entries

			Bool				 Parse(const Buffer<UnsignedByte> &);
	};
};
