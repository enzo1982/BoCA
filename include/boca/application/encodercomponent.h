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

#ifndef H_BOCA_AS_ENCODERCOMPONENT
#define H_BOCA_AS_ENCODERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT EncoderComponent : public Component, public IO::Filter
		{
			public:
						 EncoderComponent(ComponentSpecs *);
				virtual 	~EncoderComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);

				virtual String	 GetOutputFileExtension() const;
				virtual Int	 GetNumberOfPasses() const;

				virtual Bool	 IsThreadSafe() const;

				virtual Bool	 IsLossless() const;

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 WriteData(Buffer<UnsignedByte> &);
				virtual Bool	 NextPass();

				virtual Int	 GetPackageSize() const;
				virtual Int	 SetDriver(IO::Driver *);

		};
	};
};

#endif
