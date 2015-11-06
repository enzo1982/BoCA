 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(OutputOSS)

namespace BoCA
{
	class OutputOSS : public CS::OutputComponent
	{
		private:
			int			 device_fd;

			Bool			 paused;
		public:
			static const String	&GetComponentSpecs();

						 OutputOSS();
						~OutputOSS();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputOSS)

BoCA_END_COMPONENT(OutputOSS)
