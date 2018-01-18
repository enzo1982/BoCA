 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/decodercomponent.h>
#include <boca/common/utilities.h>

BoCA::AS::DecoderComponent::DecoderComponent(ComponentSpecs *iSpecs) : StreamComponent(iSpecs)
{
}

BoCA::AS::DecoderComponent::~DecoderComponent()
{
}

Bool BoCA::AS::DecoderComponent::CanOpenStream(const String &streamURI)
{
	return specs->func_CanOpenStream(component, streamURI);
}

Error BoCA::AS::DecoderComponent::GetStreamInfo(const String &streamURI, Track &track)
{
	track.origFilename = streamURI;

	track.lossless	   = specs->formats.GetFirst()->IsLossless();

	if (specs->func_GetStreamInfo(component, streamURI, &track) != Success()) return Error();

	/* Set lossless flag for chapters.
	 */
	if (track.lossless) foreach (Track &chapter, track.tracks) chapter.lossless = True;

	return Success();
}

Bool BoCA::AS::DecoderComponent::Activate()
{
	SetDriver(driver);

	if (!specs->func_Activate(component)) return False;

	packageSize = GetPackageSize();

	return True;
}

Bool BoCA::AS::DecoderComponent::Deactivate()
{
	return specs->func_Deactivate(component);
}

Bool BoCA::AS::DecoderComponent::Seek(Int64 samplePosition)
{
	return specs->func_Seek(component, samplePosition);
}

Int BoCA::AS::DecoderComponent::ReadData(Buffer<UnsignedByte> &buffer)
{
	/* Find system byte order.
	 */
	static Int	 systemByteOrder = CPU().GetEndianness() == EndianLittle ? BYTE_INTEL : BYTE_RAW;

	/* Get data from decoder component.
	 */
	Int	 bytes = specs->func_ReadData(component, &buffer);

	if (bytes >= 0)
	{
		buffer.Resize(bytes);

		/* Switch byte order to native.
		 */
		const Format	&format = track.GetFormat();

		if (format.order != BYTE_NATIVE && format.order != systemByteOrder) Utilities::SwitchBufferByteOrder(buffer, format.bits / 8);

		/* Calculate MD5 if requested.
		 */
		if (calculateMD5) md5.Feed(buffer);
	}

	return bytes;
}

Int64 BoCA::AS::DecoderComponent::GetInBytes() const
{
	return specs->func_GetInBytes(component);
}
