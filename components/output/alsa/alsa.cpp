 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <alsa/asoundlib.h>

#include "alsa.h"

const String &BoCA::OutputALSA::GetComponentSpecs()
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
		    <replace>oss-out</replace>			\
		    <input bits=\"8\" signed=\"false\"/>	\
		    <input bits=\"16,32\"/>			\
		    <input float=\"true\"/>			\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

BoCA::OutputALSA::OutputALSA()
{
	playback_handle = NIL;
}

BoCA::OutputALSA::~OutputALSA()
{
}

Bool BoCA::OutputALSA::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	if (snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0) < 0) return False;

	snd_pcm_hw_params_t	*hw_params = NIL;

	snd_pcm_hw_params_malloc(&hw_params);

	snd_pcm_hw_params_any(playback_handle, hw_params);

	snd_pcm_hw_params_set_access(playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_buffer_size(playback_handle, hw_params, format.rate * format.channels * (format.bits / 8) / 4);

	if	(format.bits ==  8		) snd_pcm_hw_params_set_format(playback_handle, hw_params,						       SND_PCM_FORMAT_U8);
	else if	(format.bits == 16		) snd_pcm_hw_params_set_format(playback_handle, hw_params, endianness == EndianBig ? SND_PCM_FORMAT_S16_BE   : SND_PCM_FORMAT_S16_LE);
	else if	(format.bits == 32 && !format.fp) snd_pcm_hw_params_set_format(playback_handle, hw_params, endianness == EndianBig ? SND_PCM_FORMAT_S32_BE   : SND_PCM_FORMAT_S32_LE);
	else if	(format.bits == 32 &&  format.fp) snd_pcm_hw_params_set_format(playback_handle, hw_params, endianness == EndianBig ? SND_PCM_FORMAT_FLOAT_BE : SND_PCM_FORMAT_FLOAT_LE);

	snd_pcm_hw_params_set_rate(playback_handle, hw_params, format.rate, 0);
	snd_pcm_hw_params_set_channels(playback_handle, hw_params, format.channels);

	snd_pcm_hw_params(playback_handle, hw_params);

	snd_pcm_hw_params_free(hw_params);

	if (snd_pcm_prepare(playback_handle) < 0) return False;

	return True;
}

Bool BoCA::OutputALSA::Deactivate()
{
	snd_pcm_close(playback_handle);

	playback_handle = NIL;

	return True;
}

Int BoCA::OutputALSA::WriteData(Buffer<UnsignedByte> &data)
{
	const Format		&format = track.GetFormat();
	snd_pcm_sframes_t	 frames = -1;

	/* Change to ALSA channel order.
	 */
	if	(format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::ALSA_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::ALSA_5_1);

	/* Output samples.
	 */
	frames = snd_pcm_writei(playback_handle, data, data.Size() / format.channels / (format.bits / 8));

	if (frames < 0) return 0;

	return frames * format.channels * (format.bits / 8);
}

Bool BoCA::OutputALSA::Finish()
{
	if (playback_handle == NIL) return False;

	if (snd_pcm_drain(playback_handle) < 0) return False;

	return True;
}

Int BoCA::OutputALSA::CanWrite()
{
	if (playback_handle == NIL) return 0;

	if (snd_pcm_state(playback_handle) == SND_PCM_STATE_PAUSED) return 0;

	const Format		&format = track.GetFormat();
	snd_pcm_sframes_t	 frames = snd_pcm_avail(playback_handle);

	if (frames < 0) return 0;

	return frames * format.channels * (format.bits / 8);
}

Int BoCA::OutputALSA::SetPause(Bool pause)
{
	if (playback_handle == NIL) return Error();

	if (snd_pcm_pause(playback_handle, pause ? 1 : 0) < 0) return Error();

	return Success();
}

Bool BoCA::OutputALSA::IsPlaying()
{
	if (playback_handle == NIL) return False;

	if (snd_pcm_state(playback_handle) == SND_PCM_STATE_RUNNING) return True;

	return False;
}
