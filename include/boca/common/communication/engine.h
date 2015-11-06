 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
			static Engine			*instance;

							 Engine();
							~Engine();
		public:
			/* Returns a new or existing instance of Engine
			 */
			static Engine			*Get();

			/* Destroys an existing instance of Engine
			 */
			static Void			 Free();
		signals:
			/* Signals fired by application to notify components of
			 * conversion engine related actions.
			 */
			Signal1<Void, const Track &>	 onStartConversion;
			Signal1<Void, const Track &>	 onFinishConversion;
			Signal1<Void, const Track &>	 onCancelConversion;
	};
};

#endif
