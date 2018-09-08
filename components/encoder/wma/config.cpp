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

const String	 BoCA::ConfigureWMA::ConfigID = "WMA";

BoCA::ConfigureWMA::ConfigureWMA()
{
	const Config	*config = Config::Get();

	uncompressed	= config->GetIntValue(ConfigID, "Uncompressed", False);
	autoselect	= config->GetIntValue(ConfigID, "AutoSelectFormat", True);

	useVBR		= config->GetIntValue(ConfigID, "EnableVBR", True);
	use2Pass	= config->GetIntValue(ConfigID, "Enable2Pass", False);

	useVBRSetting	= useVBR;
	use2PassSetting = use2Pass;

	supportCBR1Pass = False;
	supportVBR1Pass = False;
	supportCBR2Pass = False;
	supportVBR2Pass = False;

	quality		= config->GetIntValue(ConfigID, "Quality", 90) / 5;

	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);

	HRESULT	 hr = ex_WMCreateProfileManager(&profileManager);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::WMA");

	group_codec		= new GroupBox(i18n->TranslateString("Select codec"), Point(7, 11), Size(426, 67));

	option_uncompressed	= new OptionBox(i18n->TranslateString("Write uncompressed WMA files"), Point(10, 13), Size(406, 0), &uncompressed, 1);
	option_uncompressed->onAction.Connect(&ConfigureWMA::OnToggleCodec, this);

	option_codec		= new OptionBox(i18n->AddColon(i18n->TranslateString("Use codec")), Point(10, 39), Size(100, 0), &uncompressed, 0);
	option_codec->onAction.Connect(&ConfigureWMA::OnToggleCodec, this);

	option_format		= new OptionBox(i18n->AddColon(i18n->TranslateString("Use format")), Point(10, 39), Size(100, 0), &autoselect, 0);
	option_format->onAction.Connect(&ConfigureWMA::OnToggleFormat, this);

	combo_codec		= new ComboBox(Point(100, 38), Size(200, 0));

	group_codec->Add(option_uncompressed);
	group_codec->Add(option_codec);
	group_codec->Add(combo_codec);

	group_format		= new GroupBox(i18n->TranslateString("Select codec format"), Point(7, 90), Size(426, 93));

	option_autoselect	= new OptionBox(i18n->TranslateString("Automatically select format based on settings and input format"), Point(10, 13), Size(406, 0), &autoselect, 1);
	option_autoselect->onAction.Connect(&ConfigureWMA::OnToggleFormat, this);

	option_format		= new OptionBox(i18n->AddColon(i18n->TranslateString("Use format")), Point(10, 39), Size(100, 0), &autoselect, 0);
	option_format->onAction.Connect(&ConfigureWMA::OnToggleFormat, this);

	option_format->SetWidth(Math::Max(option_codec->GetUnscaledTextWidth(), option_format->GetUnscaledTextWidth()) + 21);
	option_codec->SetWidth(option_format->GetWidth());

	check_vbr		= new CheckBox(i18n->TranslateString("Use VBR encoding"), Point(18 + option_format->GetWidth(), 39), Size(100, 0), &useVBR);
	check_vbr->onAction.Connect(&ConfigureWMA::OnToggleVBR, this);
	check_2pass		= new CheckBox(i18n->TranslateString("Use 2-pass encoding"), Point(225, 39), Size(100, 0), &use2Pass);
	check_2pass->onAction.Connect(&ConfigureWMA::OnToggle2Pass, this);

	check_vbr->SetWidth(Math::Max(120, Math::Max(check_vbr->GetUnscaledTextWidth(), check_2pass->GetUnscaledTextWidth()) + 21));
	check_2pass->SetWidth(check_vbr->GetWidth());

	combo_format		= new ComboBox(Point(18 + option_codec->GetWidth(), 64), Size(200, 0));
	combo_codec->SetX(combo_format->GetX());

	group_format->Add(option_autoselect);
	group_format->Add(option_format);
	group_format->Add(check_vbr);
	group_format->Add(check_2pass);
	group_format->Add(combo_format);

	group_settings		= new GroupBox(i18n->TranslateString("Codec settings"), Point(7, 195), Size(426, 67));

	check_vbr_setting	= new CheckBox(i18n->TranslateString("Use VBR encoding"), Point(10, 14), Size(200, 0), &useVBRSetting);
	check_vbr_setting->onAction.Connect(&ConfigureWMA::OnToggleVBRSetting, this);

	check_2pass_setting	= new CheckBox(i18n->TranslateString("Use 2-pass encoding"), Point(10, 40), Size(200, 0), &use2PassSetting);
	check_2pass_setting->onAction.Connect(&ConfigureWMA::OnToggle2PassSetting, this);

	check_vbr_setting->SetWidth(Math::Max(check_vbr_setting->GetUnscaledTextWidth(), check_2pass_setting->GetUnscaledTextWidth()) + 21);
	check_2pass_setting->SetWidth(check_vbr_setting->GetWidth());

	text_quality		= new Text(i18n->AddColon(i18n->TranslateString("Quality")), Point(check_vbr_setting->GetWidth() + 18, 16));
	text_bitrate		= new Text(i18n->AddColon(i18n->TranslateString("Target bitrate")), Point(check_vbr_setting->GetWidth() + 18, 42));

	Int	 maxTextSize = Math::Max(text_quality->GetUnscaledTextWidth(), text_bitrate->GetUnscaledTextWidth());

	slider_quality		= new Slider(Point(text_quality->GetX() + maxTextSize + 7, 13), Size(100, 0), OR_HORZ, &quality, 0, 20);
	slider_quality->onValueChange.Connect(&ConfigureWMA::OnSetQuality, this);

	text_quality_value	= new Text(String::FromInt(quality * 5), Point(35, 16));
	text_quality_value->SetOrientation(OR_UPPERRIGHT);

	combo_bitrate		= new ComboBox(Point(slider_quality->GetX(), 39), Size(100, 0));
	combo_bitrate->AddEntry("32");
	combo_bitrate->AddEntry("48");
	combo_bitrate->AddEntry("64");
	combo_bitrate->AddEntry("80");
	combo_bitrate->AddEntry("96");
	combo_bitrate->AddEntry("128");
	combo_bitrate->AddEntry("160");
	combo_bitrate->AddEntry("192");
	combo_bitrate->SelectEntry(String::FromInt(config->GetIntValue(ConfigID, "Bitrate", 128)));

	text_bitrate_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Trim(), Point(35, 42));
	text_bitrate_kbps->SetOrientation(OR_UPPERRIGHT);

	group_settings->Add(check_vbr_setting);
	group_settings->Add(check_2pass_setting);
	group_settings->Add(text_quality);
	group_settings->Add(slider_quality);
	group_settings->Add(text_quality_value);
	group_settings->Add(text_bitrate);
	group_settings->Add(combo_bitrate);
	group_settings->Add(text_bitrate_kbps);

	Int	 maxSize = Math::Max(Math::Max(option_uncompressed->GetUnscaledTextWidth(), option_autoselect->GetUnscaledTextWidth()) + 21,
				     Math::Max(option_format->GetWidth() + check_vbr->GetWidth() + check_2pass->GetWidth() + 16, check_vbr_setting->GetWidth() + maxTextSize + 120));

	option_uncompressed->SetWidth(maxSize);
	option_autoselect->SetWidth(maxSize);

	combo_codec->SetWidth(maxSize - option_codec->GetWidth() - 8);
	combo_format->SetWidth(maxSize - option_codec->GetWidth() - 8);

	check_vbr->SetWidth((maxSize - option_format->GetWidth() - 16) / 2);
	check_2pass->SetX(check_vbr->GetX() + check_vbr->GetWidth() + 8 + (maxSize - option_format->GetWidth()) % 2);
	check_2pass->SetWidth(check_vbr->GetWidth());

	slider_quality->SetWidth(maxSize - maxTextSize - text_quality->GetX() - 30);
	combo_bitrate->SetWidth(slider_quality->GetWidth());

	group_codec->SetWidth(maxSize + 20);
	group_format->SetWidth(maxSize + 20);
	group_settings->SetWidth(maxSize + 20);

	Add(group_codec);
	Add(group_format);
	Add(group_settings);

	FillCodecComboBox();

	combo_codec->onSelectEntry.Connect(&ConfigureWMA::OnSelectCodec, this);

	if (config->GetIntValue(ConfigID, "Codec", -1) >= 0) combo_codec->SelectNthEntry(config->GetIntValue(ConfigID, "Codec", -1));

	combo_format->SelectNthEntry(config->GetIntValue(ConfigID, "CodecFormat", 0));

	OnToggleCodec();
	OnToggleFormat();

	OnToggleVBRSetting();
	OnToggle2PassSetting();

	/* ToDo: Implement 2-pass encoding.
	 *
	 */
	check_2pass->Deactivate();
	check_2pass_setting->Deactivate();

	SetSize(Size(maxSize + 34, 269));
}

