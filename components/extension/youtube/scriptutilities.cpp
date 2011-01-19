 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "scriptutilities.h"

#ifdef __WIN32__
#	include <windows.h>
#endif

using namespace smooth::GUI;
using namespace smooth::GUI::Dialogs;

using namespace smooth::Net;

v8::Handle<v8::Value> BoCA::Alert(const v8::Arguments &args)
{
	String	 text = (char *) *v8::String::AsciiValue(args[0]);

	QuickMessage(text, "Information", MB_OK, IDI_INFORMATION);

	return v8::Undefined();
}

v8::Handle<v8::Value> BoCA::DownloadURL(const v8::Arguments &args)
{
	String	 url = (char *) *v8::String::AsciiValue(args[0]);

	if (url.StartsWith("http://"))
	{
		Protocols::Protocol	*protocol = Protocols::Protocol::CreateForURL(url);
		Buffer<UnsignedByte>	 buffer;

		protocol->DownloadToBuffer(buffer);

		delete protocol;

		if (buffer.Size() > 0)
		{
			String	 result;

			for (Int i = 0; i < buffer.Size(); i++) result[i] = buffer[i];

			return v8::String::New(result);
		}
	}

	return v8::Undefined();
}
