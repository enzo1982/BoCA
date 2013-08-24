 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
				ConfigLayer		*configLayer;
			protected:
				Bool			 errorState;
				String			 errorString;

				Track			 track;

				Int64			 inBytes;

				virtual Int		 QueryTags(const String &, Track &) const;
			public:
							 DecoderComponentExternal(ComponentSpecs *);
				virtual			~DecoderComponentExternal();

				virtual Bool		 CanOpenStream(const String &);

				Bool			 SetAudioTrackInfo(const Track &nTrack)	{ track = nTrack;  return True; }

				Int64			 GetInBytes() const			{ return inBytes; }

				virtual Int		 GetPackageSize();
				virtual Int		 SetDriver(IO::Driver *);

				virtual ConfigLayer	*GetConfigurationLayer();
			accessors:
				Bool			 GetErrorState() const			{ return errorState; }
				const String		&GetErrorString() const			{ return errorString; }
		};
	};
};

#endif