BoCA::ConfigureWMA::~ConfigureWMA()
{
	DeleteObject(group_codec);
	DeleteObject(option_uncompressed);
	DeleteObject(option_codec);
	DeleteObject(combo_codec);

	DeleteObject(group_format);
	DeleteObject(option_autoselect);
	DeleteObject(option_format);
	DeleteObject(check_vbr);
	DeleteObject(check_2pass);
	DeleteObject(combo_format);

	DeleteObject(group_settings);
	DeleteObject(check_vbr_setting);
	DeleteObject(check_2pass_setting);
	DeleteObject(text_quality);
	DeleteObject(slider_quality);
	DeleteObject(text_quality_value);
	DeleteObject(text_bitrate);
	DeleteObject(combo_bitrate);
	DeleteObject(text_bitrate_kbps);

	profileManager->Release();

	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

Int BoCA::ConfigureWMA::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Uncompressed", uncompressed);
	config->SetIntValue(ConfigID, "Codec", combo_codec->GetSelectedEntryNumber());

	config->SetIntValue(ConfigID, "AutoSelectFormat", autoselect);
	config->SetIntValue(ConfigID, "CodecFormat", combo_format->GetSelectedEntryNumber());

	if (autoselect)
	{
		config->SetIntValue(ConfigID, "EnableVBR", useVBRSetting);
		config->SetIntValue(ConfigID, "Enable2Pass", use2PassSetting);
	}
	else
	{
		config->SetIntValue(ConfigID, "EnableVBR", useVBR);
		config->SetIntValue(ConfigID, "Enable2Pass", use2Pass);
	}

	config->SetIntValue(ConfigID, "Bitrate", combo_bitrate->GetSelectedEntry()->GetText().ToInt());
	config->SetIntValue(ConfigID, "Quality", quality * 5);

	return Success();
}

