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

#ifndef H_BOCA_DECODERCOMPONENTEXTERNAL
#define H_BOCA_DECODERCOMPONENTEXTERNAL

#include "../decodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class DecoderComponentExternal : public DecoderComponent
		{
			private:
				const Config		*configuration;

				ConfigLayer		*configLayer;
			protected:
				Int64			 inBytes;

				Int			 ProcessData(Buffer<UnsignedByte> &);

				virtual Int		 QueryTags(const String &, Track &) const;
			public:
							 DecoderComponentExternal(ComponentSpecs *);
				virtual			~DecoderComponentExternal();

				virtual Bool		 CanOpenStream(const String &);

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual File		 GetCompanionFile(const String &) const;

				virtual Bool		 IsThreadSafe() const	{ return True; }

				Int64			 GetInBytes() const	{ return inBytes; }

				virtual Int		 SetDriver(IO::Driver *);

				virtual ConfigLayer	*GetConfigurationLayer();
			accessors:
				Bool			 GetErrorState() const	{ return errorState; }
				const String		&GetErrorString() const	{ return errorString; }

				const Config		*GetConfiguration() const;
				Bool			 SetConfiguration(const Config *);
		};
	};
};

#endif
