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

#ifndef H_BOCA_PROXYCONFIG
#define H_BOCA_PROXYCONFIG

#include <smooth.h>

using namespace smooth;

namespace BoCA
{
	/* The proxy settings that metadata providers use for network requests.
	 * fre:ac edits them and keeps them in its freedb configuration category.
	 */
	namespace ProxyConfig
	{
		const char * const	 ConfigID		= "freedb";

		const char * const	 ModeID			= "ProxyMode";
		const Int		 ModeDefault		= 0;

		const char * const	 ServerID		= "Proxy";
		const char * const	 ServerDefault		= "localhost";

		const char * const	 PortID			= "ProxyPort";
		const Int		 PortDefault		= 1080;

		const char * const	 UserID			= "ProxyUserName";
		const char * const	 PasswordID		= "ProxyPassword";
	};
};

#endif