Void BoCA::ConfigureWMA::FillCodecComboBox()
{
	combo_codec->RemoveAllEntries();

	IWMCodecInfo3	*codecInfo = NIL;
	HRESULT		 hr	   = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

	if (hr == S_OK)
	{
		DWORD	 numCodecs = 0;

		hr = codecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);

		for (DWORD i = 0; i < numCodecs; i++)
		{
			DWORD	 nameLen = 0;

			hr = codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, NIL, &nameLen);

			WCHAR	*name = new WCHAR [nameLen];

			hr = codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, name, &nameLen);

			combo_codec->AddEntry(name);

			if ( String(name).Contains("Windows Media Audio") &&
			    !String(name).Contains("Voice")		  &&
			    !String(name).Contains("Lossless")		  &&
			    !String(name).Contains("Pro")) combo_codec->SelectNthEntry(i);

			delete [] name;
		}

		codecInfo->Release();
	}
}

Void BoCA::ConfigureWMA::FillFormatComboBox()
{
	static UnsignedInt	 prevCodec	= -1;
	static Bool		 prevUseVBR	= False;
	static Bool		 prevUse2Pass	= False;

	DWORD	 codecIndex = combo_codec->GetSelectedEntryNumber();

	if (combo_format->Length() > 0 &&
	    codecIndex == prevCodec    &&
	    useVBR     == prevUseVBR   &&
	    use2Pass   == prevUse2Pass) return;

	prevCodec    = codecIndex;
	prevUseVBR   = useVBR;
	prevUse2Pass = use2Pass;

	combo_format->RemoveAllEntries();

	IWMCodecInfo3		*codecInfo = NIL;
	HRESULT			 hr	   = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

	if (hr == S_OK)
	{
		BOOL	 falseValue = FALSE;
		BOOL	 trueValue = TRUE;
		DWORD	 oneValue = 1;
		DWORD	 twoValue = 2;

		if (useVBR)   hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue,  sizeof(BOOL));
		else	      hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));

		if (use2Pass) hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));
		else	      hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

		DWORD	 numFormats = 0;

		hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

		for (DWORD i = 0; i < numFormats; i++)
		{
			DWORD	 nameLen = 0;

			hr = codecInfo->GetCodecFormatDesc(WMMEDIATYPE_Audio, codecIndex, i, NIL, NIL, &nameLen);

			WCHAR	*name = new WCHAR [nameLen];

			hr = codecInfo->GetCodecFormatDesc(WMMEDIATYPE_Audio, codecIndex, i, NIL, name, &nameLen);

			combo_format->AddEntry(name);

			delete [] name;
		}

		codecInfo->Release();
	}

	combo_format->SelectNthEntry(1);
	combo_format->SelectNthEntry(0);
}

