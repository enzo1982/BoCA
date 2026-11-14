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

#ifndef H_BOCA_MUXER
#define H_BOCA_MUXER

namespace BoCA
{
	/* Muxer operation modes.
	 */
	enum MuxerMode
	{
		MUXER_MODE_READ = 0,	// Demux packets from an existing stream.
		MUXER_MODE_WRITE,	// Mux packets into a new stream.
		MUXER_MODE_MODIFY,	// Update stream information of an existing stream.

		NUM_MUXER_MODES
	};

	/* Codecs identified by muxers.
	 */
	enum MuxerCodec
	{
		MUXER_CODEC_UNKNOWN = 0,

		MUXER_CODEC_AAC,
		MUXER_CODEC_ALAC,
		MUXER_CODEC_OPUS,
		MUXER_CODEC_SPEEX,
		MUXER_CODEC_VORBIS,

		NUM_MUXER_CODECS
	};

	/* Flags describing packets read from or written to a muxer.
	 */
	enum PacketFlags
	{
		PACKET_FLAG_NONE   = 0,

		PACKET_FLAG_HEADER = 1,	// Codec header packet (not containing audio data).
		PACKET_FLAG_BOS	   = 2,	// First packet of the stream.
		PACKET_FLAG_EOS	   = 4,	// Last packet of the stream.
		PACKET_FLAG_FLUSH  = 8	// Finish the current page or chunk after writing this packet.
	};
};

#endif
