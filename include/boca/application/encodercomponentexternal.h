 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "encodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class EncoderComponentExternal : public EncoderComponent
		{
			private:
				IO::OutStream		*out;

				String			 wavFileName;
				String			 encFileName;

				Int			 nOfSamples;
			protected:
				Bool			 errorState;
				String			 errorString;

				Track			 format;
			public:
							 EncoderComponentExternal(ComponentSpecs *);
				virtual			~EncoderComponentExternal();

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual String		 GetOutputFileExtension();

				virtual Bool		 Activate();
				virtual Bool		 Deactivate();

				virtual Int		 WriteData(Buffer<UnsignedByte> &, Int);

				virtual ConfigLayer	*GetConfigurationLayer();
				virtual Void		 FreeConfigurationLayer();

				Bool			 GetErrorState()	{ return errorState; }
				const String		&GetErrorString()	{ return errorString; }
		};
	};
};
