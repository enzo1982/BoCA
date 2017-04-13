 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/protocol.h>

Array<BoCA::Protocol *>		 BoCA::Protocol::protocols;

Signal0<Void>			 BoCA::Protocol::onUpdateProtocolList;
Signal1<Void, const String &>	 BoCA::Protocol::onUpdateProtocol;

BoCA::Protocol::Protocol(const String &iName)
{
	name	   = iName;
	startTicks = S::System::System::Clock();
}

BoCA::Protocol::~Protocol()
{
}

Int BoCA::Protocol::Write(const String &message, MessageType messageType)
{
	UnsignedInt64	 ticks = S::System::System::Clock() - startTicks;

	mutex.Lock();

	messages.Add(String(ticks / 1000 / 60 / 60 <  10 ?			       "0"  : "").Append(String::FromInt(ticks / 1000 / 60 / 60)).Append(":")
		    .Append(ticks / 1000 / 60 % 60 <  10 ?			       "0"  : "").Append(String::FromInt(ticks / 1000 / 60 % 60)).Append(":")
		    .Append(ticks / 1000 % 60	   <  10 ?			       "0"  : "").Append(String::FromInt(ticks / 1000 % 60     )).Append(".")
		    .Append(ticks % 1000	   < 100 ? (ticks % 1000 < 10 ? "00" : "0") : "").Append(String::FromInt(ticks % 1000	       )).Append(" - ").Append(message));

	if	(messageType == MessageTypeWarning) warnings.Add(message);
	else if (messageType == MessageTypeError)   errors.Add(message);

	onUpdateProtocol.Emit(name);

	mutex.Release();

	return Success();
}

String BoCA::Protocol::GetProtocolText() const
{
	return String::Implode(messages, "\n");
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
	foreach (Protocol *protocol, protocols) delete protocol;

	protocols.RemoveAll();
	onUpdateProtocolList.Emit();
}
