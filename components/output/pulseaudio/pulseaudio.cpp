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

#include <smooth.h>
#include <smooth/dll.h>

#include <pulse/pulseaudio.h>

#include "pulseaudio.h"

const String &BoCA::OutputPulseAudio::GetComponentSpecs()
{
	static String	 componentSpecs;

	Initialize();

	if (context != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>PulseAudio Output Plugin</name>	\
		    <version>1.0</version>			\
		    <id>pulseaudio-out</id>			\
		    <type>output</type>				\
		    <replace>alsa-out</replace>			\
		    <replace>oss-out</replace>			\
		    <input bits=\"8\" signed=\"false\"/>	\
		    <input bits=\"16,32\"/>			\
		    <input float=\"true\"/>			\
		  </component>					\
								\
		";
	}

	Cleanup();

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	/* Register initialization and cleanup handlers.
	 */
	BoCA::Engine	*engine = BoCA::Engine::Get();

	engine->onInitialize.Connect(&BoCA::OutputPulseAudio::Initialize);
	engine->onCleanup.Connect(&BoCA::OutputPulseAudio::Cleanup);
}

Void smooth::DetachDLL()
{
	/* Unregister initialization and cleanup handlers.
	 */
	BoCA::Engine	*engine = BoCA::Engine::Get();

	engine->onInitialize.Disconnect(&BoCA::OutputPulseAudio::Initialize);
	engine->onCleanup.Disconnect(&BoCA::OutputPulseAudio::Cleanup);
}

pa_threaded_mainloop	*BoCA::OutputPulseAudio::mainloop = NIL;
pa_context		*BoCA::OutputPulseAudio::context  = NIL;

Void BoCA::OutputPulseAudio::Initialize()
{
	String	 applicationName = Application::Get()->getScreenName.Call();

	/* Connect to PulseAudio server.
	 */
	mainloop = pa_threaded_mainloop_new();
	context	 = pa_context_new(pa_threaded_mainloop_get_api(mainloop), applicationName);

	if (pa_context_connect(context, NIL, PA_CONTEXT_NOFLAGS, NIL) >= 0)
	{
		/* Start threaded main loop.
		 */
		pa_threaded_mainloop_start(mainloop);

		/* Wait for context to become ready.
		 */
		while (true)
		{
			pa_threaded_mainloop_lock(mainloop);

			pa_context_state_t	 state = pa_context_get_state(context);

			pa_threaded_mainloop_unlock(mainloop);

			if (state == PA_CONTEXT_READY) return;
			if (!PA_CONTEXT_IS_GOOD(state)) break;
		}
	}

	/* Free context on error.
	 */
	pa_context_unref(context);

	context = NIL;
}

Void BoCA::OutputPulseAudio::Cleanup()
{
	/* Disconnect context.
	 */
	if (context != NIL)
	{
		pa_threaded_mainloop_lock(mainloop);

		pa_context_disconnect(context);
		pa_context_unref(context);

		pa_threaded_mainloop_unlock(mainloop);
	}

	/* Stop threaded main loop.
	 */
	pa_threaded_mainloop_stop(mainloop);
	pa_threaded_mainloop_free(mainloop);

	mainloop = NIL;
	context	 = NIL;
}

BoCA::OutputPulseAudio::OutputPulseAudio()
{
	stream = NIL;
}

BoCA::OutputPulseAudio::~OutputPulseAudio()
{
}

