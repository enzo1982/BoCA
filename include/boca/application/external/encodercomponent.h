 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_ENCODERCOMPONENTEXTERNAL
#define H_BOCA_ENCODERCOMPONENTEXTERNAL

#include "../encodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class EncoderComponentExternal : public EncoderComponent
		{
			private:
				const Config		*configuration;

				ConfigLayer		*configLayer;
			protected:
				Format			 format;

				Int			 ProcessData(Buffer<UnsignedByte> &);

				virtual Int		 RenderTags(const String &, const Track &, Buffer<UnsignedByte> &, Buffer<UnsignedByte> &);
			public:
							 EncoderComponentExternal(ComponentSpecs *);
				virtual			~EncoderComponentExternal();

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual Bool		 SetOutputFormat(Int);
				virtual String		 GetOutputFileExtension() const;

				virtual File		 GetCompanionFile(const String &) const;

				virtual Int		 GetNumberOfPasses() const	{ return 1; }

				virtual Bool		 IsThreadSafe() const		{ return True; }

				virtual Bool		 IsLossless() const;

				virtual Bool		 Activate();
				virtual Bool		 Deactivate();

				virtual ConfigLayer	*GetConfigurationLayer();
			accessors:
				Bool			 GetErrorState() const		{ return errorState; }
				const String		&GetErrorString() const		{ return errorString; }

				const Config		*GetConfiguration() const;
				Bool			 SetConfiguration(const Config *);
		};
	};
};

#endif
