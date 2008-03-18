 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "vorbis_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::VorbisIn::GetComponentSpecs()
{

	static String	 componentSpecs;

	if (vorbisdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Ogg Vorbis Decoder</name>		\
		    <version>1.0</version>			\
		    <id>vorbis-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Ogg Vorbis Audio</name>		\
		      <extension>ogg</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadVorbisDLL();
}

Void smooth::DetachDLL()
{
	FreeVorbisDLL();
}

Bool BoCA::VorbisIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".ogg");
}

Error BoCA::VorbisIn::GetStreamInfo(const String &streamURI, Track &format)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

	format.order = BYTE_INTEL;
	format.bits = 16;
	format.fileSize = f_in->Size();

	ogg_sync_state		 foy;
	ogg_stream_state	 fos;
	ogg_page		 fog;
	ogg_packet		 fop;

	vorbis_info		 fvi;
	vorbis_comment		 fvc;

	ex_ogg_sync_init(&foy);

	Int	 size = Math::Min(4096, format.fileSize);
	char	*fbuffer = ex_ogg_sync_buffer(&foy, size);

	f_in->InputData(fbuffer, size);

	ex_ogg_sync_wrote(&foy, size);

	if (foy.data != NIL)
	{
		ex_ogg_sync_pageout(&foy, &fog);

		ex_ogg_stream_init(&fos, ex_ogg_page_serialno(&fog)); 

		ex_vorbis_info_init(&fvi);
		ex_vorbis_comment_init(&fvc);

		ex_ogg_stream_pagein(&fos, &fog);
		ex_ogg_stream_packetout(&fos, &fop);

		ex_vorbis_synthesis_headerin(&fvi, &fvc, &fop);

		Int	 i = 0;

		while (i < 2)
		{
			if (ex_ogg_sync_pageout(&foy, &fog) == 1)
			{
				ex_ogg_stream_pagein(&fos, &fog);

				while (i < 2)
				{
					if (ex_ogg_stream_packetout(&fos, &fop) == 0) break;

					ex_vorbis_synthesis_headerin(&fvi, &fvc, &fop); 

					i++;
				}
			}
			else
			{
				fbuffer = ex_ogg_sync_buffer(&foy, size);

				f_in->InputData(fbuffer, size);

				ex_ogg_sync_wrote(&foy, size);
			}
		}

		format.rate = fvi.rate;
		format.channels = fvi.channels;
		format.length = -1;

		Int	 bitrate = 0;

		if (fvi.bitrate_nominal > 0)				 bitrate = fvi.bitrate_nominal;
 		else if (fvi.bitrate_lower > 0 && fvi.bitrate_upper > 0) bitrate = (fvi.bitrate_lower + fvi.bitrate_upper) / 2;

		if (bitrate > 0) format.approxLength = format.fileSize / (bitrate / 8) * format.rate * format.channels;

		if (fvc.comments > 0)
		{
			format.track = -1;
			format.outfile = NIL;

			char	*prevInFormat = String::SetInputFormat("UTF-8");

			for (Int j = 0; j < fvc.comments; j++)
			{
				String	 comment = String(fvc.user_comments[j]);
				String	 id = String().CopyN(comment, comment.Find("=")).ToUpper();

				if	(id == "TITLE")		format.title	= comment.Tail(comment.Length() - 6);
				else if (id == "ARTIST")	format.artist	= comment.Tail(comment.Length() - 7);
				else if (id == "ALBUM")		format.album	= comment.Tail(comment.Length() - 6);
				else if (id == "GENRE")		format.genre	= comment.Tail(comment.Length() - 6);
				else if (id == "DATE")		format.year	= comment.Tail(comment.Length() - 5).ToInt();
				else if (id == "TRACKNUMBER")	format.track	= comment.Tail(comment.Length() - 12).ToInt();
			}

			String::SetInputFormat(prevInFormat);
		}

		ex_ogg_stream_clear(&fos);

		ex_vorbis_comment_clear(&fvc);
		ex_vorbis_info_clear(&fvi);
	}

	ex_ogg_sync_clear(&foy);

	delete f_in;

	return Success();
}

BoCA::VorbisIn::VorbisIn()
{
	packageSize = 0;
}

BoCA::VorbisIn::~VorbisIn()
{
}

Bool BoCA::VorbisIn::Activate()
{
	ex_ogg_sync_init(&oy);

	Int	 size = 4096;

	inBytes += size;

	buffer = ex_ogg_sync_buffer(&oy, size);

	driver->ReadData((unsigned char *) buffer, size);

	ex_ogg_sync_wrote(&oy, size);

	ex_ogg_sync_pageout(&oy, &og);

	ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og)); 

	ex_vorbis_info_init(&vi);
	ex_vorbis_comment_init(&vc);

	ex_ogg_stream_pagein(&os, &og);
	ex_ogg_stream_packetout(&os, &op);

	ex_vorbis_synthesis_headerin(&vi, &vc, &op);

	int	 i = 0;

	while (i < 2)
	{
		if (ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			ex_ogg_stream_pagein(&os, &og);

			while (i < 2)
			{
				if (ex_ogg_stream_packetout(&os, &op) == 0) break;

				ex_vorbis_synthesis_headerin(&vi, &vc, &op); 

				i++;
			}
		}
		else
		{
			inBytes += size;

			buffer = ex_ogg_sync_buffer(&oy, size);

			driver->ReadData((unsigned char *) buffer, size);

			ex_ogg_sync_wrote(&oy, size);
		}
	}

	ex_vorbis_synthesis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	return True;
}

Bool BoCA::VorbisIn::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	ex_ogg_sync_clear(&oy);

	return True;
}

Int BoCA::VorbisIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	buffer = ex_ogg_sync_buffer(&oy, size);

	size = driver->ReadData((unsigned char *) buffer, size);

	ex_ogg_sync_wrote(&oy, size);

	size = 0;

	Int	 dataBufferLen = 0;

	while (true)
	{
		short	 convbuffer[6144];
		int	 convsize = 6144 / vi.channels;

		if (ex_ogg_sync_pageout(&oy, &og) == 0) break;

		ex_ogg_stream_pagein(&os, &og);

		while (true)
		{
			if (ex_ogg_stream_packetout(&os, &op) == 0) break;

			float  **pcm;
			int	 samples;

			ex_vorbis_synthesis(&vb, &op);
			ex_vorbis_synthesis_blockin(&vd, &vb);

			while ((samples = ex_vorbis_synthesis_pcmout(&vd, &pcm)) > 0)
			{
				Int	 bout = (samples < convsize ? samples : convsize);

				for (Int i = 0; i < vi.channels; i++)
				{
					short	*ptr = convbuffer + i;
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

				if (dataBufferLen < size + (bout * vi.channels * 2))
				{
					dataBufferLen += ((bout * vi.channels * 2) + 131072);

					data.Resize(dataBufferLen);
				}

				memcpy(((unsigned char *) data) + size, convbuffer, bout * vi.channels * 2);

				size += (bout * vi.channels * 2);

				ex_vorbis_synthesis_read(&vd, bout);
			}
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	return size;
}