Bool BoCA::OutputPulseAudio::Activate()
{
	String	 applicationName = Application::Get()->getScreenName.Call();

	/* Fill sample format spec.
	 */
	const Format	&format = track.GetFormat();
	pa_sample_spec	 sampleSpec;

	sampleSpec.channels = format.channels;
	sampleSpec.rate	= format.rate;

	if	(format.bits ==  8		) sampleSpec.format = PA_SAMPLE_U8;
	else if	(format.bits == 16		) sampleSpec.format = PA_SAMPLE_S16NE;
	else if	(format.bits == 32 && !format.fp) sampleSpec.format = PA_SAMPLE_S32NE;
	else if	(format.bits == 32 &&  format.fp) sampleSpec.format = PA_SAMPLE_FLOAT32NE;

	/* Create channel map.
	 */
	pa_channel_map	 channelMap;

	pa_channel_map_init_auto(&channelMap, sampleSpec.channels, PA_CHANNEL_MAP_WAVEEX);

	/* Create stream.
	 */
	stream = pa_stream_new(context, applicationName, &sampleSpec, &channelMap);

	/* Connect stream to playback.
	 */
	pa_buffer_attr	 bufferAttr;

	bufferAttr.maxlength = (uint32_t) -1;
	bufferAttr.tlength   = format.rate / 4 * format.channels * (format.bits / 8);
	bufferAttr.prebuf    = (uint32_t) -1;
	bufferAttr.minreq    = (uint32_t) -1;
	bufferAttr.fragsize  = (uint32_t) -1;

	pa_threaded_mainloop_lock(mainloop);
	pa_stream_connect_playback(stream, NIL, &bufferAttr, PA_STREAM_NOFLAGS, NIL, NIL);
	pa_threaded_mainloop_unlock(mainloop);

	/* Wait for stream to become ready.
	 */
	while (true)
	{
		pa_threaded_mainloop_lock(mainloop);

		pa_stream_state_t	 state = pa_stream_get_state(stream);

		pa_threaded_mainloop_unlock(mainloop);

		if (state == PA_STREAM_READY)  break;
		if (!PA_STREAM_IS_GOOD(state)) return False;
	}

	return True;
}

Bool BoCA::OutputPulseAudio::Deactivate()
{
	/* Disconnect stream.
	 */
	if (stream != NIL)
	{
		pa_threaded_mainloop_lock(mainloop);

		pa_stream_disconnect(stream);
		pa_stream_unref(stream);

		pa_threaded_mainloop_unlock(mainloop);
	}

	stream = NIL;

	return True;
}

Int BoCA::OutputPulseAudio::WriteData(Buffer<UnsignedByte> &data)
{
	if (stream == NIL) return 0;

	/* Output samples.
	 */
	pa_threaded_mainloop_lock(mainloop);

	Int	 size = data.Size();

	if (pa_stream_write(stream, data, data.Size(), NIL, 0, PA_SEEK_RELATIVE) < 0) size = 0;

	pa_threaded_mainloop_unlock(mainloop);

	return size;
}

Bool BoCA::OutputPulseAudio::Finish()
{
	if (stream == NIL) return False;

	pa_threaded_mainloop_lock(mainloop);

	pa_operation	*operation = pa_stream_drain(stream, NIL, NIL);

	if (operation == NIL)
	{
		pa_threaded_mainloop_unlock(mainloop);

		return False;
	}

	pa_operation_unref(operation);

	pa_threaded_mainloop_unlock(mainloop);

	return True;
}

Int BoCA::OutputPulseAudio::CanWrite()
{
	if (stream == NIL) return 0;

	pa_threaded_mainloop_lock(mainloop);

	size_t	 size = pa_stream_writable_size(stream);

	pa_threaded_mainloop_unlock(mainloop);

	return size;
}

Int BoCA::OutputPulseAudio::SetPause(Bool pause)
{
	if (stream == NIL) return Error();

	pa_threaded_mainloop_lock(mainloop);

	pa_operation	*operation = pa_stream_cork(stream, pause ? 1 : 0, NIL, NIL);

	if (operation == NIL)
	{
		pa_threaded_mainloop_unlock(mainloop);

		return Error();
	}

	pa_operation_unref(operation);

	if (!pause)
	{
		/* Resume playback immediately after pause.
		 */
		pa_operation	*operation = pa_stream_trigger(stream, NIL, NIL);

		if (operation != NIL) pa_operation_unref(operation);
	}

	pa_threaded_mainloop_unlock(mainloop);

	return Success();
}

Bool BoCA::OutputPulseAudio::IsPlaying()
{
	if (stream == NIL) return False;

	pa_threaded_mainloop_lock(mainloop);

	Bool			 playing    = False;
	const pa_buffer_attr	*attributes = pa_stream_get_buffer_attr(stream);

	if (pa_stream_writable_size(stream) < attributes->tlength) playing = True;

	pa_threaded_mainloop_unlock(mainloop);

	return playing;
}
