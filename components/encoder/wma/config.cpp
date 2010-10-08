 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"
#include "dllinterface.h"

BoCA::ConfigureWMAEnc::ConfigureWMAEnc()
{
	Config	*config = Config::Get();

	uncompressed = config->GetIntValue("WMA", "Uncompressed", False);
	autoselect = config->GetIntValue("WMA", "AutoSelectFormat", True);

	useVBR = config->GetIntValue("WMA", "EnableVBR", True);
	use2Pass = config->GetIntValue("WMA", "Enable2Pass", False);

	useVBRSetting = useVBR;
	use2PassSetting = use2Pass;

	quality = config->GetIntValue("WMA", "Quality", 90) / 5;

	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);

	HRESULT	 hr = S_OK;

	hr = ex_WMCreateProfileManager(&profileManager);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::WMA");

	group_codec		= new GroupBox(i18n->TranslateString("Select codec"), Point(7, 11), Size(386, 67));

	option_uncompressed	= new OptionBox(i18n->TranslateString("Write uncompressed WMA files"), Point(10, 13), Size(366, 0), &uncompressed, 1);
	option_uncompressed->onAction.Connect(&ConfigureWMAEnc::OnToggleCodec, this);

	option_codec		= new OptionBox(String(i18n->TranslateString("Use codec")).Append(":"), Point(10, 39), Size(100, 0), &uncompressed, 0);
	option_codec->SetWidth(option_codec->textSize.cx + 20);
	option_codec->onAction.Connect(&ConfigureWMAEnc::OnToggleCodec, this);

	combo_codec		= new ComboBox(Point(18 + option_codec->GetWidth(), 38), Size(358 - option_codec->GetWidth(), 0));

	group_codec->Add(option_uncompressed);
	group_codec->Add(option_codec);
	group_codec->Add(combo_codec);

	group_format		= new GroupBox(i18n->TranslateString("Select codec format"), Point(7, 90), Size(386, 93));

	option_autoselect	= new OptionBox(i18n->TranslateString("Automatically select format based on settings and input format"), Point(10, 13), Size(366, 0), &autoselect, 1);
	option_autoselect->onAction.Connect(&ConfigureWMAEnc::OnToggleFormat, this);

	option_format		= new OptionBox(String(i18n->TranslateString("Use format")).Append(":"), Point(10, 39), Size(100, 0), &autoselect, 0);
	option_format->SetWidth(option_codec->textSize.cx + 20);
	option_format->onAction.Connect(&ConfigureWMAEnc::OnToggleFormat, this);

	check_vbr		= new CheckBox(i18n->TranslateString("Use VBR encoding"), Point(18 + option_format->GetWidth(), 39), Size((352 - option_format->GetWidth()) / 2, 0), &useVBR);
	check_vbr->onAction.Connect(&ConfigureWMAEnc::OnToggleVBR, this);
	check_2pass		= new CheckBox(i18n->TranslateString("Use 2-pass encoding"), Point(25 + option_format->GetWidth() + check_vbr->GetWidth(), 39), Size((352 - option_format->GetWidth()) / 2, 0), &use2Pass);
	check_2pass->onAction.Connect(&ConfigureWMAEnc::OnToggle2Pass, this);

	combo_format		= new ComboBox(Point(18 + option_format->GetWidth(), 64), Size(358 - option_format->GetWidth(), 0));

	group_format->Add(option_autoselect);
	group_format->Add(option_format);
	group_format->Add(check_vbr);
	group_format->Add(check_2pass);
	group_format->Add(combo_format);

	group_settings		= new GroupBox(i18n->TranslateString("Codec settings"), Point(7, 195), Size(386, 67));

	check_vbr_setting	= new CheckBox(i18n->TranslateString("Use VBR encoding"), Point(10, 14), Size(180, 0), &useVBRSetting);
	check_vbr_setting->onAction.Connect(&ConfigureWMAEnc::OnToggleVBRSetting, this);
	check_2pass_setting	= new CheckBox(i18n->TranslateString("Use 2-pass encoding"), Point(10, 40), Size(180, 0), &use2PassSetting);
	check_2pass_setting->onAction.Connect(&ConfigureWMAEnc::OnToggle2PassSetting, this);

	text_quality		= new Text(String(i18n->TranslateString("Quality")).Append(":"), Point(197, 16));

	slider_quality		= new Slider(Point(204 + text_quality->textSize.cx, 13), Size(142 - text_quality->textSize.cx, 0), OR_HORZ, &quality, 0, 20);
	slider_quality->onValueChange.Connect(&ConfigureWMAEnc::OnSetQuality, this);

	text_quality_value	= new Text(String::FromInt(quality * 5), Point(353, 16));

	text_bitrate		= new Text(String(i18n->TranslateString("Target bitrate")).Append(":"), Point(197, 42));

	combo_bitrate		= new ComboBox(Point(204 + text_bitrate->textSize.cx, 39), Size(142 - text_bitrate->textSize.cx, 0));
	combo_bitrate->AddEntry("32");
	combo_bitrate->AddEntry("48");
	combo_bitrate->AddEntry("64");
	combo_bitrate->AddEntry("80");
	combo_bitrate->AddEntry("96");
	combo_bitrate->AddEntry("128");
	combo_bitrate->AddEntry("160");
	combo_bitrate->AddEntry("192");
	combo_bitrate->SelectEntry(String::FromInt(config->GetIntValue("WMA", "Bitrate", 128)));

	text_bitrate_kbps	= new Text("kbps", Point(353, 42));

	group_settings->Add(check_vbr_setting);
	group_settings->Add(check_2pass_setting);
	group_settings->Add(text_quality);
	group_settings->Add(slider_quality);
	group_settings->Add(text_quality_value);
	group_settings->Add(text_bitrate);
	group_settings->Add(combo_bitrate);
	group_settings->Add(text_bitrate_kbps);

	Add(group_codec);
	Add(group_format);
	Add(group_settings);

	FillCodecComboBox();

	combo_codec->onSelectEntry.Connect(&ConfigureWMAEnc::OnSelectCodec, this);

	if (config->GetIntValue("WMA", "Codec", -1) >= 0) combo_codec->SelectNthEntry(config->GetIntValue("WMA", "Codec", -1));

	combo_format->SelectNthEntry(config->GetIntValue("WMA", "CodecFormat", 0));

	OnToggleCodec();
	OnToggleFormat();

	OnToggleVBRSetting();
	OnToggle2PassSetting();

	/* ToDo: Implement 2-pass encoding.
	 *
	 */
	check_2pass->Deactivate();
	check_2pass_setting->Deactivate();

	SetSize(Size(400, 269));
}

