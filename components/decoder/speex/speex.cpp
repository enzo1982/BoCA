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

#include <stdlib.h>

#include "speex.h"

using namespace smooth::IO;

const String &BoCA::DecoderSpeex::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (speexdll != NIL && AS::Registry::Get().ComponentExists("ogg-mux"))
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Speex Speech Decoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>speex-dec</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Speex Files</name>						\
		      <extension>spx</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";

		const char	*speexVersion = NIL;

		ex_speex_lib_ctl(SPEEX_LIB_GET_VERSION_STRING, &speexVersion);

		componentSpecs.Replace("%VERSION%", String("v").Append(speexVersion));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadSpeexDLL();
}

Void smooth::DetachDLL()
{
	FreeSpeexDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 minOutputSamples = 4096;
};

Bool BoCA::DecoderSpeex::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".spx");
}

Error BoCA::DecoderSpeex::GetStreamInfo(const String &streamURI, Track &track)
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

	/* Get stream format.
	 */
	Buffer<UnsignedByte>	 comments;
	Buffer<UnsignedByte>	 packet;

	for (Int packetNum = 0; packetNum <= 1; packetNum++)
	{
		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(packet, position, flags);

		if (packetSize < 0) break;

		/* Found header packet.
		 */
		if (packetNum == 0)
		{
			SpeexHeader	*header = ex_speex_packet_to_header((char *) (UnsignedByte *) packet, packetSize);

			if (header == NIL) break;

			format.rate	= header->rate;
			format.channels	= header->nb_channels;

			track.length = -1;

			free(header);
		}

		/* Found Vorbis Comment packet.
		 */
		if (packetNum == 1)
		{
			comments.Resize(packetSize);

			memcpy(comments, packet, packetSize);
		}
	}

	track.SetFormat(format);

	/* Find real length of stream.
	 */
	Int64	 duration = muxer->GetDuration();

	if (duration >= 0) track.length = duration;

	CloseMuxer(muxer);

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

	return Success();
}

BoCA::DecoderSpeex::DecoderSpeex()
{
	muxer	    = NIL;
	decoder	    = NIL;

	frameSize   = 0;
	lookAhead   = 0;
	nFrames	    = 0;

	preSkip	    = 0;
	preSkipLeft = 0;

	skipSamples = 0;

	firstPage   = True;

	memset(&bits, 0, sizeof(bits));
	memset(&stereo, 0, sizeof(stereo));
}

BoCA::DecoderSpeex::~DecoderSpeex()
{
}

Bool BoCA::DecoderSpeex::Activate()
{
	/* Open Ogg stream.
	 */
	muxer = OpenMuxer(driver);

	if (muxer == NIL) return False;

	/* Parse header packets.
	 */
	SpeexHeader	*header = NIL;

	for (Int packetNum = 0; header == NIL || packetNum <= 1 + header->extra_headers; packetNum++)
	{
		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(packetBuffer, position, flags);

		if (packetSize >= 0 && packetNum == 0) header = ex_speex_packet_to_header((char *) (UnsignedByte *) packetBuffer, packetSize);

		if (packetSize < 0 || header == NIL)
		{
			if (header != NIL) free(header);

			CloseMuxer(muxer);

			muxer = NIL;

			return False;
		}
	}

	inBytes = driver->GetPos();

	/* Init decoder.
	 */
	const SpeexMode	*mode = ex_speex_lib_get_mode(header->mode);

	decoder = ex_speex_decoder_init(mode);

	ex_speex_decoder_ctl(decoder, SPEEX_SET_SAMPLING_RATE, &header->rate);
	ex_speex_decoder_ctl(decoder, SPEEX_GET_FRAME_SIZE, &frameSize);
	ex_speex_decoder_ctl(decoder, SPEEX_GET_LOOKAHEAD, &lookAhead);

	nFrames	  = Math::Max(1, (Int) header->frames_per_packet);
	firstPage = True;

	stereo.balance	    = 1;
	stereo.e_ratio	    = 0.5;
	stereo.smooth_left  = 1;
	stereo.smooth_right = 1;
	stereo.reserved1    = 0;
	stereo.reserved2    = 0;

	ex_speex_bits_init(&bits);

	free(header);

	return True;
}

Bool BoCA::DecoderSpeex::Deactivate()
{
	CloseMuxer(muxer);

	muxer = NIL;

	ex_speex_decoder_destroy(decoder);
	ex_speex_bits_destroy(&bits);

	return True;
}

