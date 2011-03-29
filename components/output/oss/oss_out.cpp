 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <sys/soundcard.h>

#include <unistd.h>
#include <fcntl.h>

#include "oss_out.h"

const String &BoCA::OSSOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	int		 device_fd = NIL;

	if ((device_fd = open("/dev/dsp", O_WRONLY, 0)) >= 0)
	{
		close(device_fd);

		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>OSS Output Plugin</name>		\
		    <version>1.0</version>			\
		    <id>oss-out</id>				\
		    <type>output</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

BoCA::OSSOut::OSSOut()
{
	device_fd = -1;

	paused	  = False;
}

BoCA::OSSOut::~OSSOut()
{
}

Bool BoCA::OSSOut::Activate()
{
	const Format	&format = track.GetFormat();

	if ((device_fd = open("/dev/dsp", O_WRONLY, 0)) < 0) return False;

	int	 samples  = 0;

	if	(format.bits ==  8) samples = AFMT_U8;
	else if	(format.bits == 16) samples = AFMT_S16_LE;
	else if	(format.bits == 24) samples = AFMT_S32_LE;
	else if	(format.bits == 32) samples = AFMT_S32_LE;

	int	 channels = format.channels;
	int	 rate	  = format.rate;

	if (ioctl(device_fd, SNDCTL_DSP_SETFMT, &samples)    == -1) return False;
	if (ioctl(device_fd, SNDCTL_DSP_CHANNELS, &channels) == -1) return False;
	if (ioctl(device_fd, SNDCTL_DSP_SPEED, &rate)	     == -1) return False;

	return True;
}

Bool BoCA::OSSOut::Deactivate()
{
	close(device_fd);

	device_fd = -1;

	return True;
}

Int BoCA::OSSOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	const Format	&format = track.GetFormat();
	Int		 bytes = -1;

	if (format.bits == 24)
	{
		/* Convert 24 bit samples to 32 bit.
		 */
		Buffer<Int32>	 samples(size / (format.bits / 8));

		for (Int i = 0; i < samples.Size(); i++) samples[i] = (data[3 * i] + (data[3 * i + 1] << 8) + (data[3 * i + 2] << 16)) * 256;

		bytes = write(device_fd, samples, size / (format.bits / 8) * sizeof(Int32));
	}
	else
	{
		bytes = write(device_fd, data, size);
	}

	if (bytes < 0) return 0;

	return bytes;
}

Int BoCA::OSSOut::CanWrite()
{
	if (device_fd == -1) return 0;

	if (paused) return 0;

	const Format	&format = track.GetFormat();

	return 2048 * format.channels * (format.bits / 8);
}

Int BoCA::OSSOut::SetPause(Bool pause)
{
	if (device_fd == -1) return Error();

	if (pause) ioctl(device_fd, SNDCTL_DSP_SKIP, NIL);

	paused = pause;

	return Success();
}

Bool BoCA::OSSOut::IsPlaying()
{
	if (device_fd == -1) return False;

	oss_count_t	 ptr;

	if (ioctl(device_fd, SNDCTL_DSP_CURRENT_OPTR, &ptr) == 0)
	{
		if (ptr.fifo_samples > 0) return True;
	}

	return False;
}
