 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XMLDOCUMENT
#define H_OBJSMOOTH_XMLDOCUMENT

namespace smooth
{
	namespace XML
	{
		class Document;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "node.h"

typedef struct _xmlTextReader xmlTextReader;
typedef struct _xmlTextWriter xmlTextWriter;

typedef xmlTextReader *xmlTextReaderPtr;
typedef xmlTextWriter *xmlTextWriterPtr;

namespace smooth
{
	namespace XML
	{
		class SMOOTHAPI Document
		{
			private:
				String	 encoding;

				Node	*rootNode;
				Bool	 ownRoot;

				Int	 LoadNode(xmlTextReaderPtr, Node *);
				Int	 SaveNode(xmlTextWriterPtr, Node *);
			public:
					 Document();
					~Document();

				Node	*GetRootNode() const;
				Int	 SetRootNode(Node *);

				Int	 SetEncoding(const String &);

				Int	 LoadFile(const String &);
				Int	 SaveFile(const String &);

				Int	 ParseMemory(const Void *, Int);
		};
	};
};

#endif
