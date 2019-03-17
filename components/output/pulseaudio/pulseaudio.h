 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(OutputPulseAudio)

namespace BoCA
{
	class OutputPulseAudio : public CS::OutputComponent
	{
		private:
			static pa_threaded_mainloop	*mainloop;
			static pa_context		*context;

			pa_stream			*stream;
		public:
			static const String		&GetComponentSpecs();

			static Void			 Initialize();
			static Void			 Cleanup();

							 OutputPulseAudio();
							~OutputPulseAudio();

			Bool				 Activate();
			Bool				 Deactivate();

			Int				 WriteData(Buffer<UnsignedByte> &);
			Bool				 Finish();

			Int				 CanWrite();

			Int				 SetPause(Bool);
			Bool				 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputPulseAudio)

BoCA_END_COMPONENT(OutputPulseAudio)
