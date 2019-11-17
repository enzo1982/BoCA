 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_COMMUNICATION_ENGINE
#define H_BOCA_COMMUNICATION_ENGINE

#include <smooth.h>
#include "../metadata/track.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Engine
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Engine					*instance;

									 Engine();
									~Engine();
		public:
			/* Returns a new or existing instance of Engine
			 */
			static Engine					*Get();

			/* Destroys an existing instance of Engine
			 */
			static Void					 Free();
		signals:
			/* One time initialization and cleanup signals.
			 */
			Signal0<Void>					 onInitialize;
			Signal0<Void>					 onCleanup;

			/* Signals fired by application to notify components of
			 * conversion engine related actions.
			 */
			Signal2<Void, Int, const Array<Track> &>	 onStartConversion;
			Signal1<Void, Int>				 onFinishConversion;
			Signal1<Void, Int>				 onCancelConversion;

			Signal2<Void, Int, const String &>		 onSingleFileConversion;

			Signal2<Void, Int, const Track &>		 onStartTrackConversion;
			Signal2<Void, Int, const Track &>		 onFinishTrackConversion;
			Signal2<Void, Int, const Track &>		 onCancelTrackConversion;
	};
};

#endif
