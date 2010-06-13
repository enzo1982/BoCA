 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_TAGEDIT_EDITOR
#define H_TAGEDIT_EDITOR

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class Editor : public Layer
	{
		public:
							 Editor(const String &);
			virtual				~Editor();
		signals:
			Signal1<Void, const Track &>	 onModifyTrack;
		slots:
			virtual Void			 OnSelectTrack(const Track &);
			virtual Void			 OnSelectAlbum(const Track &);
			virtual Void			 OnSelectNone();
	};
};

#endif
