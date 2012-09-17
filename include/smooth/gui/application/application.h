 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_APPLICATION
#define H_OBJSMOOTH_APPLICATION

namespace smooth
{
	namespace GUI
	{
		class Application;
	};
};

#include "../widgets/widget.h"

namespace smooth
{
	extern Bool	 loopActive;

	namespace GUI
	{
		abstract class SMOOTHAPI Application : public Widget
		{
			private:
				static String			 command;
				static Array<String>		 args;

				static String			 startupDirectory;
				static String			 applicationDirectory;
			public:
				static const Short		 classID;

								 Application(const String & = NIL);
				virtual				~Application();

				Int				 Loop();
			accessors:
				static Void			 SetCommand(const String &nCommand)	{ command = nCommand; }
				static Void			 SetArguments(const Array<String> &);

				static const String		&GetCommand()				{ return command; }
				static const Array<String>	&GetArguments()				{ return args; }

				static String			 GetStartupDirectory();
				static String			 GetApplicationDirectory();
		};
	};
};

#endif
