 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "vorbis.h"

using namespace smooth::IO;

const String &BoCA::DecoderVorbis::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && vorbisdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Ogg Vorbis Decoder</name>					\
		    <version>1.0</version>						\
		    <id>vorbis-dec</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Ogg Vorbis Audio</name>					\
		      <extension>ogg</extension>					\
		      <extension>oga</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
	LoadVorbisDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeVorbisDLL();
}

Bool BoCA::DecoderVorbis::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	in.Seek(0);

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	Bool	 result = False;

	Bool	 initialized = False;
	Bool	 done = False;

	while (!done)
	{
		Int	 size	= Math::Min((Int64) 4096, in.Size() - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		if (ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

			initialized = True;

			ex_ogg_stream_pagein(&os, &og);

			if (ex_ogg_stream_packetout(&os, &op) == 1)
			{
				if (op.packet[0] ==  1  &&
				    op.packet[1] == 'v' && op.packet[2] == 'o' && op.packet[3] == 'r' && op.packet[4] == 'b' && op.packet[5] == 'i' && op.packet[6] == 's') result = True;

				done = True;
			}
		}
	}

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return result;
}

Error BoCA::DecoderVorbis::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	Format	 format = track.GetFormat();

	format.bits  = 16;

	track.fileSize = in.Size();

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	vorbis_info		 vi;
	vorbis_comment		 vc;

	ex_vorbis_info_init(&vi);
	ex_vorbis_comment_init(&vc);

	/* Get stream format.
	 */
	Buffer<UnsignedByte>	 comments;

	Bool	 initialized = False;
	Bool	 done	     = False;
	Int	 packetNum   = 0;

	while (!done)
	{
		Int	 size	= Math::Min((Int64) 4096, track.fileSize - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		while (ex_ogg_sync_pageout(&oy, &og) == 1 && !done)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				ex_vorbis_synthesis_headerin(&vi, &vc, &op);

				/* Found header packet.
				 */
				if (packetNum == 0)
				{
					format.rate	= vi.rate;
					format.channels	= vi.channels;
					track.length	= -1;

					Int	 bitrate = 0;

					if	(vi.bitrate_nominal > 0)		       bitrate = vi.bitrate_nominal;
			 		else if (vi.bitrate_lower > 0 && vi.bitrate_upper > 0) bitrate = (vi.bitrate_lower + vi.bitrate_upper) / 2;

					if (bitrate > 0) track.approxLength = track.fileSize / (bitrate / 8) * format.rate;
				}

				/* Found Vorbis Comment packet.
				 */
				if (packetNum == 1)
				{
					comments.Resize(op.bytes - 7);

					memcpy(comments, op.packet + 7, op.bytes - 7);
				}

				/* Done if we reached packet two.
				 */
				if (packetNum >= 2) done = True;

				packetNum++;
			}
		}
	}

	track.SetFormat(format);

	/* Find real length of stream.
	 */
	ex_ogg_sync_reset(&oy);

	in.Seek(in.Size() - 65536);

	char	*buffer = ex_ogg_sync_buffer(&oy, 65536);
	Int	 size	= in.InputData(buffer, 65536);

	ex_ogg_sync_wrote(&oy, size);

	while (true)
	{
		Int	 seek = ex_ogg_sync_pageseek(&oy, &og);

		if (seek == 0) break;
		if (seek <  0 || ex_ogg_page_serialno(&og) != os.serialno) continue;

		track.length = ex_ogg_page_granulepos(&og);

		if (ex_ogg_page_eos(&og)) break;
	}

	/* Read tags.
	 */
	if (comments.Size() > 0)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetConfiguration(GetConfiguration());
			tagger->ParseBuffer(comments, track);

			boca.DeleteComponent(tagger);
		}
	}

	/* Clean up.
	 */
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return Success();
}

BoCA::DecoderVorbis::DecoderVorbis()
{
	packageSize = 0;

	skipSamples = 0;

	memset(&oy, 0, sizeof(oy));
	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));

	memset(&vi, 0, sizeof(vi));
	memset(&vc, 0, sizeof(vc));
	memset(&vd, 0, sizeof(vd));
	memset(&vb, 0, sizeof(vb));
}

