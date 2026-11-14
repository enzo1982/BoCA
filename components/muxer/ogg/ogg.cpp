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

#include <smooth.h>
#include <smooth/dll.h>

#include "ogg.h"

const String &BoCA::MuxerOgg::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Ogg Muxer</name>			\
		    <version>1.0</version>			\
		    <id>ogg-mux</id>				\
		    <type>muxer</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 readBufferSize	  = 8192;
	const Int	 seekBufferSize	  = 131072;
	const Int	 lengthBufferSize = 65536;
};

BoCA::MuxerOgg::MuxerOgg()
{
	mode		  = MUXER_MODE_READ;

	opened		  = False;
	streamInitialized = False;

	pagePosition	  = 0;
	pageStartPosition = 0;

	memset(&oy, 0, sizeof(oy));
	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));
}

BoCA::MuxerOgg::~MuxerOgg()
{
	if (opened) Close();
}

Bool BoCA::MuxerOgg::Open(MuxerMode nMode)
{
	if (opened || driver == NIL) return False;

	if (nMode != MUXER_MODE_READ && nMode != MUXER_MODE_WRITE)
	{
		errorState  = True;
		errorString = "Unsupported muxer mode";

		return False;
	}

	mode		  = nMode;

	pagePosition	  = 0;
	pageStartPosition = 0;

	if (mode == MUXER_MODE_READ)
	{
		/* Init Ogg reader and read the first page
		 * to find the serial number of the stream.
		 */
		ex_ogg_sync_init(&oy);

		opened = True;

		if (!ReadPage())
		{
			Close();

			errorState  = True;
			errorString = "Invalid file format";

			return False;
		}
	}
	else
	{
		/* Init Ogg stream.
		 */
		Math::RandomSeed();

		ex_ogg_stream_init(&os, Math::Random());

		opened		  = True;
		streamInitialized = True;
	}

	return True;
}

Bool BoCA::MuxerOgg::Close()
{
	if (!opened) return False;

	/* Write any remaining Ogg packets.
	 */
	if (mode == MUXER_MODE_WRITE) WritePages(True);

	/* Clean up.
	 */
	if (streamInitialized) ex_ogg_stream_clear(&os);

	if (mode == MUXER_MODE_READ) ex_ogg_sync_clear(&oy);

	opened		  = False;
	streamInitialized = False;

	return True;
}

Int64 BoCA::MuxerOgg::GetDuration()
{
	if (!opened || !streamInitialized) return -1;

	if (mode == MUXER_MODE_WRITE) return pagePosition;

	/* Scan the end of the stream to find
	 * the granule position of the last page.
	 */
	Int64		 duration   = -1;

	Int64		 driverPos  = driver->GetPos();
	Int64		 driverSize = driver->GetSize();

	Int		 size	    = Math::Min(driverSize, Int64(lengthBufferSize));

	ogg_sync_state	 sync;
	ogg_page	 page;

	ex_ogg_sync_init(&sync);

	char		*buffer	    = ex_ogg_sync_buffer(&sync, size);

	driver->Seek(driverSize - size);

	size = driver->ReadData((UnsignedByte *) buffer, size);

	ex_ogg_sync_wrote(&sync, Math::Max(0, size));

	while (true)
	{
		Int	 seek = ex_ogg_sync_pageseek(&sync, &page);

		if (seek == 0) break;
		if (seek <  0 || ex_ogg_page_serialno(&page) != os.serialno) continue;

		duration = ex_ogg_page_granulepos(&page);

		if (ex_ogg_page_eos(&page)) break;
	}

	ex_ogg_sync_clear(&sync);

	/* Restore stream position.
	 */
	driver->Seek(driverPos);

	return duration;
}

