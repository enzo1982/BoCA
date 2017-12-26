 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifdef __OpenBSD__
#	include <soundcard.h>
#else
#	include <sys/soundcard.h>
#endif

#include <unistd.h>
#include <fcntl.h>

#include "oss.h"

const String &BoCA::OutputOSS::GetComponentSpecs()
{
	static String	 componentSpecs;

	int		 device_fd = NIL;

#if defined __OpenBSD__ || defined __NetBSD__
	if ((device_fd = open("/dev/sound", O_WRONLY, 0)) >= 0)
#else
	if ((device_fd = open("/dev/dsp", O_WRONLY, 0)) >= 0)
#endif
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
		    <input bits=\"8\" signed=\"false\"/>	\
		    <input bits=\"16\"/>			\
								\
		";

#ifdef AFMT_S32_LE
		componentSpecs.Append("				\
								\
		    <input bits=\"32\"/>			\
								\
		");
#endif

		componentSpecs.Append("				\
								\
		  </component>					\
								\
		");
	}

	return componentSpecs;
}

BoCA::OutputOSS::OutputOSS()
{
	device_fd = -1;

	paused	  = False;
}

BoCA::OutputOSS::~OutputOSS()
{
}

Bool BoCA::OutputOSS::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

#if defined __OpenBSD__ || defined __NetBSD__
	if ((device_fd = open("/dev/sound", O_WRONLY, 0)) < 0) return False;
#else
	if ((device_fd = open("/dev/dsp", O_WRONLY, 0)) < 0) return False;
#endif

	int	 samples  = 0;

	if	(format.bits ==  8) samples =					       AFMT_U8;
	else if	(format.bits == 16) samples = (endianness == EndianBig ? AFMT_S16_BE : AFMT_S16_LE);
#ifdef AFMT_S32_LE
	else if	(format.bits == 32) samples = (endianness == EndianBig ? AFMT_S32_BE : AFMT_S32_LE);
#endif

	int	 channels = format.channels;
	int	 rate	  = format.rate;

	if (ioctl(device_fd, SNDCTL_DSP_SETFMT, &samples)    == -1) return False;
	if (ioctl(device_fd, SNDCTL_DSP_CHANNELS, &channels) == -1) return False;
	if (ioctl(device_fd, SNDCTL_DSP_SPEED, &rate)	     == -1) return False;

	return True;
}

Bool BoCA::OutputOSS::Deactivate()
{
	close(device_fd);

	device_fd = -1;

	return True;
}

Int BoCA::OutputOSS::WriteData(Buffer<UnsignedByte> &data)
{
	Int	 bytes = write(device_fd, data, data.Size());

	if (bytes < 0) return 0;

	return bytes;
}

Int BoCA::OutputOSS::CanWrite()
{
	if (device_fd == -1) return 0;

	if (paused) return 0;

	const Format	&format = track.GetFormat();

	return format.rate * format.channels * (format.bits / 8) / 4;
}

Int BoCA::OutputOSS::SetPause(Bool pause)
{
	if (device_fd == -1) return Error();

#ifdef SNDCTL_DSP_SKIP
	if (pause) ioctl(device_fd, SNDCTL_DSP_SKIP, NIL);
#endif

	paused = pause;

	return Success();
}

Bool BoCA::OutputOSS::IsPlaying()
{
	if (device_fd == -1) return False;

#ifdef SNDCTL_DSP_CURRENT_OPTR
	oss_count_t	 ptr;

	if (ioctl(device_fd, SNDCTL_DSP_CURRENT_OPTR, &ptr) == 0)
	{
		if (ptr.fifo_samples > 0) return True;
	}
#else
	count_info	 ci;

	if (ioctl(device_fd, SNDCTL_DSP_GETOPTR, &ci) == 0)
	{
		if (ci.ptr > 0) return True;
	}
#endif

	return False;
}
