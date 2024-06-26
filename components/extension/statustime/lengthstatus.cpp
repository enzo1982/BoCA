 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "lengthstatus.h"

const String &BoCA::LengthStatus::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Total Track Length Display</name>	\
	    <version>1.0</version>			\
	    <id>statustime-ext</id>			\
	    <type>extension</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::LengthStatus::LengthStatus()
{
	statusBarLayer = NIL;

	getStatusBarLayer.Connect(&LengthStatus::GetStatusBarLayer, this);
}

BoCA::LengthStatus::~LengthStatus()
{
	if (statusBarLayer != NIL) Object::DeleteObject(statusBarLayer);
}

Layer *BoCA::LengthStatus::GetStatusBarLayer()
{
	if (statusBarLayer == NIL) statusBarLayer = new LayerLengthStatus();

	return statusBarLayer;
}
