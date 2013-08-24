 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "winamp_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::WinampOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (winamp_out_plugins.Length() > 0)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Winamp Output Plug-In Adapter</name>	\
		    <version>1.0</version>			\
		    <id>winamp-out</id>				\
		    <type>output</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWinampDLLs();
}

Void smooth::DetachDLL()
{
	FreeWinampDLLs();
}

BoCA::WinampOut::WinampOut()
{
	configLayer = NIL;

	plugin	    = NIL;
}

BoCA::WinampOut::~WinampOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::WinampOut::Activate()
{
	plugin = winamp_out_modules.GetNth(Config::Get()->GetIntValue("WinampOut", "OutputPlugin", 0));

	return (plugin->Open(track.GetFormat().rate, track.GetFormat().channels, track.GetFormat().bits, 0, 0) >= 0);
}

Bool BoCA::WinampOut::Deactivate()
{
	plugin->Close();

	return True;
}

Int BoCA::WinampOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	plugin->Write((char *) (UnsignedByte *) data, size);

	return size;
}

ConfigLayer *BoCA::WinampOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWinampOut();

	return configLayer;
}

Int BoCA::WinampOut::CanWrite()
{
	return plugin->CanWrite();
}

Int BoCA::WinampOut::SetPause(Bool pause)
{
	plugin->Pause(pause);

	return Success();
}

Bool BoCA::WinampOut::IsPlaying()
{
	return plugin->IsPlaying();
}
