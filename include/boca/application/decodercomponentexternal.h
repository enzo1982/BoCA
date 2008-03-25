 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "decodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class DecoderComponentExternal : public DecoderComponent
		{
			private:
				IO::InStream		*in;

				String			 wavFileName;
			protected:
				Bool			 errorState;
				String			 errorString;

				Track			 format;
			public:
							 DecoderComponentExternal(ComponentSpecs *);
				virtual			~DecoderComponentExternal();

				virtual Bool		 CanOpenStream(const String &);
				virtual Error		 GetStreamInfo(const String &, Track &);

				virtual Void		 SetInputFormat(const Track &);

				virtual Int64		 GetInBytes();

				virtual Int		 GetPackageSize();
				virtual Int		 SetDriver(IO::Driver *);

				virtual Bool		 Activate();
				virtual Bool		 Deactivate();

				virtual Int		 ReadData(Buffer<UnsignedByte> &, Int);

				virtual ConfigLayer	*GetConfigurationLayer();
				virtual Void		 FreeConfigurationLayer();

				Bool			 GetErrorState()	{ return errorState; }
				const String		&GetErrorString()	{ return errorString; }
		};
	};
};
