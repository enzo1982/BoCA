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

#include "vbrtag.h"

#include <math.h>

static UnsignedInt32 ReadInt32(UnsignedByte *buffer)
{
	/* Read big endian 32 bit int.
	 */
	return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}

BoCA::VbrTag::VbrTag()
{
	offset	 = -1;
	flags	 = 0;
	frames	 = 0;
	bytes	 = 0;
	quality  = -1;
	delay	 = 0;
	padding	 = 0;

	memset(toc, 0, sizeof(toc));
}

Bool BoCA::VbrTag::Parse(const Buffer<UnsignedByte> &buffer)
{
	this->offset = -1;
	this->flags  = 0;

	/* Determine header offset.
	 */
	Int      version = (buffer[1] >> 3) & 1;
	Int      mode	 = (buffer[3] >> 6) & 3;
	Int	 offset	 = 0;

	if (version) // MPEG 1
	{
		if (mode != 3) offset = 32 + 4;
		else	       offset = 17 + 4;
	}
	else	     // MPEG 2
	{
		if (mode != 3) offset = 17 + 4;
		else	       offset = 9 + 4;
	}

	/* Check for known header type.
	 */
	if ((buffer[offset] == 'X' && buffer[offset + 1] == 'i' && buffer[offset + 2] == 'n' && buffer[offset + 3] == 'g') ||
	    (buffer[offset] == 'I' && buffer[offset + 1] == 'n' && buffer[offset + 2] == 'f' && buffer[offset + 3] == 'o'))
	{
		this->offset = offset;

		/* Read flags and data.
		 */
		Int	 index = offset + 4;

						{ this->flags	= ReadInt32(buffer + index); index += 4; }

		if (this->flags & FLAG_FRAMES)  { this->frames  = ReadInt32(buffer + index); index += 4; }
		if (this->flags & FLAG_BYTES)   { this->bytes	= ReadInt32(buffer + index); index += 4; }

		if (this->flags & FLAG_TOC)
		{
			for (Int i = 0; i < 100; i++) this->toc[i] = buffer[index + i];

			index += 100;
		}

		if (this->flags & FLAG_QUALITY) { this->quality = ReadInt32(buffer + index); index += 4; }

		/* Check for valid gapless data in LAME tag.
		 */
		UnsignedInt16	 crc = Hash::CRC16::Compute(buffer, offset + 0x9A);

		if (buffer.Size() >= 192 && buffer[offset + 0x9A] == (crc >> 8) && buffer[offset + 0x9B] == (crc & 0xFF))
		{
			this->flags  |= FLAG_GAPLESS;

			this->delay   = ( buffer[offset + 0x8D]		<< 4) | ((buffer[offset + 0x8E] & 0xF0) >> 4);
			this->padding = ((buffer[offset + 0x8E] & 0x0F) << 8) | ( buffer[offset + 0x8F]		    );
		}

		return True;
	}
	else if ((buffer[0x24] == 'V' && buffer[0x25] == 'B' && buffer[0x26] == 'R' && buffer[0x27] == 'I'))
	{
		this->offset  = 0x24;
		this->flags   = FLAG_FRAMES | FLAG_BYTES;

		Int	 index = this->offset + 10;

		{ this->bytes  = ReadInt32(buffer + index);	index += 4; }
		{ this->frames = ReadInt32(buffer + index) - 1; index += 4; }

		return True;
	}

	return False;
}
