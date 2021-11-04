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

BoCA_BEGIN_COMPONENT(TaggerAPEv2)

namespace BoCA
{
	class TaggerAPEv2 : public CS::TaggerComponent
	{
		private:
			static const String	 ConfigID;

			Int			 RenderAPEHeader(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderAPEFooter(Int, Int, Buffer<UnsignedByte> &);
			Int			 RenderAPEItem(const String &, const String &, Buffer<UnsignedByte> &, Bool = True);
			Int			 RenderAPEBinaryItem(const String &, const Buffer<UnsignedByte> &, Buffer<UnsignedByte> &);

			Bool			 ParseAPEHeader(const Buffer<UnsignedByte> &, Int *, Int *);
			Bool			 ParseAPEFooter(const Buffer<UnsignedByte> &, Int *, Int *);
			Bool			 ParseAPEItem(const Buffer<UnsignedByte> &, Int &, String *, String *, Bool = True);
			Bool			 ParseAPEBinaryItem(const Buffer<UnsignedByte> &, Int &, String *, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 TaggerAPEv2();
						~TaggerAPEv2();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);

			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerAPEv2)

BoCA_END_COMPONENT(TaggerAPEv2)
