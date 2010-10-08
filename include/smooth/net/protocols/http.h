 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_PROTOCOL_HTTP
#define H_OBJSMOOTH_PROTOCOL_HTTP

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
					Short			 mode;

					String			 server;
					String			 path;
					Int			 port;

					String			 proxy;
					Int			 proxyPort;
					Short			 proxyMode;
					String			 proxyUser;
					String			 proxyPass;

					Array<Parameter>	 requestFields;
					Array<Parameter>	 requestParameters;
					Buffer<UnsignedByte>	 requestBuffer;

					Array<Parameter>	 responseFields;

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

					Int			 SetMode(Short);					// Set mode (get or post)

					Int			 SetContent(const String &);				// Set content to be transmitted; forces HTTP_METHOD_POST

					Int			 SetProxy(const String &, Int);				// Set proxy host name and port
					Int			 SetProxyMode(Short);					// Set proxy mode
					Int			 SetProxyAuth(const String &, const String &);		// Set user name and password for proxy

					String			 GetResponseHeaderField(const String &);

					Int			 DownloadToFile(const String &);
			};

			const Short	 HTTP_METHOD_GET	= 0;
			const Short	 HTTP_METHOD_POST	= 1;

			const Short	 HTTP_PROXY_NONE	= 0;
			const Short	 HTTP_PROXY_HTTP	= 1;
			const Short	 HTTP_PROXY_HTTPS	= 2;
			const Short	 HTTP_PROXY_SOCKS4	= 3;
			const Short	 HTTP_PROXY_SOCKS5	= 4;
		};
	};
};

#endif
