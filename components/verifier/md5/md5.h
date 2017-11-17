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

#include <boca.h>

BoCA_BEGIN_COMPONENT(VerifierMD5)

namespace BoCA
{
	class VerifierMD5 : public CS::VerifierComponent
	{
		private:
			Hash::MD5		 md5;
		public:
			static const String	&GetComponentSpecs();

						 VerifierMD5();
						~VerifierMD5();

			Bool			 CanVerifyTrack(const Track &);

			Int			 ProcessData(Buffer<UnsignedByte> &);

			Bool			 Verify();
	};
};

BoCA_DEFINE_VERIFIER_COMPONENT(VerifierMD5)

BoCA_END_COMPONENT(VerifierMD5)
