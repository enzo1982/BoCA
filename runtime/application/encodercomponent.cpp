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

#include <boca/application/encodercomponent.h>

BoCA::AS::EncoderComponent::EncoderComponent(ComponentSpecs *iSpecs) : StreamComponent(iSpecs)
{
}

BoCA::AS::EncoderComponent::~EncoderComponent()
{
}

Bool BoCA::AS::EncoderComponent::SetOutputFormat(Int n)
{
	return specs->func_SetOutputFormat(component, n);
}

String BoCA::AS::EncoderComponent::GetOutputFileExtension() const
{
	String	 extension = specs->func_GetOutputFileExtension(component);

	if (extension != NIL) return extension;
	else		      return specs->formats.GetFirst()->GetExtensions().GetFirst();
}

Int BoCA::AS::EncoderComponent::GetNumberOfPasses() const
{
	return specs->func_GetNumberOfPasses(component);
}

Bool BoCA::AS::EncoderComponent::IsThreadSafe() const
{
	if (!specs->threadSafe) return False;
	else			return specs->func_IsThreadSafe(component);
}

Bool BoCA::AS::EncoderComponent::IsLossless() const
{
	if (specs->func_IsLossless(component)) return True;
	else				       return specs->formats.GetFirst()->IsLossless();
}

Int BoCA::AS::EncoderComponent::WriteData(Buffer<UnsignedByte> &buffer)
{
	return specs->func_WriteData(component, &buffer);
}

Bool BoCA::AS::EncoderComponent::NextPass()
{
	return specs->func_NextPass(component);
}
