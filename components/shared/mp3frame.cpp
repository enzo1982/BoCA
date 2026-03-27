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

#include "mp3frame.h"

Int BoCA::MP3Frame::GetMPEGFrameSize(const Buffer<UnsignedByte> &header)
{
	/* MPEG bitrate table - [version][layer][bitrate]
	 */
	const UnsignedInt16	 mpegBitrate[4][4][16] = {
		{ // Version 2.5
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 3
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 2
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0 }  // Layer 1
		},
		{ // Reserved
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }  // Invalid
		},
		{ // Version 2
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 3
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 2
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0 }  // Layer 1
		},
		{ // Version 1
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0 }, // Layer 3
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0 }, // Layer 2
			{ 0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 }, // Layer 1
		}
	};

	/* MPEG sample rate table - [version][srate]
	 */
	const UnsignedInt16	 mpegSampleRate[4][4] = {
		{ 11025, 12000,  8000, 0 }, // MPEG 2.5
		{     0,     0,     0, 0 }, // Reserved
		{ 22050, 24000, 16000, 0 }, // MPEG 2
		{ 44100, 48000, 32000, 0 }  // MPEG 1
	};

	/* MPEG samples per frame table - [version][layer]
	 */
	const UnsignedInt16	 mpegFrameSamples[4][4] = {
		{    0,  576, 1152,  384 }, // MPEG 2.5
		{    0,    0,    0,    0 }, // Reserved
		{    0,  576, 1152,  384 }, // MPEG 2
		{    0, 1152, 1152,  384 }  // MPEG 1
	};

	/* MPEG unit size - [layer]
	 */
	const UnsignedInt8	 mpegUnitSize[4] = { 0, 1, 1, 4 }; // Reserved, 3, 2, 1

	/* Check header validity.
	 */
	if ((header[0] & 0xFF) != 0xFF ||	  // 8 synchronization bits
	    (header[1] & 0xE0) != 0xE0 ||	  // 3 synchronization bits
	    (header[1] & 0x18) == 0x08 ||	  // reserved version
	    (header[1] & 0x06) == 0x00 ||	  // reserved layer
	    (header[2] & 0xF0) == 0xF0) return 0; // reserved bitrate

	/* Get header values.
	 */
	Int	 version    = (header[1] & 0x18) >> 3; // version
	Int	 layer	    = (header[1] & 0x06) >> 1; // layer
	Int	 padding    = (header[2] & 0x02) >> 1; // padding
	Int	 bitrate    = (header[2] & 0xf0) >> 4; // bitrate
	Int	 sampleRate = (header[2] & 0x0c) >> 2; // samplerate

	/* Return frame size.
	 */
	return (((Float) mpegFrameSamples[version][layer] / 8.0 * (Float) (mpegBitrate[version][layer][bitrate] * 1000)) / (Float) mpegSampleRate[version][sampleRate]) + ((padding) ? mpegUnitSize[layer] : 0);
}
