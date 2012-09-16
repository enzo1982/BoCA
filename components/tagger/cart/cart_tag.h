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

BoCA_BEGIN_COMPONENT(CartTag)

namespace BoCA
{
	class CartTag : public CS::TaggerComponent
	{
		private:
			Int			 RenderTagHeader(Buffer<UnsignedByte> &);

			Int			 RenderStringItem(const String &, Int, Buffer<UnsignedByte> &);
			Int			 RenderIntegerItem(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderTimerItem(const String &, Int, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 CartTag();
						~CartTag();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(CartTag)

BoCA_END_COMPONENT(CartTag)
