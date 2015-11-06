 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_MULTIENCODERHUBCONFIG
#define H_MULTIENCODERHUBCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureMultiEncoderHub : public ConfigLayer
	{
		private:
			GroupBox	*group_encoders;

			Text		*text_available;
			ListBox		*list_available;
			Text		*text_selected;
			ListBox		*list_selected;

			Button		*btn_add;
			Button		*btn_remove;
			Button		*btn_configure;

			GroupBox	*group_options;

			CheckBox	*check_folders;

			Bool		 separateFolders;

			Void		 AddEncoders();
		public:
					 ConfigureMultiEncoderHub();
					~ConfigureMultiEncoderHub();

			Int		 SaveSettings();
		slots:
			Void		 OnSelectAvailable();
			Void		 OnSelectEncoder();

			Void		 OnAddEncoder();
			Void		 OnRemoveEncoder();

			Void		 OnConfigureEncoder();
	};
};

#endif
