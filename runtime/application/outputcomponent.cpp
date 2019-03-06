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

#include <boca/application/outputcomponent.h>

BoCA::AS::OutputComponent::OutputComponent(ComponentSpecs *iSpecs) : StreamComponent(iSpecs)
{
}

BoCA::AS::OutputComponent::~OutputComponent()
{
}

Int BoCA::AS::OutputComponent::WriteData(const Buffer<UnsignedByte> &data)
{
	Buffer<UnsignedByte>	&buffer = const_cast<Buffer<UnsignedByte> &>(data);

	converter->Transform(buffer);

	if (buffer.Size() == 0) return 0;

	return specs->func_WriteData(component, &buffer);
}

Bool BoCA::AS::OutputComponent::Finish()
{
	/* Finish conversion.
	 */
	Buffer<UnsignedByte>	 buffer;

	converter->Finish(buffer);

	if (buffer.Size() != 0) specs->func_WriteData(component, &buffer);

	/* Signal component to finish.
	 */
	return specs->func_Finish(component);
}

Int BoCA::AS::OutputComponent::CanWrite()
{
	return specs->func_CanWrite(component);
}

Int BoCA::AS::OutputComponent::SetPause(Bool pause)
{
	return specs->func_SetPause(component, pause);
}

Bool BoCA::AS::OutputComponent::IsPlaying()
{
	return specs->func_IsPlaying(component);
}
