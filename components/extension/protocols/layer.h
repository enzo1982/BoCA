 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_PROTOCOLS_LAYER
#define H_PROTOCOLS_LAYER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class LayerProtocols : public Layer
	{
		private:
			Text		*text_protocol;
			ComboBox	*combo_protocol;

			MultiEdit	*edit_protocol;
			EditBox		*edit_status;

			Text		*text_errors;
			EditBox		*edit_errors;
			ComboBox	*combo_errors;
			Button		*button_details;
		slots:
			Void		 OnChangeSize(const Size &);

			Void		 UpdateProtocolList();
			Void		 UpdateProtocol(const String &);

			Void		 SelectProtocol();
			Void		 ShowDetails();
		public:
					 LayerProtocols();
					~LayerProtocols();
	};
};

#endif
