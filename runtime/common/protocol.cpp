 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/protocol.h>

Array<BoCA::Protocol *>	 BoCA::Protocol::protocols;

Signal0<Void>			 BoCA::Protocol::onUpdateProtocolList;
Signal1<Void, const String &>	 BoCA::Protocol::onUpdateProtocol;

BoCA::Protocol::Protocol(const String &iName)
{
	name = iName;
}

BoCA::Protocol::~Protocol()
{
}

Int BoCA::Protocol::Write(const String &message)
{
	messages.Add(message);

	onUpdateProtocol.Emit(name);

	return Success();
}

Int BoCA::Protocol::WriteWarning(const String &message)
{
	warnings.Add(message);

	return Write(message);
}

Int BoCA::Protocol::WriteError(const String &message)
{
	errors.Add(message);

	return Write(message);
}

String BoCA::Protocol::GetProtocolText() const
{
	String	 text;

	foreach (const String &message, messages)
	{
		text.Append(message).Append("\n");
	}

	return text;
}

BoCA::Protocol *BoCA::Protocol::Get(const String &name)
{
	foreach (Protocol *protocol, protocols)
	{
		if (protocol->GetName() == name) return protocol;
	}

	/* Protocol does not exist, yet.
	 */
	Protocol	*protocol = new Protocol(name);

	protocols.Add(protocol);
	onUpdateProtocolList.Emit();

	for (Int i = 0; i < protocols.Length(); i++)
	{Protocol *protocol = protocols.GetNth(i);
		if (protocol->GetName() == name) return protocol;
	}

	return protocol;
}

Bool BoCA::Protocol::Free(const String &name)
{
	for (Int i = 0; i < protocols.Length(); i++)
	{
		Protocol	*protocol = protocols.GetNth(i);

		if (protocol->GetName() == name)
		{
			protocols.RemoveNth(i);
			onUpdateProtocolList.Emit();

			delete protocol;

			return True;
		}
	}

	return False;
}

const Array<BoCA::Protocol *> &BoCA::Protocol::Get()
{
	return protocols;
}

Void BoCA::Protocol::Free()
{
	foreach (Protocol *protocol, protocols)
	{
		delete protocol;
	}

	protocols.RemoveAll();
	onUpdateProtocolList.Emit();
}
