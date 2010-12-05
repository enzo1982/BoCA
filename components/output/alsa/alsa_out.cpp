 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <alsa/asoundlib.h>

#include "alsa_out.h"

const String &BoCA::ALSAOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	snd_pcm_t	*playback_handle = NIL;

	if (snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) >= 0)
	{
		snd_pcm_close(playback_handle);

		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>ALSA Output Plugin</name>		\
		    <version>1.0</version>			\
		    <id>alsa-out</id>				\
		    <type>output</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

BoCA::ALSAOut::ALSAOut()
{
	playback_handle = NIL;
}

BoCA::ALSAOut::~ALSAOut()
{
}

Bool BoCA::ALSAOut::Activate()
{
	const Format	&format = track.GetFormat();

	if (snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) return False;

	snd_pcm_hw_params_t	*hw_params = NIL;

	snd_pcm_hw_params_malloc(&hw_params);

	snd_pcm_hw_params_any(playback_handle, hw_params);

	snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);

	if	(format.bits ==  8) snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S8);
	else if	(format.bits == 16) snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S16_LE);
	else if	(format.bits == 24) snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S24_LE);
	else if	(format.bits == 32) snd_pcm_hw_params_set_format(playback_handle, hw_params, SND_PCM_FORMAT_S32_LE);

	snd_pcm_hw_params_set_rate(playback_handle, hw_params, format.rate, 0);
	snd_pcm_hw_params_set_channels(playback_handle, hw_params, format.channels);

	snd_pcm_hw_params(playback_handle, hw_params);

	snd_pcm_hw_params_free(hw_params);

	if (snd_pcm_prepare(playback_handle) < 0) return False;

	return True;
}

Bool BoCA::ALSAOut::Deactivate()
{
	snd_pcm_close(playback_handle);

	playback_handle = NIL;

	return True;
}

Int BoCA::ALSAOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	const Format		&format = track.GetFormat();
	snd_pcm_sframes_t	 frames = snd_pcm_writei(playback_handle, data, size / format.channels / (format.bits / 8));

	if (frames < 0) return 0;

	return frames * format.channels * (format.bits / 8);
}

Int BoCA::ALSAOut::CanWrite()
{
	if (playback_handle == NIL) return 0;

	if (snd_pcm_state(playback_handle) == SND_PCM_STATE_PAUSED) return 0;

	const Format		&format = track.GetFormat();
	snd_pcm_sframes_t	 frames = snd_pcm_avail(playback_handle);

	if (frames < 0) return 0;

	return frames * format.channels * (format.bits / 8);
}

Int BoCA::ALSAOut::SetPause(Bool pause)
{
	if (playback_handle == NIL) return Error();

	if (snd_pcm_pause(playback_handle, pause ? 1 : 0) < 0) return Error();

	return Success();
}

Bool BoCA::ALSAOut::IsPlaying()
{
	if (playback_handle == NIL) return False;

	snd_pcm_uframes_t	 bufferSize = 0;
	snd_pcm_uframes_t	 periodSize = 0;

	snd_pcm_get_params(playback_handle, &bufferSize, &periodSize);

	if (snd_pcm_avail(playback_handle) < bufferSize - 1) return True;

	return False;
}
