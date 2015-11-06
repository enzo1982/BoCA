 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "directsound.h"

#include <mmreg.h>

const String &BoCA::OutputDirectSound::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>DirectSound Output Plugin</name>	\
	    <version>1.0</version>			\
	    <id>directsound-out</id>			\
	    <type>output</type>				\
	    <precede>waveout-out</precede>		\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::OutputDirectSound::OutputDirectSound()
{
	directSound   = NIL;

	primaryBuffer = NIL;
	workBuffer    = NIL;

	bufferSize    = 0;
	bufferPos     = 0;

	firstBlock    = True;
}

BoCA::OutputDirectSound::~OutputDirectSound()
{
}

Bool BoCA::OutputDirectSound::Activate()
{
	const Format	&format = track.GetFormat();

	/* Set helper variables.
	 */
	firstBlock = True;
	bufferSize = format.rate * format.channels * (format.bits / 8) / 4;

	/* Init wave format descriptor.
	 */
	WAVEFORMATEXTENSIBLE	 wfx;

	ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));

	wfx.Format.nSamplesPerSec	= format.rate;
	wfx.Format.wBitsPerSample	= format.bits;
	wfx.Format.nChannels		= format.channels;

	wfx.Format.cbSize		= sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	wfx.Format.wFormatTag		= WAVE_FORMAT_EXTENSIBLE;
	wfx.Format.nBlockAlign		= (wfx.Format.wBitsPerSample >> 3) * wfx.Format.nChannels;
	wfx.Format.nAvgBytesPerSec	= wfx.Format.nBlockAlign * wfx.Format.nSamplesPerSec;

	wfx.SubFormat			= KSDATAFORMAT_SUBTYPE_PCM;

	wfx.Samples.wValidBitsPerSample = wfx.Format.wBitsPerSample;

	if	(format.channels == 1) wfx.dwChannelMask = SPEAKER_FRONT_CENTER;
	else if (format.channels == 2) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	else if (format.channels == 3) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER;
	else if (format.channels == 4) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
	else if (format.channels == 5) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
	else if (format.channels == 6) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
	else if (format.channels == 7) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_CENTER | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;
	else if (format.channels == 8) wfx.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;

	/* Open audio device.
	 */
	DirectSoundCreate8(&DSDEVID_DefaultPlayback, &directSound, NIL);

	directSound->SetCooperativeLevel((HWND) Window::GetNthWindow(0)->GetSystemWindow(), DSSCL_PRIORITY);

	/* Create primary sound buffer.
	 */
	DSBUFFERDESC	 soundBuffDesc;

	ZeroMemory(&soundBuffDesc, sizeof(DSBUFFERDESC));

	soundBuffDesc.dwSize	    = sizeof(DSBUFFERDESC);
	soundBuffDesc.dwFlags	    = DSBCAPS_PRIMARYBUFFER;

	if (directSound->CreateSoundBuffer(&soundBuffDesc, &primaryBuffer, NIL) != DS_OK)
	{
		directSound->Release();

		return False;
	}

	if (primaryBuffer->SetFormat((WAVEFORMATEX *) &wfx) != DS_OK)
	{
		primaryBuffer->Release(); 

		directSound->Release();

		return False;
	}

	/* Create secondary sound buffer.
	 */
	soundBuffDesc.dwFlags	    = DSBCAPS_GLOBALFOCUS;
	soundBuffDesc.dwBufferBytes = bufferSize;
	soundBuffDesc.lpwfxFormat   = (WAVEFORMATEX *) &wfx;

	if (directSound->CreateSoundBuffer(&soundBuffDesc, &workBuffer, NIL) != DS_OK)
	{
		primaryBuffer->Release(); 

		directSound->Release();

		return False;
	}

	return True;
}

Bool BoCA::OutputDirectSound::Deactivate()
{
	workBuffer->Stop();

	workBuffer->Release(); 
	primaryBuffer->Release(); 

	directSound->Release();

	return True;
}

Int BoCA::OutputDirectSound::WriteData(Buffer<UnsignedByte> &data)
{
	Int	 size	      = data.Size();
	Int	 bytesWritten = 0;

	/* Loop until all bytes are fed to the buffer.
	 */
	while (size > 0)
	{
		/* Get current playback position.
		 */
		DWORD	 playCursor = bufferSize;

		if (!firstBlock) workBuffer->GetCurrentPosition(&playCursor, NIL);

		if (playCursor == bufferPos) S::System::System::Sleep(25);

		/* Lock buffer and feed samples.
		 */
		HRESULT	 hr	    = DS_OK;

		Void	*block1	    = NIL;
		Void	*block2	    = NIL;

		DWORD	 block1Size = 0;
		DWORD	 block2Size = 0;

		if (playCursor >= bufferPos) hr = workBuffer->Lock(bufferPos, Math::Min(size, playCursor - bufferPos),		    &block1, &block1Size,  NIL,	    NIL,	0);
		else			     hr = workBuffer->Lock(bufferPos, Math::Min(size, playCursor - bufferPos + bufferSize), &block1, &block1Size, &block2, &block2Size, 0);

		if (hr == DS_OK)
		{
			if (block1 != NIL) memcpy(block1, data + bytesWritten,		    block1Size);
			if (block2 != NIL) memcpy(block2, data + bytesWritten + block1Size, block2Size);

			workBuffer->Unlock(block1, block1Size, block2, block2Size);

			size	     -= block1Size + block2Size;

			bufferPos    += block1Size + block2Size;
			bytesWritten += block1Size + block2Size;

			if (bufferPos >= bufferSize) bufferPos -= bufferSize;
		}

		/* Start playback.
		 */
		if (firstBlock) workBuffer->Play(0, 0, DSBPLAY_LOOPING);

		firstBlock = False;
	}

	return bytesWritten;
}

Bool BoCA::OutputDirectSound::Finish()
{
	/* Fill buffer with null samples.
	 */
	Buffer<UnsignedByte>	 buffer(bufferSize);

	memset(buffer, 0, bufferSize);

	WriteData(buffer);

	/* Stop playing the buffer as we must have
	 * reached the null samples at this point.
	 */
	workBuffer->Stop();

	return True;
}

Int BoCA::OutputDirectSound::CanWrite()
{
	return bufferSize;
}

Int BoCA::OutputDirectSound::SetPause(Bool pause)
{
	if (pause) workBuffer->Stop();
	else	   workBuffer->Play(0, 0, DSBPLAY_LOOPING);

	return Success();
}

Bool BoCA::OutputDirectSound::IsPlaying()
{
	if (workBuffer == NIL) return False;

	/* Get and check buffer status.
	 */
	DWORD	 status = 0;

	workBuffer->GetStatus(&status);

	if (status & DSBSTATUS_PLAYING) return True;
	else				return False;
}
