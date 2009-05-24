 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer.h"

BoCA::LayerProtocols::LayerProtocols() : Layer("Protocols")
{
	text_protocol	= new Text("Protocol:", Point(7, 11));

	combo_protocol	= new ComboBox(Point(358, 8), Size(350, 0));
	combo_protocol->onSelectEntry.Connect(&LayerProtocols::SelectProtocol, this);
	combo_protocol->SetOrientation(OR_UPPERRIGHT);

	edit_protocol	= new MultiEdit(NIL, Point(7, 35), Size(500, 360));

	edit_status	= new EditBox(NIL, Point(7, 56), Size(500, 0));
	edit_status->SetOrientation(OR_LOWERLEFT);

	text_errors	= new Text("Errors / Warnings:", Point(7, 25));
	text_errors->SetOrientation(OR_LOWERLEFT);

	edit_errors	= new EditBox("0/0", Point(text_errors->textSize.cx + 14, 28), Size(25, 0));
	edit_errors->SetOrientation(OR_LOWERLEFT);

	combo_errors	= new ComboBox(Point(text_errors->textSize.cx + 47, 28), Size(250, 0));
	combo_errors->SetOrientation(OR_LOWERLEFT);

	button_details	= new Button("Details", NIL, Point(87, 29), Size(80, 0));
	button_details->onAction.Connect(&LayerProtocols::ShowDetails, this);
	button_details->SetOrientation(OR_LOWERRIGHT);

	Add(text_protocol);
	Add(combo_protocol);

	Add(edit_protocol);
	Add(edit_status);

	Add(text_errors);
	Add(edit_errors);
	Add(combo_errors);
	Add(button_details);

	UpdateProtocolList();

	onChangeSize.Connect(&LayerProtocols::OnChangeSize, this);

	Protocol::onUpdateProtocolList.Connect(&LayerProtocols::UpdateProtocolList, this);
	Protocol::onUpdateProtocol.Connect(&LayerProtocols::UpdateProtocol, this);
}

BoCA::LayerProtocols::~LayerProtocols()
{
	Protocol::onUpdateProtocolList.Disconnect(&LayerProtocols::UpdateProtocolList, this);
	Protocol::onUpdateProtocol.Disconnect(&LayerProtocols::UpdateProtocol, this);

	DeleteObject(text_protocol);
	DeleteObject(combo_protocol);

	DeleteObject(edit_protocol);
	DeleteObject(edit_status);

	DeleteObject(text_errors);
	DeleteObject(edit_errors);
	DeleteObject(combo_errors);
	DeleteObject(button_details);
}

Void BoCA::LayerProtocols::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	edit_protocol->SetSize(clientSize - Size(15, 92));
	edit_status->SetWidth(clientSize.cx - 15);

	combo_errors->SetWidth(clientSize.cx - text_errors->textSize.cx - 142);
}

Void BoCA::LayerProtocols::UpdateProtocolList()
{
	combo_protocol->RemoveAllEntries();

	const Array<Protocol *>	&protocols = Protocol::Get();

	foreach (Protocol *protocol, protocols)
	{
		combo_protocol->AddEntry(protocol->GetName());
	}
}

Void BoCA::LayerProtocols::UpdateProtocol(const String &name)
{
	if (combo_protocol->GetSelectedEntry() == NIL) return;

	if (name == combo_protocol->GetSelectedEntry()->GetText()) SelectProtocol();
}

Void BoCA::LayerProtocols::SelectProtocol()
{
	edit_protocol->SetText(Protocol::Get(combo_protocol->GetSelectedEntry()->GetText())->GetProtocolText());
}

Void BoCA::LayerProtocols::ShowDetails()
{
// TODO: Implement error detail view

	Dialogs::QuickMessage("Not implemented, yet!", "Error", MB_OK, IDI_HAND);
}
