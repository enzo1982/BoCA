 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
			static Application		*instance;

							 Application();
							~Application();
		public:
			/* Returns a new or existing instance of Application
			 */
			static Application		*Get();

			/* Destroys an existing instance of Application
			 */
			static Void			 Free();
		signals:
			Signal0<Void>			 onQuit;	// called when application is about to quit
	};
};

#endif
