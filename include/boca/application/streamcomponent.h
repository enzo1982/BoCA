 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_STREAMCOMPONENT
#define H_BOCA_AS_STREAMCOMPONENT

#include "component.h"
#include "../common/formatconverter.h"
#include "../common/metadata/track.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT StreamComponent : public Component, public IO::Filter
		{
			protected:
				FormatConverter		*converter;

				Bool			 errorState;
				String			 errorString;

				Bool			 calculateMD5;
				Hash::MD5		 md5;

				Track			 track;
			public:
							 StreamComponent(ComponentSpecs *);
				virtual 		~StreamComponent();

				virtual Bool		 SetAudioTrackInfo(const Track &);

				virtual Bool		 IsThreadSafe() const;

				virtual Bool		 Activate();
				virtual Bool		 Deactivate();

				virtual Int		 SetDriver(IO::Driver *);

				Void			 SetCalculateMD5(Bool);
				String			 GetMD5Checksum();

				virtual Bool		 GetErrorState() const;
				virtual const String	&GetErrorString() const;
		};
	};
};

#endif
