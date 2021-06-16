 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(TaggerCart)

namespace BoCA
{
	class TaggerCart : public CS::TaggerComponent
	{
		private:
			static const String	 ConfigID;

			Int			 RenderTagHeader(Buffer<UnsignedByte> &);

			Int			 RenderStringItem(const String &, Int, Buffer<UnsignedByte> &);
			Int			 RenderIntegerItem(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderTimerItem(const String &, Int, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 TaggerCart();
						~TaggerCart();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);

			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerCart)

BoCA_END_COMPONENT(TaggerCart)