Void BoCA::ConfigureWMA::OnToggleCodec()
{
	if (uncompressed)
	{
		combo_codec->Deactivate();

		group_format->Deactivate();
		group_settings->Deactivate();
	}
	else
	{
		combo_codec->Activate();

		group_format->Activate();

		OnToggleFormat();
	}
}

Void BoCA::ConfigureWMA::OnSelectCodec()
{
	IWMCodecInfo3	*codecInfo = NIL;
	HRESULT		 hr	   = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

	if (hr == S_OK)
	{
		DWORD	 codecIndex = combo_codec->GetSelectedEntryNumber();

		supportCBR1Pass = False;
		supportVBR1Pass = False;
		supportCBR2Pass = False;
		supportVBR2Pass = False;

		/* Check if CBR is supported.
		 */
		{
			BOOL	 falseValue = FALSE;
			DWORD	 oneValue = 1;

			hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &falseValue, sizeof(BOOL));
			hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

			DWORD	 numFormats = 0;

			hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

			if (hr == S_OK && numFormats > 0)
			{
				supportCBR1Pass = True;

				DWORD	 twoValue = 2;

				hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));

				if (hr == S_OK)
				{
					hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

					if (hr == S_OK && numFormats > 0) supportCBR2Pass = True;
				}
			}
		}

		/* Check if VBR is supported.
		 */
		{
			WMT_ATTR_DATATYPE	 valueType = WMT_TYPE_BOOL;
			DWORD			 valueSize = sizeof(BOOL);
			BOOL			 isVBRSupported = FALSE;

			hr = codecInfo->GetCodecProp(WMMEDIATYPE_Audio, codecIndex, g_wszIsVBRSupported, &valueType, (BYTE *) &isVBRSupported, &valueSize);

			if (isVBRSupported)
			{
				BOOL	 trueValue = TRUE;
				DWORD	 oneValue = 1;

				hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszVBREnabled, WMT_TYPE_BOOL, (BYTE *) &trueValue, sizeof(BOOL));
				hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &oneValue, sizeof(DWORD));

				if (hr == S_OK)
				{
					DWORD	 numFormats = 0;

					hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

					if (hr == S_OK && numFormats > 0)
					{
						supportVBR1Pass = True;

						DWORD	 twoValue = 2;

						hr = codecInfo->SetCodecEnumerationSetting(WMMEDIATYPE_Audio, codecIndex, g_wszNumPasses, WMT_TYPE_DWORD, (BYTE *) &twoValue, sizeof(DWORD));

						if (hr == S_OK)
						{
							hr = codecInfo->GetCodecFormatCount(WMMEDIATYPE_Audio, codecIndex, &numFormats);

							if (hr == S_OK && numFormats > 0) supportVBR2Pass = True;
						}
					}
				}
			}
		}

		codecInfo->Release();
	}

	if ( (supportVBR1Pass || supportVBR2Pass) &&
	    !(supportCBR1Pass || supportCBR2Pass)) { useVBR = True; useVBRSetting = True; }

	if (!(supportVBR1Pass || supportVBR2Pass) &&
	     (supportCBR1Pass || supportCBR2Pass)) { useVBR = False; useVBRSetting = False; }

	OnToggleVBR();
	OnToggle2Pass();

	OnToggleVBRSetting();
	OnToggle2PassSetting();

	if ((supportVBR1Pass || supportVBR2Pass) &&
	    (supportCBR1Pass || supportCBR2Pass)) check_vbr_setting->Activate();
	else					  check_vbr_setting->Deactivate();

