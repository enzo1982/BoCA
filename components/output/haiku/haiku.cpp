 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <SoundPlayer.h>

#include "haiku.h"

const String &BoCA::OutputHaiku::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Haiku Output Plugin</name>		\
	    <version>1.0</version>			\
	    <id>haiku-out</id>				\
	    <type>output</type>				\
	  </component>					\
							\
	";

	return componentSpecs;
}

namespace BoCA
{
	void	 PlayBuffer(void *, void *, size_t, const media_raw_audio_format &);
};

BoCA::OutputHaiku::OutputHaiku()
{
	player = NIL;
	paused = False;

	samplesBufferMutex = new Mutex();
}

BoCA::OutputHaiku::~OutputHaiku()
{
	delete samplesBufferMutex;
}

Bool BoCA::OutputHaiku::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Define stream format.
	 */
	const Format		&format = track.GetFormat();
	media_raw_audio_format	 fmt	= media_raw_audio_format::wildcard;

	fmt.buffer_size	  = 2048 * format.channels * (format.bits / 8);
	fmt.byte_order	  = (endianness == EndianBig ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN);
	fmt.frame_rate	  = format.rate;
	fmt.channel_count = format.channels;

	if	(format.bits ==  8) fmt.format = media_raw_audio_format::B_AUDIO_UCHAR;
	else if	(format.bits == 16) fmt.format = media_raw_audio_format::B_AUDIO_SHORT;
	else if	(format.bits == 24) fmt.format = media_raw_audio_format::B_AUDIO_INT;
	else if	(format.bits == 32) fmt.format = media_raw_audio_format::B_AUDIO_INT;

	/* Preallocate sample buffer.
	 */
	samplesBuffer.Resize((format.rate / 4 + 2048) * format.channels * (format.bits / 8));
	samplesBuffer.Resize(0);

	/* Start sound player.
	 */
	player = new BSoundPlayer(&fmt, "Haiku Output Plugin", PlayBuffer, NULL, this);

	player->SetHasData(true);
	player->Start();

	paused = False;

	return True;
}

Bool BoCA::OutputHaiku::Deactivate()
{
	/* Stop sound player.
	 */
	player->Stop();

	delete player;

	player = NIL;

	return True;
}

Int BoCA::OutputHaiku::WriteData(Buffer<UnsignedByte> &data)
{
	samplesBufferMutex->Lock();

	Int	 bytes = samplesBuffer.Size();

	samplesBuffer.Resize(bytes + data.Size());

	memcpy(samplesBuffer + bytes, data, data.Size());

	samplesBufferMutex->Release();

	return data.Size();
}

Int BoCA::OutputHaiku::CanWrite()
{
	if (player == NIL || paused) return 0;

	const Format	&format = track.GetFormat();

	samplesBufferMutex->Lock();

	Int	 canWrite = (format.rate / 4 + 2048) * format.channels * (format.bits / 8) - samplesBuffer.Size();

	samplesBufferMutex->Release();

	return canWrite;
}

Int BoCA::OutputHaiku::SetPause(Bool pause)
{
	if (player == NIL) return Error();

	if (pause) player->Stop(false, false);
	else	   player->Start();

	paused = pause;

	return Success();
}

Bool BoCA::OutputHaiku::IsPlaying()
{
	if (player == NIL) return False;

	samplesBufferMutex->Lock();

	Bool	 isPlaying = (samplesBuffer.Size() > 0);

	samplesBufferMutex->Release();

	return isPlaying;
}

void BoCA::PlayBuffer(void *cookie, void *buffer, size_t size, const media_raw_audio_format &format)
{
	OutputHaiku	*component = (OutputHaiku *) cookie;

	component->samplesBufferMutex->Lock();

	UnsignedInt	 numBytes  = Math::Min(component->samplesBuffer.Size(), size);

	/* Copy samples to output buffer.
	 */
	if (numBytes < size) memset(buffer, 0, size);

	memcpy(buffer, component->samplesBuffer, numBytes);

	/* Move remaining samples to the start of the buffer.
	 */
	memmove(component->samplesBuffer, component->samplesBuffer + numBytes, component->samplesBuffer.Size() - numBytes);

	component->samplesBuffer.Resize(component->samplesBuffer.Size() - numBytes);

	component->samplesBufferMutex->Release();
}
