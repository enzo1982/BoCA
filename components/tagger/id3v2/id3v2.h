 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include <id3/tag.h>

BoCA_BEGIN_COMPONENT(TaggerID3v2)

namespace BoCA
{
	class TaggerID3v2 : public CS::TaggerComponent
	{
		private:
			static const String	 genres[192];

			Int			 ParseContainer(const ID3_Container &, Track &);
			Int			 RenderContainer(ID3_Container &, const Track &, Bool = False);

			String			 GetStringField(const ID3_Frame &, ID3_FieldID);
			Int			 SetStringField(ID3_Frame &, ID3_FieldID, const String &);

			String			 GetASCIIField(const ID3_Frame &, ID3_FieldID);
			Int			 SetASCIIField(ID3_Frame &, ID3_FieldID, const String &);

			Int			 GetIntegerField(const ID3_Frame &, ID3_FieldID);
			Int			 SetIntegerField(ID3_Frame &, ID3_FieldID, Int);

			Int			 GetBinaryField(const ID3_Frame &, ID3_FieldID, Buffer<UnsignedByte> &);
			Int			 SetBinaryField(ID3_Frame &, ID3_FieldID, const Buffer<UnsignedByte> &);

			const String		&GetID3CategoryName(UnsignedInt);
		public:
			static const String	&GetComponentSpecs();

						 TaggerID3v2();
						~TaggerID3v2();

			Error			 ParseBuffer(const Buffer<UnsignedByte> &, Track &);
			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderBuffer(Buffer<UnsignedByte> &, const Track &);

			Error			 UpdateStreamInfo(const String &, const Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerID3v2)

BoCA_END_COMPONENT(TaggerID3v2)