/* ToDo: Implement 2-pass encoding.
 *
 */
/*	if ((supportCBR1Pass || supportVBR1Pass) &&
	    (supportCBR2Pass || supportVBR2Pass)) check_2pass_setting->Activate();
	else					  check_2pass_setting->Deactivate();
*/
}

Void BoCA::ConfigureWMA::OnToggleFormat()
{
	if (autoselect)
	{
		check_vbr->Deactivate();
		check_2pass->Deactivate();
		combo_format->Deactivate();

		group_settings->Activate();

/* ToDo: Implement 2-pass encoding.
 *
 */
		check_2pass_setting->Deactivate();
	}
	else
	{
		combo_format->Activate();

		group_settings->Deactivate();

		OnToggleVBR();
		OnToggle2Pass();
	}
}

Void BoCA::ConfigureWMA::OnToggleVBR()
{
/* ToDo: Implement 2-pass encoding.
 *
 */
/*	if (useVBR)
	{
		if	( supportVBR1Pass &&  supportVBR2Pass)	check_2pass->Activate();
		else						check_2pass->Deactivate();
	}
	else
	{
		if	( supportCBR1Pass &&  supportCBR2Pass)	check_2pass->Activate();
		else						check_2pass->Deactivate();
	}
*/
	FillFormatComboBox();
}

Void BoCA::ConfigureWMA::OnToggle2Pass()
{
	if (use2Pass)
	{
		if	( supportCBR2Pass &&  supportVBR2Pass)	check_vbr->Activate();
		else						check_vbr->Deactivate();
	}
	else
	{
		if	( supportCBR1Pass &&  supportVBR1Pass)	check_vbr->Activate();
		else						check_vbr->Deactivate();
	}

	FillFormatComboBox();
}

Void BoCA::ConfigureWMA::OnToggleVBRSetting()
{
	if (useVBRSetting)
	{
		text_bitrate->Deactivate();
		combo_bitrate->Deactivate();
		text_bitrate_kbps->Deactivate();

		text_quality->Activate();
		slider_quality->Activate();
		text_quality_value->Activate();
	}
	else
	{
		text_bitrate->Activate();
		combo_bitrate->Activate();
		text_bitrate_kbps->Activate();

		text_quality->Deactivate();
		slider_quality->Deactivate();
		text_quality_value->Deactivate();
	}
}

Void BoCA::ConfigureWMA::OnToggle2PassSetting()
{
}

Void BoCA::ConfigureWMA::OnSetQuality()
{
	text_quality_value->SetText(String::FromInt(quality * 5));
}
