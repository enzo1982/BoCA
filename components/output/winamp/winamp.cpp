 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "winamp.h"
#include "config.h"

const String &BoCA::OutputWinamp::GetComponentSpecs()
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

BoCA::OutputWinamp::OutputWinamp()
{
	configLayer = NIL;

	plugin	    = NIL;
}

BoCA::OutputWinamp::~OutputWinamp()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::OutputWinamp::Activate()
{
	const Config	*config = GetConfiguration();

	plugin = winamp_out_modules.GetNth(config->GetIntValue("WinampOut", "OutputPlugin", 0));

	return (plugin->Open(track.GetFormat().rate, track.GetFormat().channels, track.GetFormat().bits, 0, 0) >= 0);
}

Bool BoCA::OutputWinamp::Deactivate()
{
	plugin->Close();

	return True;
}

Int BoCA::OutputWinamp::WriteData(Buffer<UnsignedByte> &data)
{
	plugin->Write((char *) (UnsignedByte *) data, data.Size());

	return data.Size();
}

ConfigLayer *BoCA::OutputWinamp::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureWinamp();

	return configLayer;
}

Int BoCA::OutputWinamp::CanWrite()
{
	return plugin->CanWrite();
}

Int BoCA::OutputWinamp::SetPause(Bool pause)
{
	plugin->Pause(pause);

	return Success();
}

Bool BoCA::OutputWinamp::IsPlaying()
{
	return plugin->IsPlaying();
}
