 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_COMMUNICATION_JOBLIST
#define H_BOCA_COMMUNICATION_JOBLIST

#include <smooth.h>
#include "../metadata/track.h"

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

			Bool				 locked;
		public:
			/* Returns a new or existing instance of JobList
			 */
			static JobList			*Get();

			/* Destroys an existing instance of JobList
			 */
			static Void			 Free();

			Int				 Lock();
			Int				 Unlock();
		accessors:
			Bool				 IsLocked();
		signals:
			/* Signals fired by application to notify components of
			 * joblist related actions.
			 */
			Signal1<Void, const Track &>	 onApplicationAddTrack;
			Signal1<Void, const Track &>	 onApplicationModifyTrack;
			Signal1<Void, const Track &>	 onApplicationRemoveTrack;
			Signal1<Void, const Track &>	 onApplicationSelectTrack;

			Signal0<Void>			 onApplicationRemoveAllTracks;

			Signal1<Void, const Track &>	 onApplicationMarkTrack;
			Signal1<Void, const Track &>	 onApplicationUnmarkTrack;

			/* Signals fired by components to notify application of
			 * joblist related actions.
			 *
			 * Component developers must be aware that the application
			 * will confirm the signal by firing the corresponding
			 * application signal. Components must not fire the component
			 * signal again in that case.
			 */
			Signal1<Void, const Track &>	 onComponentAddTrack;
			Signal1<Void, const Track &>	 onComponentModifyTrack;
			Signal1<Void, const Track &>	 onComponentRemoveTrack;
			Signal1<Void, const Track &>	 onComponentSelectTrack;

			Signal1<Void, const Track &>	 onComponentMarkTrack;
			Signal1<Void, const Track &>	 onComponentUnmarkTrack;

			/* Callbacks that can be called by components to request
			 * the corresponding action to be performed.
			 */
			Callback0<Bool>			 doRemoveAllTracks;

			/* Callbacks that can be called by components to get
			 * information about current joblist state.
			 */
			Callback0<const Array<Track> *>	 getTrackList;
	};
};

#endif
