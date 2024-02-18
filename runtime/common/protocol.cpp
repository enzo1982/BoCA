 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
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
#include <boca/common/utilities.h>

namespace BoCA
{
	struct CallbackInfo
	{
		Protocol		*protocol;
		Array<String>		 messages;
		Array<MessageType>	 types;
		Array<Int64>		 ticks;
	};
}

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

Void BoCA::Protocol::Lock()
{
	mutex.Lock();
}

Void BoCA::Protocol::Release()
{
	mutex.Release();
}

Void BoCA::Protocol::WriteMessage(Protocol *protocol, const String &message, MessageType messageType, Int64 ticks)
{
	protocol->messages.Add(Utilities::ConvertTicksToTimestamp(ticks, True).Append(" - ").Append(message));

	if	(messageType == MessageTypeWarning) protocol->warnings.Add(message);
	else if (messageType == MessageTypeError)   protocol->errors.Add(message);
}

Int BoCA::Protocol::Write(const String &message, MessageType messageType)
{
	static Array<CallbackInfo *>	 callbackInfos;

	UnsignedInt64	 ticks = S::System::System::Clock() - startTicks;

	Lock();

	/* Check if we are called from a callback.
	 */
	foreach (CallbackInfo *callbackInfo, callbackInfos)
	{
		if (callbackInfo->protocol != this) continue;

		callbackInfo->messages.Add(message);
		callbackInfo->types.Add(messageType);
		callbackInfo->ticks.Add(ticks);

		Release();

		return Success();
	}

	/* Write message to protocol.
	 */
	WriteMessage(this, message, messageType, ticks);

	/* Add callback info and trigger callback.
	 */
	CallbackInfo	 callbackInfo = { this };

	Int	 index = callbackInfos.Add(&callbackInfo);

	onUpdateProtocol.Emit(name);

	/* Check for messages added to the callback info.
	 */
	foreach (const String &message, callbackInfo.messages)
	{
		WriteMessage(this, message, callbackInfo.types.GetNth(foreachindex), callbackInfo.ticks.GetNth(foreachindex));

		onUpdateProtocol.Emit(name);
	}

	callbackInfos.Remove(index);

	Release();

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
