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

#include "config.h"
#include "dllinterface.h"

const String	 BoCA::ConfigureMPG123::ConfigID = "mpg123";

BoCA::ConfigureMPG123::ConfigureMPG123()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Decoders::mpg123");

	String	 selectedDecoder = config->GetStringValue(ConfigID, "Decoder", NIL);

	group_decoding	= new GroupBox(i18n->TranslateString("Decoder"), Point(7, 11), Size(286, 41));

	text_decoder	= new Text(i18n->AddColon(i18n->TranslateString("Active decoder")), Point(10, 15));

	combo_decoder	= new ComboBox(Point(17 + text_decoder->GetUnscaledTextWidth(), 12), Size(259 - text_decoder->GetUnscaledTextWidth(), 0));
	combo_decoder->AddEntry(i18n->TranslateString("auto select"));

	const char	**decoders = ex_mpg123_supported_decoders();

	for (Int i = 0; decoders[i] != NIL; i++) combo_decoder->AddEntry(decoders[i]);

	if (selectedDecoder != NIL) combo_decoder->SelectEntry(selectedDecoder);
	else			    combo_decoder->SelectNthEntry(0);

	group_decoding->Add(text_decoder);
	group_decoding->Add(combo_decoder);

	Add(group_decoding);

	SetSize(Size(300, 169));
}

BoCA::ConfigureMPG123::~ConfigureMPG123()
{
	DeleteObject(group_decoding);
	DeleteObject(text_decoder);
	DeleteObject(combo_decoder);
}

Int BoCA::ConfigureMPG123::SaveSettings()
{
	Config	*config = Config::Get();

	String	 selectedDecoder = combo_decoder->GetSelectedEntry()->GetText();

	if (combo_decoder->GetSelectedEntryNumber() == 0) config->SetStringValue(ConfigID, "Decoder", NIL);
	else						  config->SetStringValue(ConfigID, "Decoder", selectedDecoder);

	return Success();
}