Int BoCA::MuxerOgg::ReadPacket(Buffer<UnsignedByte> &buffer, Int64 &position, Int &flags)
{
	if (!opened || mode != MUXER_MODE_READ) return -1;

	/* Get next packet, reading more
	 * pages from the stream if necessary.
	 */
	while (true)
	{
		Int	 result = ex_ogg_stream_packetout(&os, &op);

		if (result == 1) break;
		if (result <  0) continue;

		if (!ReadPage()) return -1;
	}

	/* Copy packet data and information.
	 */
	buffer.Resize(op.bytes);

	if (op.bytes > 0) memcpy(buffer, op.packet, op.bytes);

	position = op.granulepos;
	flags	 = PACKET_FLAG_NONE;

	if (op.b_o_s) flags |= PACKET_FLAG_BOS;
	if (op.e_o_s) flags |= PACKET_FLAG_EOS;

	return op.bytes;
}

Int BoCA::MuxerOgg::WritePacket(const UnsignedByte *data, Int size, Int64 position, Int flags)
{
	if (!opened || mode != MUXER_MODE_WRITE) return -1;

	/* Submit packet to Ogg stream.
	 */
	op.packet     = (unsigned char *) data;
	op.bytes      = size;
	op.b_o_s      = (flags & PACKET_FLAG_BOS) ? 1 : 0;
	op.e_o_s      = (flags & PACKET_FLAG_EOS) ? 1 : 0;
	op.granulepos = position;
	op.packetno   = 0;

	if (ex_ogg_stream_packetin(&os, &op) != 0) return -1;

	pagePosition = position;

	/* Write finished pages.
	 */
	return WritePages(flags & PACKET_FLAG_FLUSH);
}

Int64 BoCA::MuxerOgg::SeekPacket(Int64 position)
{
	if (!opened || mode != MUXER_MODE_READ) return -1;

	/* Skip pages until we find the
	 * one containing the position.
	 */
	Bool	 skipped = False;

	while (pagePosition <= position)
	{
		Int	 result = 0;

		while ((result = ex_ogg_sync_pageseek(&oy, &og)) == 0)
		{
			char	*buffer = ex_ogg_sync_buffer(&oy, seekBufferSize);
			Int	 size	= driver->ReadData((UnsignedByte *) buffer, seekBufferSize);

			if (size <= 0) return -1;

			ex_ogg_sync_wrote(&oy, size);
		}

		if (result < 0 || ex_ogg_page_serialno(&og) != os.serialno) continue;

		skipped = True;

		/* Update position, ignoring pages
		 * on which no packet is completed.
		 */
		if (ex_ogg_page_granulepos(&og) == -1) continue;

		pageStartPosition = pagePosition;
		pagePosition	  = ex_ogg_page_granulepos(&og);
	}

	/* Submit the page we found to the Ogg stream.
	 */
	if (skipped) ex_ogg_stream_pagein(&os, &og);

	return pageStartPosition;
}

Bool BoCA::MuxerOgg::ReadPage()
{
	/* Read data until a page is available.
	 */
	while (true)
	{
		Int	 result = ex_ogg_sync_pageout(&oy, &og);

		if (result == 1) break;
		if (result <  0) continue;

		char	*buffer = ex_ogg_sync_buffer(&oy, readBufferSize);
		Int	 size	= driver->ReadData((UnsignedByte *) buffer, readBufferSize);

		if (size <= 0) return False;

		ex_ogg_sync_wrote(&oy, size);
	}

	/* Init stream using the serial number of the first page.
	 */
	if (!streamInitialized)
	{
		ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

		streamInitialized = True;
	}

	/* Submit page to the Ogg stream, ignoring pages of other streams.
	 */
	if (ex_ogg_page_serialno(&og) != os.serialno) return True;

	ex_ogg_stream_pagein(&os, &og);

	if (ex_ogg_page_granulepos(&og) != -1)
	{
		pageStartPosition = pagePosition;
		pagePosition	  = ex_ogg_page_granulepos(&og);
	}

	return True;
}

Int BoCA::MuxerOgg::WritePages(Bool flush)
{
	Int	 bytes = 0;

	do
	{
		int	 result = 0;

		if (flush) result = ex_ogg_stream_flush(&os, &og);
		else	   result = ex_ogg_stream_pageout(&os, &og);

		if (result == 0) break;

		bytes += driver->WriteData(og.header, og.header_len);
		bytes += driver->WriteData(og.body, og.body_len);
	}
	while (true);

	return bytes;
}
