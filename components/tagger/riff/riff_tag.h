 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(RIFFTag)

namespace BoCA
{
	class RIFFTag : public CS::TaggerComponent
	{
		private:
			Int			 RenderTagHeader(Buffer<UnsignedByte> &);
			Int			 RenderTagItem(const String &, const String &, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 RIFFTag();
						~RIFFTag();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);
//			Error			 RenderStreamInfo(const String &, const Track &);

//			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(RIFFTag)

BoCA_END_COMPONENT(RIFFTag)
