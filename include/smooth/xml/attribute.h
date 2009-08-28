 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XMLATTRIBUTE
#define H_OBJSMOOTH_XMLATTRIBUTE

namespace smooth
{
	namespace XML
	{
		class Attribute;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "node.h"

namespace smooth
{
	namespace XML
	{
		class SMOOTHAPI Attribute
		{
			private:
				static Array<String>	 attributeNames;

				Int			 attributeID;

				Int			 nameIndex;
				String			 content;
			public:
							 Attribute(const String &, const String &);
							~Attribute();

				Int			 GetAttributeID() const;
				Int			 SetAttributeID(Int);

				const String		&GetName() const;
				Int			 SetName(const String &);

				const String		&GetContent() const;
				Int			 SetContent(const String &);
		};
	};
};

#endif
