 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_VIDEOSITE
#define H_YOUTUBE_VIDEOSITE

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include <v8/v8.h>

namespace BoCA
{
	class Metadata
	{
		public:
			String	 title;
			String	 description;

			String	 date;
			String	 thumbnail;

			String	 uploader;
	};

	class VideoSite
	{
		private:
			String				 name;
			String				 version;

			String				 decoder;

			String				 script;

			v8::HandleScope			 handleScope;

			v8::Persistent<v8::Context>	 context;
			v8::Local<v8::Script>		 compiled;

			Bool				 CreateScriptContext();
			Bool				 DestroyScriptContext();

			Int				 ParseXML(const String &);

			String				 ReplaceInnerHTML(const String &);
		public:
							 VideoSite(const String &);
							~VideoSite();

			Bool				 IsSane();

			Bool				 CanHandleURL(const String &);

			String				 GetVideoURL(const String &);

			Metadata			 QueryMetadata(const String &);
		accessors:
			const String			&GetName()	{ return name; }
			const String			&GetVersion()	{ return version; }

			const String			&GetDecoderID()	{ return decoder; }
	};
};

#endif
