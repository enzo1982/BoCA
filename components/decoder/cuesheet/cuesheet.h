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

BoCA_BEGIN_COMPONENT(DecoderCueSheet)

namespace BoCA
{
	class DecoderCueSheet : public CS::DecoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			Void			 UpdateInfoWithAlbumInfo(Info &, const Info &) const;

			Bool			 AddTrack(const Track &, Array<Track> &) const;

			static String		 UnescapeString(const String &);
		public:
			static const String	&GetComponentSpecs();

						 DecoderCueSheet();
						~DecoderCueSheet();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCueSheet)

BoCA_END_COMPONENT(DecoderCueSheet)
