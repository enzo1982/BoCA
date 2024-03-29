 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(EncoderSndFile)

namespace BoCA
{
	class EncoderSndFile : public CS::EncoderComponent
	{
		friend sf_count_t	 sf_vio_get_filelen_callback(void *);
		friend sf_count_t	 sf_vio_seek_callback(sf_count_t, int, void *);
		friend sf_count_t	 sf_vio_read_callback(void *, sf_count_t, void *);
		friend sf_count_t	 sf_vio_write_callback(const void *, sf_count_t, void *);
		friend sf_count_t	 sf_vio_tell_callback(void *);

		private:
			ConfigLayer		*configLayer;
			Config			*config;

			AS::EncoderComponent	*encoder;

			Int			 fileFormat;

			SNDFILE			*sndf;

			static Int		 SelectBestSubFormat(const Format &, Int);

			static Bool		 ConvertArguments(Config *);
		public:
			static const String	&GetComponentSpecs();

						 EncoderSndFile();
						~EncoderSndFile();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Bool			 SetOutputFormat(Int);
			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderSndFile)

BoCA_END_COMPONENT(EncoderSndFile)
