 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "waveout.h"

#include <mmreg.h>

const String &BoCA::OutputWaveOut::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>WaveOut Output Plugin</name>		\
	    <version>1.0</version>			\
	    <id>waveout-out</id>			\
	    <type>output</type>				\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::OutputWaveOut::OutputWaveOut()
{
	hWaveOut      = NIL;
	hEvent	      = NIL;

	write_ptr     = 0;
	p_time	      = 0;

	paused	      = False;
	needplay      = False;
	stop	      = False;
	newpause      = False;

	buffer	      = NIL;

	buf_size      = 0;
	buf_size_used = 0;

	maxblock      = 0;
	minblock      = 0;
	avgblock      = 0;

	n_playing     = 0;
	data_written  = 0;
	last_time     = 0;

	thread	      = NIL;

	memset(&sync, 0, sizeof(sync));
}

BoCA::OutputWaveOut::~OutputWaveOut()
{
}

Bool BoCA::OutputWaveOut::Activate()
{
	const Format	&format = track.GetFormat();

	hWaveOut	= NIL;

	write_ptr	= 0;
	p_time		= 0;

	paused		= False;
	needplay	= False;
	stop		= False;
	newpause	= False;

	InitializeCriticalSection(&sync);

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
	hEvent = CreateEvent(0, 0, 0, 0);

	MMRESULT	 mr = waveOutOpen(&hWaveOut, WAVE_MAPPER, (WAVEFORMATEX *) &wfx, (DWORD) (Int64) hEvent, 0, CALLBACK_EVENT);

	if (mr)
	{
		String	 e  = String("Unknown MMSYSTEM error.");
		String	 e2 = String("Unknown error.");

		wchar_t	 waveOutError[MAXERRORLENGTH];

		if (waveOutGetErrorText(mr, waveOutError, MAXERRORLENGTH) == MMSYSERR_NOERROR) e = waveOutError;

		switch (mr)
		{
			case 32:
				/* fixme: some broken drivers blow mmsystem032 for no reason, with "standard" 44khz/16bps/stereo, need better error message when pcm format isn't weird
				 */
				e2 = String("Unsupported PCM format (").Append(String::FromInt(format.rate)).Append(" Hz, ").Append(String::FromInt(format.bits)).Append(" bits per sample, ").Append(String::FromInt(format.channels)).Append(" channels). Please change format settings in input plug-in configuration or change output device in waveOut plug-in configuration.");

				break;
			case 4:
				e2 = String("Another program is using your soundcard. Please change output device (in waveOut plug-in configuration), switch to DirectSound output (in Winamp preferences / plug-ins / output) or get a soundcard which can play multiple streams.");

				break;
			case 2:
				e2 = String("No sound devices found. Please buy a soundcard first (or install proper drivers if you think that you have one).\nMore info can be found at http://www.geocities.com/mmsystem002/");

				break;
			case 20:
				e2 = String("Internal driver error; please reboot your computer.");

				break;
			case 7:
				e2 = String("Please reinstall soundcard drivers.");

				break;
			case 8:
				// fixme

				break;
		}

		errorString = String(e2).Append("\n\nError code: ").Append(String::FromInt(mr)).Append("\nWindows error message: \n\"").Append(e).Append("\"");
		errorState = True;

		return False;
	}

	buf_size = MulDiv(2000, wfx.Format.nAvgBytesPerSec, 1000);

	maxblock = 0x10000;
	minblock = 0x100;
	avgblock = buf_size >> 4;

	if (maxblock > buf_size >> 2)	maxblock = buf_size >> 2;
	if (avgblock > maxblock)	avgblock = maxblock;
	if (maxblock < minblock)	maxblock = minblock;
	if (avgblock < minblock)	avgblock = minblock;

	buffer = (char *) LocalAlloc(LMEM_FIXED | LMEM_ZEROINIT, buf_size + maxblock); // extra space at the end of the buffer

	n_playing	= 0;
	data_written	= 0;
	buf_size_used	= 0;
	last_time	= 0;

	waveOutRestart(hWaveOut);

	/* Start worker thread.
	 */
	thread = NonBlocking0<>(&OutputWaveOut::WorkerThread, this).Call();

	return True;
}

Bool BoCA::OutputWaveOut::Deactivate()
{
	EnterCriticalSection(&sync);

	stop = True;

	SetEvent(hEvent);

	LeaveCriticalSection(&sync);

	/* Wait for worker thread to exit.
	 */
	thread->Wait();

	if (hEvent) CloseHandle(hEvent);

	if (hWaveOut)
	{
		waveOutReset(hWaveOut);

		for (Int i = 0; i < headers.Length(); i++)
		{
			WAVEHDR	*hdr = headers.GetNth(i);

			if (hdr->dwFlags & WHDR_PREPARED)
			{
				waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			}

			delete hdr;

			headers.RemoveNth(i--);
		}

		waveOutClose(hWaveOut);
	}

	if (buffer) LocalFree(buffer);

	DeleteCriticalSection(&sync);

	return True;
}

