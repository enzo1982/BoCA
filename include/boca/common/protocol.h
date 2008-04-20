 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_PROTOCOL_
#define H_BOCA_PROTOCOL_

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Protocol
	{
		private:
			/* Managed class, therefore private constructor/destructor
			 */
			static Array<Protocol *>		 protocols;

			String					 name;

			Array<String>				 messages;

			Array<String>				 warnings;
			Array<String>				 errors;

								 Protocol(const String &);
								~Protocol();
		public:
			Int					 Write(const String &);

			Int					 WriteWarning(const String &);
			Int					 WriteError(const String &);

			/* Returns a new or existing instance of Protocol
			 */
			static Protocol				*Get(const String &);

			/* Destroys a given instance of Protocol
			 */
			static Bool				 Free(const String &);

			/* Returns all existing instances of Protocol
			 */
			static const Array<Protocol *>		&Get();

			/* Destroys all existing instances of Protocol
			 */
			static Void				 Free();
		accessors:
			const String				&GetName()		{ return name; }
			String					 GetProtocolText();

			Array<String>				&GetWarnings()		{ return warnings; }
			Array<String>				&GetErrors()		{ return errors; }
		signals:
			static Signal0<Void>			 onUpdateProtocolList;
			static Signal1<Void, const String &>	 onUpdateProtocol;
	};
};

#endif
