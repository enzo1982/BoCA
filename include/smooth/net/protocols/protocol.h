 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_PROTOCOL
#define H_OBJSMOOTH_PROTOCOL

#include "../../definitions.h"
#include "../../templates/signals.h"

namespace smooth
{
	namespace Net
	{
		namespace Protocols
		{
			abstract class SMOOTHAPI Protocol
			{
				private:
					static Array<Protocol *(*)(const String &), Void *>	*protocol_creators;
					static Array<String>					*protocol_magics;
				protected:
					String							 url;
				public:
					static Int						 AddProtocol(Protocol *(*)(const String &), const String &);

					static Protocol						*CreateForURL(const String &);

												 Protocol(const String &);
					virtual							~Protocol();

					virtual Int						 DownloadToFile(const String &) = 0;

					Int							 DownloadToBuffer(Buffer<UnsignedByte> &);
				signals:
					Signal1<Void, Int>					 uploadProgress;
					Signal1<Void, const String &>				 uploadSpeed;

					Signal1<Void, Int>					 downloadProgress;
					Signal1<Void, const String &>				 downloadSpeed;
			};
		};
	};
};

#endif
