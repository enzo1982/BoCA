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

#ifndef H_BOCA_FORMATCONVERTER
#define H_BOCA_FORMATCONVERTER

namespace BoCA
{
	class FormatConverter;

	namespace AS
	{
		class DSPComponent;
	};
};

#include "../application/component.h"
#include "metadata/format.h"

namespace BoCA
{
	class FormatConverterData;

	class BOCA_DLL_EXPORT FormatConverter
	{
		private:
			Array<AS::DSPComponent *, Void *>	 converters;
			FormatConverterData			*converterData;

			Bool					 errorState;
			String					 errorString;

			Threads::Thread				 converterThread;
			Threads::Mutex				 converterMutex;

			Buffer<UnsignedByte>			 samplesBuffer;
			Buffer<UnsignedByte>			 backBuffer;

			Bool					 process;
			Bool					 finish;

			Int					 ConverterThread();

			static Int				 FindBestValue(Int, const String &);
		public:
			static Format				 GetBestTargetFormat(const Format &, const AS::Component *);

								 FormatConverter(const Format &, const Format &);
								~FormatConverter();

			Int					 Transform(Buffer<UnsignedByte> &);
			Int					 Finish(Buffer<UnsignedByte> &);

			Bool					 GetErrorState() const;
			const String				&GetErrorString() const;
	};
};

#endif
