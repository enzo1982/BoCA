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

#ifndef H_BOCA_COMMUNICATION_APPLICATION
#define H_BOCA_COMMUNICATION_APPLICATION

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Application
	{
		private:
			/* Singleton class, therefore private constructor/destructor
			 */
			static Application	*instance;

						 Application();
						~Application();
		public:
			/* Returns a new or existing instance of Application
			 */
			static Application	*Get();

			/* Destroys an existing instance of Application
			 */
			static Void		 Free();
		callbacks:
			Callback0<String>	 getScreenName;		// returns the application's full name string

			Callback0<String>	 getClientName;		// returns the application's short name string
			Callback0<String>	 getClientVersion;	// returns the application's version string
		signals:
			Signal0<Void>		 onQuit;		// called when application is about to quit
	};
};

#endif
