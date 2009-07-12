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

BoCA_BEGIN_COMPONENT(ID3v2Tag)

namespace BoCA
{
	class ID3v2Tag : public CS::TaggerComponent
	{
		private:
			static const String	 genres[148];
			static String		 dummyString;

			String			 GetFrameString(Void *, String & = dummyString);
			Int			 SetFrameString(Void *, const String &, const String & = NIL);

			String			 GetFrameURL(Void *);
			Int			 SetFrameURL(Void *, const String &);

			const String		&GetID3CategoryName(Int);
		public:
			static const String	&GetComponentSpecs();

						 ID3v2Tag();
						~ID3v2Tag();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);

			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(ID3v2Tag)

BoCA_END_COMPONENT(ID3v2Tag)
