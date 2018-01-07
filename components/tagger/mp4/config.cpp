 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureMP4::ConfigID = "MP4";

BoCA::ConfigureMP4::ConfigureMP4()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Taggers::MP4");

	Int	 chapterType = config->GetIntValue(ConfigID, "ChapterType", MP4ChapterTypeAny);

	group_chapter	= new GroupBox(i18n->TranslateString("Chapter format"), Point(7, 11), Size(236, 41));

	text_chapter	= new Text(i18n->AddColon(i18n->TranslateString("Chapter format")), Point(10, 15));

	combo_chapter	= new ComboBox(Point(17 + text_chapter->GetUnscaledTextWidth(), 12), Size(209 - text_chapter->GetUnscaledTextWidth(), 0));
	combo_chapter->AddEntry(i18n->TranslateString("both"));
	combo_chapter->AddEntry("QuickTime");
	combo_chapter->AddEntry("Nero");

	switch (chapterType)
	{
		default:
		case MP4ChapterTypeAny:	 combo_chapter->SelectNthEntry(0); break;
		case MP4ChapterTypeQt:	 combo_chapter->SelectNthEntry(1); break;
		case MP4ChapterTypeNero: combo_chapter->SelectNthEntry(2); break;
	}

	group_chapter->Add(text_chapter);
	group_chapter->Add(combo_chapter);

	Add(group_chapter);

	SetSize(Size(250, 169));
}

BoCA::ConfigureMP4::~ConfigureMP4()
{
	DeleteObject(group_chapter);
	DeleteObject(text_chapter);
	DeleteObject(combo_chapter);
}

Int BoCA::ConfigureMP4::SaveSettings()
{
	Config	*config = Config::Get();

	Int	 chapterType = MP4ChapterTypeAny;

	switch (combo_chapter->GetSelectedEntryNumber())
	{
		case 1:	chapterType = MP4ChapterTypeQt;	  break; // QuickTime
		case 2:	chapterType = MP4ChapterTypeNero; break; // Nero
	}

	config->SetIntValue(ConfigID, "ChapterType", chapterType);

	return Success();
}
