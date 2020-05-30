 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

#include "md5.h"

const String &BoCA::VerifierMD5::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>MD5 Checksum Verifier</name>		\
	    <version>1.0</version>			\
	    <id>md5-verify</id>				\
	    <type>verifier</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::VerifierMD5::VerifierMD5()
{
}

BoCA::VerifierMD5::~VerifierMD5()
{
}

Bool BoCA::VerifierMD5::CanVerifyTrack(const Track &track)
{
	if (track.md5 != NIL && track.md5 != String().FillN('0', 32)) return True;
	else							      return False;
}

Int BoCA::VerifierMD5::ProcessData(Buffer<UnsignedByte> &data)
{
	/* Find system byte order.
	 */
	static Int	 systemByteOrder = CPU().GetEndianness() == EndianLittle ? BYTE_INTEL : BYTE_RAW;

	const Format	&format = track.GetFormat();

	/* Switch byte order to Intel.
	 */
	if (systemByteOrder != BYTE_INTEL) BoCA::Utilities::SwitchBufferByteOrder(data, format.bits / 8);

	/* Calculate MD5.
	 */
	md5.Feed(data);

	/* Switch back to native byte order.
	 */
	if (systemByteOrder != BYTE_INTEL) BoCA::Utilities::SwitchBufferByteOrder(data, format.bits / 8);

	return data.Size();
}

Bool BoCA::VerifierMD5::Verify()
{
	String	 checksum = md5.Finish();

	errorString = String("Checksum: ").Append(checksum);

	if (checksum != track.md5) errorString = String("Expected: ").Append(track.md5);

	return checksum == track.md5;
}
