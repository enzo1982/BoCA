 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(WMATag)

namespace BoCA
{
	class WMATag : public CS::TaggerComponent
	{
		private:
			Error			 RenderWMAStringItem(const String &, const String &, Void *);
			Error			 RenderWMAIntegerItem(const String &, Int, Void *);
			Error			 RenderWMABinaryItem(const String &, const Buffer<UnsignedByte> &, Void *);
		public:
			static const String	&GetComponentSpecs();

						 WMATag();
						~WMATag();

			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderStreamInfo(const String &, const Track &);
			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(WMATag)

BoCA_END_COMPONENT(WMATag)
