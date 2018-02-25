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

#include <boca/application/dspcomponent.h>

BoCA::AS::DSPComponent::DSPComponent(ComponentSpecs *iSpecs) : StreamComponent(iSpecs)
{
}

BoCA::AS::DSPComponent::~DSPComponent()
{
}

Int BoCA::AS::DSPComponent::GetPackageSize() const
{
	return specs->func_GetPackageSize(component);
}

const BoCA::Format &BoCA::AS::DSPComponent::GetFormatInfo() const
{
	return *((const Format *) specs->func_GetFormatInfo(component));
}

Int BoCA::AS::DSPComponent::TransformData(Buffer<UnsignedByte> &buffer)
{
	converter->Transform(buffer);

	if (buffer.Size() == 0) return 0;

	return specs->func_TransformData(component, &buffer);
}

Int BoCA::AS::DSPComponent::Flush(Buffer<UnsignedByte> &buffer)
{
	/* Finish conversion.
	 */
	converter->Finish(buffer);

	if (buffer.Size() != 0) specs->func_TransformData(component, &buffer);

	/* Flush component.
	 */
	Buffer<UnsignedByte>	 flush;

	specs->func_Flush(component, &flush);

	/* Append flush buffer contents to output.
	 */
	buffer.Resize(buffer.Size() + flush.Size());

	memcpy(buffer + buffer.Size() - flush.Size(), flush, flush.Size());

	return buffer.Size();
}
