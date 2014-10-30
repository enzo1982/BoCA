 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
				ConfigLayer		*configLayer;
			protected:
				Bool			 errorState;
				String			 errorString;

				Track			 track;
				Format			 format;

				virtual Int		 RenderTags(const String &, const Track &, Buffer<UnsignedByte> &, Buffer<UnsignedByte> &);
			public:
							 EncoderComponentExternal(ComponentSpecs *);
				virtual			~EncoderComponentExternal();

				virtual Bool		 SetAudioTrackInfo(const Track &nTrack)	{ track = nTrack; format = nTrack.GetFormat(); return True; }

				virtual String		 GetOutputFileExtension() const;

				virtual ConfigLayer	*GetConfigurationLayer();
			accessors:
				Bool			 GetErrorState() const	{ return errorState; }
				const String		&GetErrorString() const	{ return errorString; }
		};
	};
};

#endif
