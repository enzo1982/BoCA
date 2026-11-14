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

#include <smooth/io/drivers/driver_ansi.h>

#include "vorbis.h"

using namespace smooth::IO;

const String &BoCA::DecoderVorbis::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (vorbisdll != NIL && AS::Registry::Get().ComponentExists("ogg-mux"))
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
	LoadVorbisDLL();
}

Void smooth::DetachDLL()
{
	FreeVorbisDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 minOutputSamples = 4096;
};

Bool BoCA::DecoderVorbis::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	/* Check if Vorbis stream.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer == NIL) return False;

	Bool			 result	  = False;
	Buffer<UnsignedByte>	 packet;

	Int64			 position = 0;
	Int			 flags	  = 0;

	if (muxer->ReadPacket(packet, position, flags) >= 7 && packet[0] == 1 && memcmp(packet + 1, "vorbis", 6) == 0) result = True;

	CloseMuxer(muxer);

	return result;
}

Error BoCA::DecoderVorbis::GetStreamInfo(const String &streamURI, Track &track)
{
	/* Open file.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer == NIL) return Error();

	/* Set up track format.
	 */
	Format	 format = track.GetFormat();

	format.bits  = 16;

	track.fileSize = ioDriver.GetSize();

	/* Set up Vorbis decoder.
	 */
	vorbis_info		 vi;
	vorbis_comment		 vc;

	ex_vorbis_info_init(&vi);
	ex_vorbis_comment_init(&vc);

	/* Get stream format.
	 */
	Buffer<UnsignedByte>	 comments;

	for (Int packetNum = 0; packetNum <= 2; packetNum++)
	{
		if (!ReadPacket(muxer)) break;

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

			if (bitrate > 0) track.approxLength = Math::Round(Float(track.fileSize) / (bitrate / 8) * format.rate);
		}

		/* Found Vorbis Comment packet.
		 */
		if (packetNum == 1 && op.bytes >= 7)
		{
			comments.Resize(op.bytes - 7);

			memcpy(comments, op.packet + 7, op.bytes - 7);
		}
	}

	track.SetFormat(format);

	/* Find real length of stream.
	 */
	Int64	 duration = muxer->GetDuration();

	if (duration >= 0) track.length = duration;

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

	CloseMuxer(muxer);

	return Success();
}

BoCA::DecoderVorbis::DecoderVorbis()
{
	muxer	     = NIL;

	packetNumber = 0;
	skipSamples  = 0;

	memset(&op, 0, sizeof(op));

	memset(&vi, 0, sizeof(vi));
	memset(&vc, 0, sizeof(vc));
	memset(&vd, 0, sizeof(vd));
	memset(&vb, 0, sizeof(vb));

	memset(convBuffer, 0, sizeof(convBuffer));
}

BoCA::DecoderVorbis::~DecoderVorbis()
{
}

Bool BoCA::DecoderVorbis::Activate()
{
	/* Open Ogg stream.
	 */
	muxer = OpenMuxer(driver);

	if (muxer == NIL) return False;

	/* Parse header packets and init decoder.
	 */
	ex_vorbis_info_init(&vi);
	ex_vorbis_comment_init(&vc);

	for (Int packetNum = 0; packetNum <= 2; packetNum++)
	{
		if (!ReadPacket(muxer))
		{
			ex_vorbis_comment_clear(&vc);
			ex_vorbis_info_clear(&vi);

			CloseMuxer(muxer);

			muxer = NIL;

			return False;
		}

		ex_vorbis_synthesis_headerin(&vi, &vc, &op);
	}

	inBytes = driver->GetPos();

	ex_vorbis_synthesis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	return True;
}

Bool BoCA::DecoderVorbis::Deactivate()
{
	CloseMuxer(muxer);

	muxer = NIL;

	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	return True;
}

Bool BoCA::DecoderVorbis::Seek(Int64 samplePosition)
{
	/* Seek to the page containing the requested sample position.
	 */
	Int64	 position = muxer->SeekPacket(samplePosition - 1024);

	inBytes = driver->GetPos();

	if (position < 0) return False;

	skipSamples = Math::Max(Int64(0), samplePosition - 1024 - position);

	/* Signal discontinuity to the decoder.
	 */
	packetNumber++;

	ex_vorbis_synthesis_restart(&vd);

	return True;
}

Int BoCA::DecoderVorbis::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format	= track.GetFormat();

	Int	 size	       = 0;
	Int	 dataBufferLen = 0;

	/* Decode packets until we have enough samples.
	 */
	while (size < minOutputSamples * vi.channels * 2)
	{
		if (!ReadPacket(muxer)) break;

		float  **pcm;
		int	 samples;

		if (ex_vorbis_synthesis(&vb, &op) != 0) continue;

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

	inBytes = driver->GetPos();

	/* Bail out if no more data available.
	 */
	if (size == 0) return -1;

	data.Resize(size);

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_7_1, Channel::Default_7_1);

	return size;
}

Bool BoCA::DecoderVorbis::ReadPacket(AS::MuxerComponent *muxer)
{
	/* Read next packet from Ogg stream.
	 */
	Int64	 position   = 0;
	Int	 flags	    = 0;
	Int	 packetSize = muxer->ReadPacket(packetBuffer, position, flags);

	if (packetSize < 0) return False;

	/* Fill packet structure for Vorbis decoder.
	 */
	op.packet     = packetBuffer;
	op.bytes      = packetSize;
	op.b_o_s      = (flags & PACKET_FLAG_BOS) ? 1 : 0;
	op.e_o_s      = (flags & PACKET_FLAG_EOS) ? 1 : 0;
	op.granulepos = position;
	op.packetno   = packetNumber++;

	return True;
}

BoCA::AS::MuxerComponent *BoCA::DecoderVorbis::OpenMuxer(Driver *ioDriver) const
{
	/* Create Ogg muxer and open stream.
	 */
	AS::Registry		&boca  = AS::Registry::Get();
	AS::MuxerComponent	*muxer = (AS::MuxerComponent *) boca.CreateComponentByID("ogg-mux");

	if (muxer == NIL) return NIL;

	muxer->SetConfiguration(GetConfiguration());
	muxer->SetDriver(ioDriver);

	if (!muxer->Open(MUXER_MODE_READ))
	{
		boca.DeleteComponent(muxer);

		return NIL;
	}

	return muxer;
}

Void BoCA::DecoderVorbis::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}