Bool BoCA::DecoderSpeex::Seek(Int64 samplePosition)
{
	/* Seek to the page containing the requested sample position.
	 */
	Int64	 position = muxer->SeekPacket(samplePosition);

	inBytes = driver->GetPos();

	if (position < 0) return False;

	skipSamples  = samplePosition - position;
	preSkipLeft += skipSamples;

	ex_speex_decoder_ctl(decoder, SPEEX_RESET_STATE, 0);

	return True;
}

Int BoCA::DecoderSpeex::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Read all packets of the first page to
	 * find the number of samples to skip.
	 */
	if (firstPage)
	{
		Int64	 position = -1;

		while (position == -1)
		{
			Int	 flags	    = 0;
			Int	 packetSize = muxer->ReadPacket(packetBuffer, position, flags);

			if (packetSize < 0) break;

			pendingPackets.Resize(pendingPackets.Size() + packetSize);

			memcpy(pendingPackets + pendingPackets.Size() - packetSize, packetBuffer, packetSize);

			pendingPacketSizes.Add(packetSize);
		}

		Int	 pageSamples = pendingPacketSizes.Length() * nFrames * frameSize;

		if (position != -1 && position < pageSamples) preSkip += pageSamples - position;

		preSkip	    += lookAhead;
		preSkipLeft += preSkip;

		firstPage = False;
	}

	/* Decode packets until we have enough samples.
	 */
	Int	 size	       = 0;
	Int	 dataBufferLen = 0;
	Int	 pendingOffset = 0;

	while (size < minOutputSamples * format.channels * 2)
	{
		/* Get next packet.
		 */
		UnsignedByte	*packet	    = NIL;
		Int		 packetSize = 0;

		if (pendingPacketSizes.Length() > 0)
		{
			packet	       = pendingPackets + pendingOffset;
			packetSize     = pendingPacketSizes.GetNth(0);

			pendingOffset += packetSize;

			pendingPacketSizes.RemoveNth(0);
		}
		else
		{
			Int64	 position = 0;
			Int	 flags	  = 0;

			packetSize = muxer->ReadPacket(packetBuffer, position, flags);

			if (packetSize < 0) break;

			packet	   = packetBuffer;
		}

		/* Decode packet.
		 */
		pcmBuffer.Resize(frameSize * format.channels);

		ex_speex_bits_read_from(&bits, (char *) packet, packetSize);

		for (Int i = 0; i < nFrames; i++)
		{
			if (ex_speex_decode_int(decoder, &bits, pcmBuffer) < 0) break;

			if (format.channels == 2) ex_speex_decode_stereo_int(pcmBuffer, frameSize, &stereo);

			if (frameSize > preSkipLeft)
			{
				if (preSkipLeft) memmove((short *) pcmBuffer, (short *) pcmBuffer + preSkipLeft * format.channels, (frameSize - preSkipLeft) * format.channels * 2);

				if (dataBufferLen < size + ((frameSize - preSkipLeft) * format.channels * 2))
				{
					dataBufferLen += (((frameSize - preSkipLeft) * format.channels * 2) + 131072);

					data.Resize(dataBufferLen);
				}

				for (Int j = 0; j < (frameSize - preSkipLeft) * format.channels; j++)
				{
					((short *) (UnsignedByte *) data)[size / 2 + j] = pcmBuffer[j];
				}

				size += ((frameSize - preSkipLeft) * format.channels * 2);
			}

			preSkipLeft = Math::Max(0, preSkipLeft - frameSize);
		}
	}

	/* Keep pending packets that have not been decoded yet.
	 */
	if (pendingPacketSizes.Length() > 0)
	{
		memmove(pendingPackets, pendingPackets + pendingOffset, pendingPackets.Size() - pendingOffset);

		pendingPackets.Resize(pendingPackets.Size() - pendingOffset);
	}
	else
	{
		pendingPackets.Resize(0);
	}

	inBytes = driver->GetPos();

	/* Bail out if no more data available.
	 */
	if (size == 0) return -1;

	data.Resize(size);

	return size;
}

BoCA::AS::MuxerComponent *BoCA::DecoderSpeex::OpenMuxer(Driver *ioDriver) const
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

Void BoCA::DecoderSpeex::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}
