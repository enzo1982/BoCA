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
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(TaggerMP4)

namespace BoCA
{
	class TaggerMP4 : public CS::TaggerComponent
	{
		private:
			static const String	 ConfigID;

			static const String	 ItmfMeaningiTunes;
			static const String	 ItmfMeaningReplayGain;

			static const String	 genres[192];

			ConfigLayer		*configLayer;

			static Bool		 AddItmfItem(MP4FileHandle, const String &, const String &, const String &, const String &, MP4ItmfBasicType = MP4_ITMF_BT_UTF8);
			static Bool		 RemoveItmfItem(MP4FileHandle, const String &, const String & = NIL, const String & = NIL);

			static Bool		 ParseItmfItems(MP4FileHandle, Info &);
			static String		 GetItmfItemValue(MP4ItmfItem &);

			static const String	&GetID3CategoryName(UnsignedInt);
		public:
			static const String	&GetComponentSpecs();

						 TaggerMP4();
						~TaggerMP4();

			Error			 ParseStreamInfo(const String &, Track &);

			Error			 RenderStreamInfo(const String &, const Track &);
			Error			 UpdateStreamInfo(const String &, const Track &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerMP4)

BoCA_END_COMPONENT(TaggerMP4)
