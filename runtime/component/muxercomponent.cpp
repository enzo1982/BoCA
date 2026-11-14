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

#include <boca/component/muxercomponent.h>

BoCA::CS::MuxerComponent::MuxerComponent()
{
	driver		= NIL;

	codec		= MUXER_CODEC_UNKNOWN;

	format.rate	= 0;
	format.channels	= 0;
	format.bits	= 0;

	timeScale	= 0;

	gaplessInfo	= False;

	gaplessDelay	= 0;
	gaplessPadding	= 0;
	gaplessLength	= 0;
}

BoCA::CS::MuxerComponent::~MuxerComponent()
{
}

Bool BoCA::CS::MuxerComponent::SetAudioTrackInfo(const Track &track)
{
	this->track = track;

	return True;
}

Void BoCA::CS::MuxerComponent::SetDriver(IO::Driver *driver)
{
	this->driver = driver;
}

const BoCA::Format &BoCA::CS::MuxerComponent::GetFormatInfo() const
{
	return format;
}

BoCA::MuxerCodec BoCA::CS::MuxerComponent::GetCodec() const
{
	return codec;
}

Bool BoCA::CS::MuxerComponent::SetCodec(MuxerCodec nCodec)
{
	codec = nCodec;

	return True;
}

Bool BoCA::CS::MuxerComponent::GetCodecConfig(Buffer<UnsignedByte> &buffer) const
{
	buffer.Resize(codecConfig.Size());

	if (codecConfig.Size() > 0) memcpy(buffer, codecConfig, codecConfig.Size());

	return codecConfig.Size() > 0;
}

Bool BoCA::CS::MuxerComponent::SetCodecConfig(const Buffer<UnsignedByte> &buffer)
{
	codecConfig.Resize(buffer.Size());

	if (buffer.Size() > 0) memcpy(codecConfig, buffer, buffer.Size());

	return True;
}

Int BoCA::CS::MuxerComponent::GetTimeScale() const
{
	return timeScale;
}

Bool BoCA::CS::MuxerComponent::SetTimeScale(Int nTimeScale)
{
	timeScale = nTimeScale;

	return True;
}

Bool BoCA::CS::MuxerComponent::GetGaplessInfo(Int &delay, Int &padding, Int64 &length) const
{
	if (!gaplessInfo) return False;

	delay	= gaplessDelay;
	padding	= gaplessPadding;
	length	= gaplessLength;

	return True;
}

Bool BoCA::CS::MuxerComponent::SetGaplessInfo(Int delay, Int padding, Int64 length)
{
	gaplessInfo    = True;

	gaplessDelay   = delay;
	gaplessPadding = padding;
	gaplessLength  = length;

	return True;
}

Int64 BoCA::CS::MuxerComponent::GetNumberOfPackets()
{
	return -1;
}

Int64 BoCA::CS::MuxerComponent::GetDuration()
{
	return -1;
}

Int64 BoCA::CS::MuxerComponent::SeekPacket(Int64 position)
{
	return -1;
}

Int BoCA::CS::MuxerComponent::ReadPacket(Buffer<UnsignedByte> &buffer, Int64 &position, Int &flags)
{
	return -1;
}

Int BoCA::CS::MuxerComponent::WritePacket(const UnsignedByte *data, Int size, Int64 position, Int flags)
{
	return -1;
}

Bool BoCA::CS::MuxerComponent::UpdateStreamInfo()
{
	return False;
}

Bool BoCA::CS::MuxerComponent::OptimizeFile(const String &streamURI)
{
	return True;
}
