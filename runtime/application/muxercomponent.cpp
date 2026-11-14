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

#include <boca/application/muxercomponent.h>

BoCA::AS::MuxerComponent::MuxerComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
}

BoCA::AS::MuxerComponent::~MuxerComponent()
{
}

Bool BoCA::AS::MuxerComponent::SetAudioTrackInfo(const Track &track)
{
	return specs->func_SetAudioTrackInfo(component, &track);
}

Void BoCA::AS::MuxerComponent::SetDriver(IO::Driver *driver)
{
	specs->func_SetDriver(component, driver);
}

Bool BoCA::AS::MuxerComponent::Open(MuxerMode mode)
{
	return specs->func_Open(component, mode);
}

Bool BoCA::AS::MuxerComponent::Close()
{
	return specs->func_Close(component);
}

const BoCA::Format &BoCA::AS::MuxerComponent::GetFormatInfo() const
{
	return *((const Format *) specs->func_GetFormatInfo(component));
}

BoCA::MuxerCodec BoCA::AS::MuxerComponent::GetCodec() const
{
	return (MuxerCodec) specs->func_GetCodec(component);
}

Bool BoCA::AS::MuxerComponent::SetCodec(MuxerCodec codec)
{
	return specs->func_SetCodec(component, codec);
}

Bool BoCA::AS::MuxerComponent::GetCodecConfig(Buffer<UnsignedByte> &buffer) const
{
	return specs->func_GetCodecConfig(component, &buffer);
}

Bool BoCA::AS::MuxerComponent::SetCodecConfig(const Buffer<UnsignedByte> &buffer)
{
	return specs->func_SetCodecConfig(component, &buffer);
}

Int BoCA::AS::MuxerComponent::GetTimeScale() const
{
	return specs->func_GetTimeScale(component);
}

Bool BoCA::AS::MuxerComponent::SetTimeScale(Int timeScale)
{
	return specs->func_SetTimeScale(component, timeScale);
}

Bool BoCA::AS::MuxerComponent::GetGaplessInfo(Int &delay, Int &padding, Int64 &length) const
{
	return specs->func_GetGaplessInfo(component, &delay, &padding, &length);
}

Bool BoCA::AS::MuxerComponent::SetGaplessInfo(Int delay, Int padding, Int64 length)
{
	return specs->func_SetGaplessInfo(component, delay, padding, length);
}

Int64 BoCA::AS::MuxerComponent::GetNumberOfPackets()
{
	return specs->func_GetNumberOfPackets(component);
}

Int64 BoCA::AS::MuxerComponent::GetDuration()
{
	return specs->func_GetDuration(component);
}

Int BoCA::AS::MuxerComponent::ReadPacket(Buffer<UnsignedByte> &buffer, Int64 &position, Int &flags)
{
	return specs->func_ReadPacket(component, &buffer, &position, &flags);
}

Int BoCA::AS::MuxerComponent::WritePacket(const UnsignedByte *data, Int size, Int64 position, Int flags)
{
	return specs->func_WritePacket(component, data, size, position, flags);
}

Int64 BoCA::AS::MuxerComponent::SeekPacket(Int64 position)
{
	return specs->func_SeekPacket(component, position);
}

Bool BoCA::AS::MuxerComponent::OptimizeFile(const String &streamURI)
{
	return specs->func_OptimizeFile(component, streamURI);
}
