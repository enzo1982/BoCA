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

BoCA_BEGIN_COMPONENT(APETag)

namespace BoCA
{
	class APETag : public CS::TaggerComponent
	{
		private:
			Int			 RenderAPEHeader(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderAPEFooter(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderAPEItem(const String &, const String &, Buffer<UnsignedByte> &);
			Int			 RenderAPEBinaryItem(const String &, const Buffer<UnsignedByte> &, Buffer<UnsignedByte> &);

			Bool			 ParseAPEHeader(const Buffer<UnsignedByte> &, Int *, Int *);
			Bool			 ParseAPEFooter(const Buffer<UnsignedByte> &, Int *, Int *);
			Bool			 ParseAPEItem(const Buffer<UnsignedByte> &, Int &, String *, String *);
			Bool			 ParseAPEBinaryItem(const Buffer<UnsignedByte> &, Int &, String *, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 APETag();
						~APETag();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);

			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(APETag)

BoCA_END_COMPONENT(APETag)
