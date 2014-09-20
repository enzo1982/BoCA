 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_COMPONENT
#define H_BOCA_AS_COMPONENT

#include "componentspecs.h"
#include "../common/configlayer.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		abstract class BOCA_DLL_EXPORT Component
		{
			protected:
				ComponentSpecs			*specs;

				void				*component;
			public:
								 Component(ComponentSpecs *);
				virtual				~Component();

				virtual String			 GetComponentSpecs();

				const String			&GetName() const;
				const String			&GetVersion() const;

				const String			&GetID() const;
				ComponentType			 GetType() const;

				Bool				 IsThreadSafe() const;	

				const Array<FileFormat *>	&GetFormats() const;
				const Array<TagSpec *>		&GetTagSpecs() const;

				virtual ConfigLayer		*GetConfigurationLayer();

				virtual Bool			 GetErrorState() const;
				virtual const String		&GetErrorString() const;
		};
	};
};

#endif
