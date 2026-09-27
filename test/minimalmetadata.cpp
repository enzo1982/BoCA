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

#include <boca.h>

BoCA_BEGIN_COMPONENT(MetadataMinimal)

namespace BoCA
{
	/* A metadata component for tests that overrides
	 * only the functions that it must override.
	 */
	class MetadataMinimal : public CS::MetadataComponent
	{
		public:
			static const String	&GetComponentSpecs();

			Int			 GetQueryKeys() const					{ return METADATA_QUERY_KEY_TEXT; }

			Int			 Query(const DiscMetadata &, Array<MetadataMatch> &)	{ return METADATA_QUERY_RESULT_NONE; }
			Bool			 Read(const String &, DiscMetadata &)			{ return False; }
	};
};

const String &BoCA::MetadataMinimal::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>Minimal Metadata Provider</name>		\
	    <version>1.0</version>				\
	    <id>minimal-metadata</id>				\
	    <type>metadata</type>				\
	  </component>						\
								\
	";

	return componentSpecs;
}

BoCA_DEFINE_METADATA_COMPONENT(MetadataMinimal)

BoCA_END_COMPONENT(MetadataMinimal)