BoCA::ConfigureWMAEnc::~ConfigureWMAEnc()
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

Int BoCA::ConfigureWMAEnc::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("WMA", "Uncompressed", uncompressed);
	config->SetIntValue("WMA", "Codec", combo_codec->GetSelectedEntryNumber());

	config->SetIntValue("WMA", "AutoSelectFormat", autoselect);
	config->SetIntValue("WMA", "CodecFormat", combo_format->GetSelectedEntryNumber());

	if (autoselect)
	{
		config->SetIntValue("WMA", "EnableVBR", useVBRSetting);
		config->SetIntValue("WMA", "Enable2Pass", use2PassSetting);
	}
	else
	{
		config->SetIntValue("WMA", "EnableVBR", useVBR);
		config->SetIntValue("WMA", "Enable2Pass", use2Pass);
	}

	config->SetIntValue("WMA", "Bitrate", combo_bitrate->GetSelectedEntry()->GetText().ToInt());
	config->SetIntValue("WMA", "Quality", quality * 5);

	return Success();
}

Void BoCA::ConfigureWMAEnc::FillCodecComboBox()
{
	combo_codec->RemoveAllEntries();

	HRESULT			 hr = S_OK;
	IWMProfileManager	*profileManager = NIL;
	IWMCodecInfo3		*codecInfo = NIL;

	hr = ex_WMCreateProfileManager(&profileManager);
	hr = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

	DWORD			 numCodecs = 0;

	hr = codecInfo->GetCodecInfoCount(WMMEDIATYPE_Audio, &numCodecs);

	for (DWORD i = 0; i < numCodecs; i++)
	{
		DWORD	 nameLen = 0;

		hr = codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, NIL, &nameLen);

		WCHAR	*name = new WCHAR [nameLen];

		hr = codecInfo->GetCodecName(WMMEDIATYPE_Audio, i, name, &nameLen);

		combo_codec->AddEntry(name);

		if ( String(name).Contains("Windows Media Audio") &&
		    !String(name).Contains("Voice") &&
		    !String(name).Contains("Lossless")) combo_codec->SelectNthEntry(i);

		delete [] name;
	}

	codecInfo->Release();
	profileManager->Release();
}

Void BoCA::ConfigureWMAEnc::FillFormatComboBox()
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

	HRESULT			 hr = S_OK;
	IWMCodecInfo3		*codecInfo = NIL;

	hr = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

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

	combo_format->SelectNthEntry(1);
	combo_format->SelectNthEntry(0);
}

Void BoCA::ConfigureWMAEnc::OnToggleCodec()
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

Void BoCA::ConfigureWMAEnc::OnSelectCodec()
{
	HRESULT			 hr = S_OK;
	IWMCodecInfo3		*codecInfo = NIL;

	hr = profileManager->QueryInterface(IID_IWMCodecInfo3, (void **) &codecInfo);

	DWORD			 codecIndex = combo_codec->GetSelectedEntryNumber();

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

Void BoCA::ConfigureWMAEnc::OnToggleFormat()
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

Void BoCA::ConfigureWMAEnc::OnToggleVBR()
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

Void BoCA::ConfigureWMAEnc::OnToggle2Pass()
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

Void BoCA::ConfigureWMAEnc::OnToggleVBRSetting()
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

Void BoCA::ConfigureWMAEnc::OnToggle2PassSetting()
{
}

Void BoCA::ConfigureWMAEnc::OnSetQuality()
{
	text_quality_value->SetText(String::FromInt(quality * 5));
}