Int BoCA::OutputWaveOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	UnsignedByte	*_data = (UnsignedByte *) data;
	Int		 bytesWritten = 0;

	EnterCriticalSection(&sync);

	while (size > 0)
	{
		UINT	 ptr = (data_written + write_ptr) % buf_size;
		UINT	 delta = size;

		if (ptr + delta > buf_size) delta = buf_size - ptr;

		memcpy(buffer + ptr, _data, delta);

		_data += delta;
		size -= delta;
		bytesWritten += delta;
		data_written += delta;
	}

	if (bytesWritten) SetEvent(hEvent); // new shit, time to update

	LeaveCriticalSection(&sync);

	return bytesWritten;
}

Int BoCA::OutputWaveOut::CanWrite()
{
	EnterCriticalSection(&sync);

	Int	 bytes = paused ? 0 : buf_size - buf_size_used - data_written;

	LeaveCriticalSection(&sync);

	return bytes;
}

Int BoCA::OutputWaveOut::SetPause(Bool pause)
{
	EnterCriticalSection(&sync);

	newpause = pause ? True : False; // needs to be done in our thread to keep stupid win2k/xp happy

	SetEvent(hEvent);

	LeaveCriticalSection(&sync);

	while (paused != newpause) S::System::System::Sleep(0);

	return Success();
}

Bool BoCA::OutputWaveOut::IsPlaying()
{
	if (!hWaveOut) return False;

	EnterCriticalSection(&sync); // needs to be done in our thread

	if (!paused) { needplay = True; SetEvent(hEvent); }

	const Format	&format = track.GetFormat();
	Int		 r = MulDiv(buf_size_used + data_written, 1000, (format.bits >> 3) * format.channels * format.rate);

	if	(paused)    r -= p_time;
	else if (n_playing) r -= timeGetTime() - last_time;

	LeaveCriticalSection(&sync);

	return (r > 0);
}

Void BoCA::OutputWaveOut::WorkerThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	const Format	&format = track.GetFormat();

	while (hWaveOut)
	{
		WaitForSingleObject(hEvent, INFINITE);

		EnterCriticalSection(&sync);

		if (stop) { LeaveCriticalSection(&sync); break; }

		for (Int i = 0; i < headers.Length(); i++)
		{
			WAVEHDR	 *hdr = headers.GetNth(i);

			if (hdr->dwFlags & WHDR_DONE)
			{
				n_playing--;

				buf_size_used -= hdr->dwBufferLength;

				last_time = timeGetTime();

				waveOutUnprepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));

				delete hdr;

				headers.RemoveNth(i--);
			}
		}

		if (!paused && newpause)
		{
			paused = True;

			if (hWaveOut) waveOutPause(hWaveOut);

			p_time = timeGetTime() - last_time;
		}

		if (paused && !newpause)
		{
			paused = False;

			if (hWaveOut) waveOutRestart(hWaveOut);

			last_time = timeGetTime() - p_time;
		}


		UINT limit = 0;

		if	(!needplay && !n_playing && limit < avgblock)	   limit = avgblock;
		else if (buf_size_used < (buf_size >> 1) || n_playing < 3) limit = minblock; // skipping warning, blow whatever we have
		else							   limit = avgblock; // just a block

		while (data_written > limit)
		{
			UINT	 d = (data_written > maxblock) ? maxblock : data_written;

			d -= d % ((format.bits >> 3) * format.channels);

			if (!d) break;

			data_written -= d;
			buf_size_used += d;

			WAVEHDR	*hdr = new WAVEHDR;

			ZeroMemory(hdr, sizeof(WAVEHDR));

			hdr->dwBufferLength = d;
			hdr->dwBytesRecorded = d;
			hdr->lpData = buffer + write_ptr;

			headers.Add(hdr);

			write_ptr += d;

			if (write_ptr > buf_size)
			{
				write_ptr -= buf_size;
				memcpy(buffer + buf_size, buffer, write_ptr);
			}

			n_playing++;

			waveOutPrepareHeader(hWaveOut, hdr, sizeof(WAVEHDR));
			waveOutWrite(hWaveOut, hdr, sizeof(WAVEHDR)); // important: make all waveOutWrite calls from *our* thread to keep win2k/xp happy

			if (n_playing == 1) last_time = timeGetTime();
		}

		needplay = False;

		LeaveCriticalSection(&sync);
	}
}
