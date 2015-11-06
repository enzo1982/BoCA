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

#ifndef H_BOCA_AS_TAGGERCOMPONENT
#define H_BOCA_AS_TAGGERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT TaggerComponent : public Component
		{
			public:
						 TaggerComponent(ComponentSpecs *);
				virtual		~TaggerComponent();

				virtual Void	 SetVendorString(const String &);

				virtual Error	 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
				virtual Error	 ParseStreamInfo(const String &, Track &);

				virtual Error	 RenderBuffer(Buffer<UnsignedByte> &, const Track &);
				virtual Error	 RenderStreamInfo(const String &, const Track &);

				virtual Error	 UpdateStreamInfo(const String &, const Track &);
		};
	};
};

#endif
