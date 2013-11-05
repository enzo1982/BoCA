 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <sys/asoundlib.h>

#include "qsa.h"

const String &BoCA::OutputQSA::GetComponentSpecs()
{
	static String	 componentSpecs;

	snd_pcm_t	*playback_handle = NIL;

	if (snd_pcm_open_preferred(&playback_handle, NIL, NIL, SND_PCM_OPEN_PLAYBACK | SND_PCM_OPEN_NONBLOCK) == 0)
	{
		snd_pcm_close(playback_handle);

		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>QSA Output Plugin</name>		\
		    <version>1.0</version>			\
		    <id>qsa-out</id>				\
		    <type>output</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

BoCA::OutputQSA::OutputQSA()
{
	playback_handle = NIL;

	paused		= False;
}

BoCA::OutputQSA::~OutputQSA()
{
}

Bool BoCA::OutputQSA::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	if (snd_pcm_open(&playback_handle, NIL, NIL, SND_PCM_OPEN_PLAYBACK | SND_PCM_OPEN_NONBLOCK) < 0) return False;

	snd_pcm_channel_params_t	 channel_params;

	memset(&channel_params, 0, sizeof(channel_params));

	channel_params.channel		   = SND_PCM_CHANNEL_PLAYBACK;
	channel_params.mode		   = SND_PCM_MODE_BLOCK;
	channel_params.start_mode	   = SND_PCM_START_DATA;
	channel_params.stop_mode	   = SND_PCM_STOP_ROLLOVER;

	channel_params.format.interleave   = true;
	channel_params.format.rate	   = format.rate;
	channel_params.format.voices	   = format.channels;

	if	(format.bits ==  8) channel_params.format.format =						    SND_PCM_SFMT_U8;
	else if	(format.bits == 16) channel_params.format.format = (endianness == EndianBig ? SND_PCM_SFMT_S16_BE : SND_PCM_SFMT_S16_LE);
	else if	(format.bits == 24) channel_params.format.format = (endianness == EndianBig ? SND_PCM_SFMT_S24_BE : SND_PCM_SFMT_S24_LE);
	else if	(format.bits == 32) channel_params.format.format = (endianness == EndianBig ? SND_PCM_SFMT_S32_BE : SND_PCM_SFMT_S32_LE);

	channel_params.buf.block.frag_size = format.channels * (format.bits / 8);
	channel_params.buf.block.frags_min = 1;
	channel_params.buf.block.frags_max = 1023;

	snd_pcm_channel_params(playback_handle, &channel_params);

	if (snd_pcm_playback_prepare(playback_handle) < 0) return False;

	return True;
}

Bool BoCA::OutputQSA::Deactivate()
{
	snd_pcm_close(playback_handle);

	playback_handle = NIL;

	return True;
}

Int BoCA::OutputQSA::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	ssize_t	 bytes = snd_pcm_write(playback_handle, data, size);

	if (bytes < 0) return 0;

	return bytes;
}

Int BoCA::OutputQSA::CanWrite()
{
	if (playback_handle == NIL) return 0;

	if (paused) return 0;

	snd_pcm_channel_status_t	 channel_status;

	snd_pcm_channel_status(playback_handle, &channel_status);

	return channel_status.free;
}

Int BoCA::OutputQSA::SetPause(Bool pause)
{
	if (playback_handle == NIL) return Error();

	if (pause) snd_pcm_playback_drain(playback_handle);

	paused = pause;

	return Success();
}

Bool BoCA::OutputQSA::IsPlaying()
{
	if (playback_handle == NIL) return False;

	snd_pcm_channel_status_t	 channel_status;

	snd_pcm_channel_status(playback_handle, &channel_status);

	if (channel_status.count > 0) return True;

	return False;
}
