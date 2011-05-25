 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_CONVERTER
#define H_YOUTUBE_CONVERTER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class Converter
	{
		private:
			static Array<Converter *>	 converters;

			String				 name;
			String				 version;

			String				 decoder;

			AS::FileFormat			*format;

			String				 external_command;
			Bool				 external_ignoreExitCode;

			String				 external_arguments;

							 Converter(const String &);
							~Converter();

			Bool				 IsSane();

			Int				 ParseXML(const String &);
		public:
			static Array<Converter *>	&Get();
			static Bool			 Free();

			Int				 Run(const String &, const String &);
		accessors:
			const String			&GetName()	{ return name; }
			const String			&GetVersion()	{ return version; }

			const String			&GetDecoderID()	{ return decoder; }

			AS::FileFormat			*GetFormat()	{ return format; }
	};
};

#endif
