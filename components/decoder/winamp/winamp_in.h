 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(WinampIn)

namespace BoCA
{
	class WinampIn : public CS::DecoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			In_Module		*plugin;

			In_Module		*GetPluginForFile(const String &);
		public:
			static const String	&GetComponentSpecs();

						 WinampIn();
						~WinampIn();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(WinampIn)

BoCA_END_COMPONENT(WinampIn)
