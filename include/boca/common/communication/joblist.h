 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_JOBLIST
#define H_BOCA_JOBLIST

#include <smooth.h>
#include "../../core/definitions.h"
#include "../track.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT JobList
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static JobList			*instance;

							 JobList();
							~JobList();
		public:
			/* Returns a new or existing instance of JobList
			 */
			static JobList			*Get();

			/* Destroys an existing instance of JobList
			 */
			static Void			 Free();
		signals:
			Signal1<Void, const Track &>	 onApplicationAddTrack;
			Signal1<Void, const Track &>	 onApplicationModifyTrack;
			Signal1<Void, const Track &>	 onApplicationRemoveTrack;
			Signal1<Void, const Track &>	 onApplicationSelectTrack;

			Signal1<Void, const Track &>	 onComponentAddTrack;
			Signal1<Void, const Track &>	 onComponentModifyTrack;
			Signal1<Void, const Track &>	 onComponentRemoveTrack;
			Signal1<Void, const Track &>	 onComponentSelectTrack;
	};
};

#endif