BoCA::DecoderVorbis::~DecoderVorbis()
{
}

Bool BoCA::DecoderVorbis::Activate()
{
	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size	= 4096;
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		size = driver->ReadData((unsigned char *) buffer, 4096);

		inBytes += size;

		ex_ogg_sync_wrote(&oy, size);

		while (ex_ogg_sync_pageout(&oy, &og) == 1 && !done)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				ex_vorbis_info_init(&vi);
				ex_vorbis_comment_init(&vc);

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				ex_vorbis_synthesis_headerin(&vi, &vc, &op);

				if (packetNum >= 2) done = True;

				packetNum++;
			}
		}
	}

	ex_vorbis_synthesis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	return True;
}

Bool BoCA::DecoderVorbis::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	ex_ogg_sync_clear(&oy);

	return True;
}

Bool BoCA::DecoderVorbis::Seek(Int64 samplePosition)
{
	while (ex_ogg_page_granulepos(&og) + 1024 <= samplePosition || ex_ogg_page_serialno(&og) != os.serialno)
	{
		skipSamples = samplePosition - ex_ogg_page_granulepos(&og) - 1024;

		while (ex_ogg_sync_pageseek(&oy, &og) == 0)
		{
			char	*buffer = ex_ogg_sync_buffer(&oy, 131072);
			Int	 size	= driver->ReadData((unsigned char *) buffer, 131072);

			inBytes += size;

			ex_ogg_sync_wrote(&oy, size);

			if (size == 0) return False;
		}
	}

	ex_ogg_stream_pagein(&os, &og);

	ex_vorbis_synthesis_restart(&vd);

	return True;
}

Int BoCA::DecoderVorbis::ReadData(Buffer<UnsignedByte> &data)
{
	char	*buffer = ex_ogg_sync_buffer(&oy, data.Size());
	Int	 size	= driver->ReadData((unsigned char *) buffer, data.Size());

	if (size <= 0) return -1;

	inBytes += size;

	ex_ogg_sync_wrote(&oy, size);

	size = 0;

	Int	 dataBufferLen = 0;

	while (ex_ogg_sync_pageout(&oy, &og) == 1)
	{
		ex_ogg_stream_pagein(&os, &og);

		while (ex_ogg_stream_packetout(&os, &op) == 1)
		{
			float  **pcm;
			int	 samples;

			ex_vorbis_synthesis(&vb, &op);
			ex_vorbis_synthesis_blockin(&vd, &vb);

			while ((samples = ex_vorbis_synthesis_pcmout(&vd, &pcm)) > 0)
			{
				Int	 bout = Math::Min(samples, sizeof(convBuffer) / vi.channels / 2);

				for (Int i = 0; i < vi.channels; i++)
				{
					short	*ptr = convBuffer + i;
					float	*mono = pcm[i];

					for (Int j = 0; j < bout; j++)
					{
						Int	 val = Int(mono[j] * 32767.f);

						val = Math::Min(val, 32767);
						val = Math::Max(val, -32768);

						*ptr = val;
						ptr += vi.channels;
					}
				}

				if (bout > skipSamples)
				{
					if (dataBufferLen < size + ((bout - skipSamples) * vi.channels * 2))
					{
						dataBufferLen += (((bout - skipSamples) * vi.channels * 2) + 131072);

						data.Resize(dataBufferLen);
					}

					memcpy(((unsigned char *) data) + size, convBuffer + skipSamples * vi.channels, (bout - skipSamples) * vi.channels * 2);

					size += ((bout - skipSamples) * vi.channels * 2);
				}

				skipSamples = Math::Max(0, skipSamples - bout);

				ex_vorbis_synthesis_read(&vd, bout);
			}
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	data.Resize(size);

	const Format	&format	= track.GetFormat();

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_7_1, Channel::Default_7_1);

	return size;
}
