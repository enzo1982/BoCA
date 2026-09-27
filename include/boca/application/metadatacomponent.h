 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2026 Shen-Ta Hsieh
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_METADATACOMPONENT
#define H_BOCA_AS_METADATACOMPONENT

#include "component.h"
#include "../common/metadata/discmetadata.h"
#include "../common/metadata/picture.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT MetadataComponent : public Component
		{
			public:
						 MetadataComponent(ComponentSpecs *);
				virtual		~MetadataComponent();

				virtual Int	 GetQueryKeys() const;

				virtual Int	 Query(const DiscMetadata &, Array<MetadataMatch> &);
				virtual Bool	 Read(const String &, DiscMetadata &);

				virtual String	 NormalizeRecordID(const String &);

				virtual Int	 GetSubmitData() const;
				virtual Int	 GetSubmitMethod() const;
				virtual Bool	 Submit(const DiscMetadata &);

				virtual Bool	 CanReadPictures() const;
				virtual Bool	 ReadPictures(const String &, Array<Picture> &);

				virtual Void	 Cancel();
		};
	};
};

#endif
