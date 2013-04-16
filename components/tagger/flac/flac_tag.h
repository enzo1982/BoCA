 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(FLACTag)

namespace BoCA
{
	class FLACTag : public CS::TaggerComponent
	{
		public:
			static const String	&GetComponentSpecs();

						 FLACTag();
						~FLACTag();

			Error			 RenderStreamInfo(const String &, const Track &);
			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(FLACTag)

BoCA_END_COMPONENT(FLACTag)
