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

#ifndef H_BOCA_DISCMETADATA
#define H_BOCA_DISCMETADATA

#include <smooth.h>
#include "../../core/definitions.h"
#include "info.h"

using namespace smooth;

namespace BoCA
{
	/* Types of data that a metadata provider can use to find a disc.
	 */
	const Int	 METADATA_QUERY_KEY_TOC		= 1 << 0;	// Needs disc.mcdi or disc.offsets.
	const Int	 METADATA_QUERY_KEY_TEXT	= 1 << 1;	// Needs disc.artist and disc.album.
	const Int	 METADATA_QUERY_KEY_ID		= 1 << 2;	// Needs a provider ID in INFO_METADATA_RECORDID.

	/* Results of a metadata query.
	 */
	const Int	 METADATA_QUERY_RESULT_ERROR	= -1;
	const Int	 METADATA_QUERY_RESULT_NONE	=  0;
	const Int	 METADATA_QUERY_RESULT_SINGLE	=  1;
	const Int	 METADATA_QUERY_RESULT_MULTIPLE	=  2;
	const Int	 METADATA_QUERY_RESULT_FUZZY	=  3;

	/* Data that a metadata provider takes in a submit.
	 */
	const Int	 METADATA_SUBMIT_DATA_NONE	= 0;		// The provider takes no submits.
	const Int	 METADATA_SUBMIT_DATA_RECORD	= 1;		// The provider takes the edited record.
	const Int	 METADATA_SUBMIT_DATA_TOC	= 2;		// The provider takes only the TOC.

	/* Who finishes a submit.
	 */
	const Int	 METADATA_SUBMIT_BY_PROVIDER	= 0;		// Submit() sends the data or puts it into a queue.
	const Int	 METADATA_SUBMIT_BY_USER	= 1;		// Submit() opens the browser, and the user finishes there.

	/* One complete set of disc and track metadata from one provider.
	 */
	class BOCA_DLL_EXPORT DiscMetadata
	{
		public:
			String			 provider;	// Component ID of the provider, e.g. "cddb-metadata".
			String			 recordID;	// Key of the record at the provider, e.g. "rock/a50c3e0c".
			Int			 revision;	// Record revision for submit, -1 if the provider has none.

			Info			 disc;		// Disc fields and the TOC (disc.mcdi or disc.offsets).
			Array<Info>		 tracks;	// One entry for each TOC entry, in TOC order, data tracks included.

			Array<String>		 extensions;	// "key=value" provider data that is not a tag.

						 DiscMetadata();
						 DiscMetadata(const DiscMetadata &);
						~DiscMetadata();

			DiscMetadata &operator	 =(const DiscMetadata &);
	};

	/* One candidate record that a query returns.
	 */
	class BOCA_DLL_EXPORT MetadataMatch
	{
		public:
			String			 recordID;
			String			 title;		// Text for the match list.
			Bool			 fuzzy;		// True if the match is not exact.

						 MetadataMatch() { fuzzy = False; }
	};
};

#endif
