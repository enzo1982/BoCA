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

#ifndef H_BOCA_CONFIGLAYEREXTERNAL
#define H_BOCA_CONFIGLAYEREXTERNAL

#include "../../common/configlayer.h"
#include "../componentspecs.h"

namespace BoCA
{
	namespace AS
	{
		class ConfigLayerExternal : public ConfigLayer
		{
			protected:
				ComponentSpecs			*specs;

				Text				*text_parameters;
				ListBox				*list_parameters;

				Text				*text_commandline;
				EditBox				*edit_commandline;

				Array<GroupBox *, Void *>	 groups_parameters;
				Array<Widget *, Void *>		 widgets_parameters;

				GroupBox			*GetParameterGroupBox(const String &);
				String				 GetArgumentsString();
			public:
								 ConfigLayerExternal(ComponentSpecs *);
				virtual				~ConfigLayerExternal();

				virtual Int			 SaveSettings();
			slots:
				Void				 OnSelectParameter(ListEntry *);
				Void				 OnUpdateParameterValue();
				Void				 OnSliderValueChange();
		};
	};
};

#endif
