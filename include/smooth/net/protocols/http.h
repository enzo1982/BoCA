 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_PROTOCOL_HTTP_
#define _H_OBJSMOOTH_PROTOCOL_HTTP_

#include "protocol.h"

namespace smooth
{
	namespace Net
	{
		namespace Protocols
		{
			class SMOOTHAPI Parameter
			{
				public:
					String	 key;
					String	 value;

					Bool	 isFile;

						 Parameter(Int null = 0) { isFile = False; }
			};

			class SMOOTHAPI HTTP : public Protocol
			{
				private:
					Int			 mode;

					String			 server;
					String			 path;
					Int			 port;

					String			 proxy;
					Int			 proxyPort;
					Int			 proxyMode;
					String			 proxyUser;
					String			 proxyPass;

					Array<Parameter>	 fields;
					Array<Parameter>	 parameters;
					Buffer<UnsignedByte>	 requestBuffer;

					String			 content;

					Error			 DecodeURL();

					Buffer<UnsignedByte>	&ComposeHTTPRequest();

					Void			 ComposeGETRequest();
					Void			 ComposePOSTRequest();

					String			 ComposeHeader();

					String			 GetParametersURLEncoded();
				public:
								 HTTP(const String &);
					virtual			~HTTP();

					Int			 SetHeaderField(const String &, const String &);	// Set header field

					Int			 SetParameter(const String &, const String &);		// Set parameter
					Int			 SetParameterFile(const String &, const String &);	// Set file to be transmitted; forces HTTP_METHOD_POST

					Int			 SetMode(Int);						// Set mode (get or post)

					Int			 SetContent(const String &);				// Set content to be transmitted; forces HTTP_METHOD_POST

					Int			 SetProxy(const String &, Int);				// Set proxy host name and port
					Int			 SetProxyMode(Int);					// Set proxy mode
					Int			 SetProxyAuth(const String &, const String &);		// Set user name and password for proxy

					Int			 DownloadToFile(const String &);
			};

			const Int	 HTTP_METHOD_GET	= 0;
			const Int	 HTTP_METHOD_POST	= 1;

			const Int	 HTTP_PROXY_NONE	= 0;
			const Int	 HTTP_PROXY_HTTP	= 1;
			const Int	 HTTP_PROXY_HTTPS	= 2;
			const Int	 HTTP_PROXY_SOCKS4	= 3;
			const Int	 HTTP_PROXY_SOCKS5	= 4;
		};
	};
};

#endif
