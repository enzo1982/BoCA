 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(TaggerWMA)

namespace BoCA
{
	class TaggerWMA : public CS::TaggerComponent
	{
		private:
			Error			 RenderWMAStringItem(const String &, const String &, Void *);
			Error			 RenderWMAIntegerItem(const String &, Int, Void *);
			Error			 RenderWMABinaryItem(const String &, const Buffer<UnsignedByte> &, Void *);
		public:
			static const String	&GetComponentSpecs();

			static Void		 Initialize();
			static Void		 Cleanup();

						 TaggerWMA();
						~TaggerWMA();

			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderStreamInfo(const String &, const Track &);
			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerWMA)

BoCA_END_COMPONENT(TaggerWMA)
