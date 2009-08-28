 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_XMLNODE
#define H_OBJSMOOTH_XMLNODE

namespace smooth
{
	namespace XML
	{
		class Node;
	};
};

#include "../definitions.h"
#include "../misc/string.h"
#include "attribute.h"
#include "document.h"

namespace smooth
{
	namespace XML
	{
		class SMOOTHAPI Node
		{
			private:
				static Array<String>		 elementNames;

				Array<Attribute *, Void *>	*attributes;
				Array<Node *, Void *>		*subnodes;

				Int				 nodeID;

				Int				 nameIndex;
				String				 content;
			public:
								 Node(const String &, const String & = NIL);
								~Node();

				Int				 GetNodeID() const;
				Int				 SetNodeID(Int);

				const String			&GetName() const;
				Int				 SetName(const String &);

				const String			&GetContent() const;
				Int				 SetContent(const String &);

/* Functions for attribute access */
				Int				 GetNOfAttributes() const;
				Attribute			*GetNthAttribute(Int) const;
				Attribute			*GetAttributeByName(const String &) const;

				Attribute			*SetAttribute(const String &, const String &);
				Int				 RemoveAttribute(Attribute *);
				Int				 RemoveAttributeByName(const String &);

/* Functions for subnode access */
				Int				 GetNOfNodes() const;
				Node				*GetNthNode(Int) const;
				Node				*GetNodeByName(const String &) const;

				Node				*AddNode(const String &, const String & = NIL);
				Int				 RemoveNode(Node *);
				Int				 RemoveNodeByName(const String &);
		};
	};
};

#endif
