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

#include <boca/common/metadata/format.h>

BoCA::Format::Format(int nil)
{
	channels	= 0;
	rate		= 0;

	bits		= 0;

	fp		= False;
	sign		= True;

	order		= BYTE_NATIVE;
}

BoCA::Format::~Format()
{
}

Bool BoCA::Format::operator ==(const Format &format) const
{
	return channels == format.channels &&
	       rate	== format.rate	   &&
	       bits	== format.bits	   &&
	       fp	== format.fp	   &&
	       sign	== format.sign;
}

Bool BoCA::Format::operator !=(const Format &format) const
{
	 return !(*this == format);
}
