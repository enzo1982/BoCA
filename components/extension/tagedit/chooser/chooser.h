 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_TAGEDIT_CHOOSER
#define H_TAGEDIT_CHOOSER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class Chooser : public Layer
	{
		public:
							 Chooser(const String &);
			virtual				~Chooser();

			Bool				 IsActiveChooser();

			virtual Void			 ReselectEntry();
		signals:
			Signal1<Void, const Track &>	 onSelectTrack;
			Signal1<Void, const Track &>	 onSelectAlbum;
			Signal0<Void>			 onSelectNone;
		callbacks:
			Callback0<Bool>			 doReselectEntry;
		slots:
			virtual Void			 OnModifyTrack(const Track &);
	};
};

#endif
