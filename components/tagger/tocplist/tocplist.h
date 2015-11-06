 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(TaggerTOCPList)

namespace BoCA
{
	class TaggerTOCPList : public CS::TaggerComponent
	{
		private:
			Bool			 ReadSessions(XML::Node *, Buffer<UnsignedByte> &, Int32 &);
			Bool			 ReadSession(XML::Node *, Buffer<UnsignedByte> &, Int32 &);
			Bool			 ReadTracks(XML::Node *, Buffer<UnsignedByte> &);
			Bool			 ReadTrack(XML::Node *, Buffer<UnsignedByte> &);
		public:
			static const String	&GetComponentSpecs();

						 TaggerTOCPList();
						~TaggerTOCPList();

			Error			 ParseStreamInfo(const String &, Track &);
	};
};

BoCA_DEFINE_TAGGER_COMPONENT(TaggerTOCPList)

BoCA_END_COMPONENT(TaggerTOCPList)
