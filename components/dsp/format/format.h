 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DSPFormat)

namespace BoCA
{
	class DSPFormat : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			Buffer<Int64>		 samplesBuffer;

			Void			 TransformSamples(const UnsignedByte *, const Format &, UnsignedByte *, const Format &, Int);
		public:
			static const String	&GetComponentSpecs();

						 DSPFormat();
						~DSPFormat();

			Bool			 Activate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPFormat)

BoCA_END_COMPONENT(DSPFormat)
