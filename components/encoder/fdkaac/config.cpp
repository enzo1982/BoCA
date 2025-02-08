 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2025 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureFDKAAC::ConfigID = "FDKAAC";

BoCA::ConfigureFDKAAC::ConfigureFDKAAC()
{
	const Config	*config = Config::Get();

	Int	 aacType = config->GetIntValue(ConfigID, "AACType", AOT_AAC_LC);

	mpegVersion	= config->GetIntValue(ConfigID, "MPEGVersion", 0);
	mode		= config->GetIntValue(ConfigID, "Mode", 0);
	bitrate		= config->GetIntValue(ConfigID, "Bitrate", 64);
	quality		= config->GetIntValue(ConfigID, "Quality", 4);
	afterburner	= config->GetIntValue(ConfigID, "Afterburner", True);
	bandwidth	= config->GetIntValue(ConfigID, "Bandwidth", 0);
	allowID3	= config->GetIntValue(ConfigID, "AllowID3v2", False);
	fileFormat	= config->GetIntValue(ConfigID, "MP4Container", True);
	fileExtension	= config->GetIntValue(ConfigID, "MP4FileExtension", 0);

	if (bandwidth) bandwidth = (bandwidth - 7900) / 100;

	previousBandwidth = 0;

	I18n	*i18n = I18n::Get();

	tabwidget		= new TabWidget(Point(7, 7), Size(500, 258));

	i18n->SetContext("Encoders::AAC::Format");

	layer_format		= new Layer(i18n->TranslateString("Format"));

	group_mp4		= new GroupBox(i18n->TranslateString("File format"), Point(7, 11), Size(120, 65));

	option_mp4		= new OptionBox("MP4", Point(10, 13), Size(99, 0), &fileFormat, 1);
	option_mp4->onAction.Connect(&ConfigureFDKAAC::SetFileFormat, this);

	if (mp4v2dll == NIL)
	{
		option_mp4->Deactivate();

		fileFormat = 0;
	}

	option_aac		= new OptionBox("AAC", Point(10, 38), Size(99, 0), &fileFormat, 0);
	option_aac->onAction.Connect(&ConfigureFDKAAC::SetFileFormat, this);

	group_mp4->Add(option_mp4);
	group_mp4->Add(option_aac);

	group_version		= new GroupBox(i18n->TranslateString("MPEG version"), Point(135, 11), Size(120, 65));

	option_version_mpeg2	= new OptionBox("MPEG 2", Point(10, 13), Size(group_version->GetWidth() - 21, 0), &mpegVersion, 127);
	option_version_mpeg2->onAction.Connect(&ConfigureFDKAAC::SetMPEGVersion, this);

	option_version_mpeg4	= new OptionBox("MPEG 4", Point(10, 38), Size(group_version->GetWidth() - 21, 0), &mpegVersion, 0);
	option_version_mpeg4->onAction.Connect(&ConfigureFDKAAC::SetMPEGVersion, this);

	group_version->Add(option_version_mpeg2);
	group_version->Add(option_version_mpeg4);

	group_id3v2		= new GroupBox(i18n->TranslateString("Tags"), Point(7, 88), Size(279, 90));

	check_id3v2		= new CheckBox(i18n->TranslateString("Allow ID3v2 tags in AAC files"), Point(10, 13), Size(200, 0), &allowID3);
	check_id3v2->SetWidth(check_id3v2->GetUnscaledTextWidth() + 20);

	text_note		= new Text(i18n->AddColon(i18n->TranslateString("Note")), Point(10, 38));
	text_id3v2		= new Text(i18n->TranslateString("Some players may have problems playing AAC\nfiles with ID3 tags attached. Please use this option only\nif you are sure that your player can handle these tags."), Point(text_note->GetUnscaledTextWidth() + 12, 38));

	group_id3v2->SetSize(Size(Math::Max(384, text_note->GetUnscaledTextWidth() + text_id3v2->GetUnscaledTextWidth() + 22), Math::Max(text_note->GetUnscaledTextHeight(), text_id3v2->GetUnscaledTextHeight()) + 48));

	group_id3v2->Add(check_id3v2);
	group_id3v2->Add(text_note);
	group_id3v2->Add(text_id3v2);

	group_extension		= new GroupBox(i18n->TranslateString("File extension"), Point(263, 11), Size(group_id3v2->GetWidth() - 256, 65));

	option_extension_m4a	= new OptionBox(".m4a", Point(10, 13),					Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 0);
	option_extension_m4b	= new OptionBox(".m4b", Point(10, 38),					Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 1);
	option_extension_m4r	= new OptionBox(".m4r", Point(group_extension->GetWidth() / 2 + 4, 13), Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 2);
	option_extension_mp4	= new OptionBox(".mp4", Point(group_extension->GetWidth() / 2 + 4, 38), Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 3);

	group_extension->Add(option_extension_m4a);
	group_extension->Add(option_extension_m4b);
	group_extension->Add(option_extension_m4r);
	group_extension->Add(option_extension_mp4);

	i18n->SetContext("Encoders::AAC::Codec");

	layer_quality		= new Layer(i18n->TranslateString("Codec"));

	group_codec		= new GroupBox(i18n->TranslateString("Audio codec"), Point(7, 11), Size(group_id3v2->GetWidth(), 43));

	text_codec		= new Text(i18n->AddColon(i18n->TranslateString("Audio codec")), Point(10, 15));

	combo_codec		= new ComboBox(Point(text_codec->GetUnscaledTextSize().cx + 17, 12), Size(group_codec->GetWidth() - text_codec->GetUnscaledTextSize().cx - 27, 0));

	FillCodecs();

	foreach (Int codec, codecs) if (aacType == codec) combo_codec->SelectNthEntry(foreachindex);

	combo_codec->onSelectEntry.Connect(&ConfigureFDKAAC::SetCodec, this);

	group_codec->Add(text_codec);
	group_codec->Add(combo_codec);

	i18n->SetContext("Encoders::AAC::Quality");

	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate / Quality"), Point(7, 66), Size(group_id3v2->GetWidth(), 103));

	option_bitrate		= new OptionBox(i18n->AddColon(i18n->TranslateString("Bitrate per channel")), Point(10, 13), Size(150, 0), &mode, 0);
	option_bitrate->onAction.Connect(&ConfigureFDKAAC::SetMode, this);
	option_bitrate->SetWidth(option_bitrate->GetUnscaledTextWidth() + 19);

	text_bitrate_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Trim(), Point(35, 15));
	text_bitrate_kbps->SetX(text_bitrate_kbps->GetUnscaledTextWidth() + 10);
	text_bitrate_kbps->SetOrientation(OR_UPPERRIGHT);

	edit_bitrate		= new EditBox(Point(text_bitrate_kbps->GetX() + 32, 12), Size(25, 0), 3);
	edit_bitrate->SetFlags(EDB_NUMERIC);
	edit_bitrate->SetOrientation(OR_UPPERRIGHT);
	edit_bitrate->onInput.Connect(&ConfigureFDKAAC::SetBitrateByEditBox, this);

	slider_bitrate		= new Slider(Point(option_bitrate->GetWidth() + 18, 13), Size(group_bitrate->GetWidth() - edit_bitrate->GetX() - 26 - option_bitrate->GetWidth(), 0), OR_HORZ, &bitrate, 8, 256);
	slider_bitrate->onValueChange.Connect(&ConfigureFDKAAC::SetBitrate, this);

	option_quality		= new OptionBox(i18n->AddColon(i18n->TranslateString("Set quality")), Point(10, 38), Size(150, 0), &mode, 1);
	option_quality->onAction.Connect(&ConfigureFDKAAC::SetMode, this);
	option_quality->SetWidth(option_bitrate->GetWidth());

	slider_quality		= new Slider(Point(option_quality->GetWidth() + 18, 38), Size(slider_bitrate->GetWidth(), 0), OR_HORZ, &quality, 1, 5);
	slider_quality->onValueChange.Connect(&ConfigureFDKAAC::SetQuality, this);

	text_quality_level	= new Text(NIL, Point(edit_bitrate->GetX(), 40));
	text_quality_level->SetOrientation(OR_UPPERRIGHT);

	text_quality_worse	= new Text(i18n->TranslateString("worse"), Point(slider_quality->GetX(), 57));
	text_quality_worse->SetX(text_quality_worse->GetX() - text_quality_worse->GetUnscaledTextWidth() / 2);

	text_quality_better	= new Text(i18n->TranslateString("better"), Point(slider_quality->GetX() + slider_quality->GetWidth(), 57));
	text_quality_better->SetX(text_quality_better->GetX() - text_quality_better->GetUnscaledTextWidth() / 2);

	check_afterburner	= new CheckBox(i18n->TranslateString("Enable Afterburner processing"), Point(10, 76), Size(group_bitrate->GetWidth() - 20, 0), &afterburner);

	group_bitrate->Add(option_bitrate);
	group_bitrate->Add(slider_bitrate);
	group_bitrate->Add(edit_bitrate);
	group_bitrate->Add(text_bitrate_kbps);
	group_bitrate->Add(option_quality);
	group_bitrate->Add(slider_quality);
	group_bitrate->Add(text_quality_level);
	group_bitrate->Add(text_quality_worse);
	group_bitrate->Add(text_quality_better);
	group_bitrate->Add(check_afterburner);

	group_bandwidth		= new GroupBox(i18n->TranslateString("Maximum bandwidth"), Point(7, group_bitrate->GetHeight() + 78), Size(group_id3v2->GetWidth(), 40));

	text_bandwidth		= new Text(i18n->AddColon(i18n->TranslateString("Maximum AAC frequency bandwidth to use (Hz)")), Point(9, 15));

	text_bandwidth_hz	= new Text(i18n->TranslateString("%1 Hz", "Technical").Replace("%1", "20000"), Point(35, 15));
	text_bandwidth_hz->SetX(Math::Max(text_bandwidth_hz->GetUnscaledTextWidth(), text_bandwidth_hz->GetFont().GetUnscaledTextSizeX(i18n->TranslateString("auto"))) + 10);
	text_bandwidth_hz->SetOrientation(OR_UPPERRIGHT);

	slider_bandwidth	= new Slider(Point(text_bandwidth->GetUnscaledTextWidth() + 16, 13), Size(group_bandwidth->GetWidth() - text_bandwidth->GetUnscaledTextWidth() - text_bandwidth_hz->GetX() - 24, 0), OR_HORZ, &bandwidth, 0, 121);
	slider_bandwidth->onValueChange.Connect(&ConfigureFDKAAC::SetBandwidth, this);

	group_bandwidth->Add(text_bandwidth);
	group_bandwidth->Add(slider_bandwidth);
	group_bandwidth->Add(text_bandwidth_hz);

	SetCodec();
	SetFileFormat();
	SetMPEGVersion();
	SetMode();
	SetBitrate();
	SetQuality();
	SetBandwidth();

	tabwidget->SetSize(Size(group_id3v2->GetWidth() + 18, Math::Max(251, group_id3v2->GetHeight() + 118)));

	Add(tabwidget);

	tabwidget->Add(layer_quality);
	tabwidget->Add(layer_format);

	layer_format->Add(group_version);
	layer_format->Add(group_mp4);
	layer_format->Add(group_extension);
	layer_format->Add(group_id3v2);

	layer_quality->Add(group_codec);
	layer_quality->Add(group_bitrate);
	layer_quality->Add(group_bandwidth);

	SetSize(tabwidget->GetSize() + Size(14, 14));
}

BoCA::ConfigureFDKAAC::~ConfigureFDKAAC()
{
	DeleteObject(tabwidget);
	DeleteObject(layer_format);
	DeleteObject(layer_quality);

	DeleteObject(group_version);
	DeleteObject(option_version_mpeg2);
	DeleteObject(option_version_mpeg4);
	DeleteObject(group_mp4);
	DeleteObject(option_mp4);
	DeleteObject(option_aac);
	DeleteObject(group_extension);
	DeleteObject(option_extension_m4a);
	DeleteObject(option_extension_m4b);
	DeleteObject(option_extension_m4r);
	DeleteObject(option_extension_mp4);
	DeleteObject(group_id3v2);
	DeleteObject(check_id3v2);
	DeleteObject(text_note);
	DeleteObject(text_id3v2);

	DeleteObject(group_codec);
	DeleteObject(text_codec);
	DeleteObject(combo_codec);
	DeleteObject(group_bitrate);
	DeleteObject(option_bitrate);
	DeleteObject(slider_bitrate);
	DeleteObject(edit_bitrate);
	DeleteObject(text_bitrate_kbps);
	DeleteObject(option_quality);
	DeleteObject(slider_quality);
	DeleteObject(text_quality_level);
	DeleteObject(text_quality_worse);
	DeleteObject(text_quality_better);
	DeleteObject(check_afterburner);

	DeleteObject(group_bandwidth);
	DeleteObject(text_bandwidth);
	DeleteObject(slider_bandwidth);
	DeleteObject(text_bandwidth_hz);
}

Int BoCA::ConfigureFDKAAC::SaveSettings()
{
	Config	*config = Config::Get();

	if (bitrate <	8) bitrate =   8;
	if (bitrate > 256) bitrate = 256;

	Int	 aacType = codecs.GetNth(combo_codec->GetSelectedEntryNumber());

	if (aacType != AOT_AAC_LC &&
	    aacType != AOT_ER_AAC_LD) bandwidth = previousBandwidth;

	if (bandwidth) bandwidth = 7900 + 100 * bandwidth;

	config->SetIntValue(ConfigID, "MPEGVersion", mpegVersion);
	config->SetIntValue(ConfigID, "AACType", aacType);
	config->SetIntValue(ConfigID, "Mode", mode);
	config->SetIntValue(ConfigID, "Bitrate", bitrate);
	config->SetIntValue(ConfigID, "Quality", quality);
	config->SetIntValue(ConfigID, "Afterburner", afterburner);
	config->SetIntValue(ConfigID, "Bandwidth", bandwidth);
	config->SetIntValue(ConfigID, "AllowID3v2", allowID3);
	config->SetIntValue(ConfigID, "MP4Container", fileFormat);
	config->SetIntValue(ConfigID, "MP4FileExtension", fileExtension);

	return Success();
}

Void BoCA::ConfigureFDKAAC::FillCodecs()
{
	LIB_INFO	 info[FDK_MODULE_LAST];

	FDKinitLibInfo(info);
	ex_aacEncGetLibInfo(info);

	UINT	 sbrFlags = FDKlibInfo_getCapabilities(info, FDK_SBRENC);

	codecs.RemoveAll();
	combo_codec->RemoveAllEntries();

					  { combo_codec->AddEntry("MPEG AAC Low Complexity");	  codecs.Add(AOT_AAC_LC);     }

	if (sbrFlags			) { combo_codec->AddEntry("MPEG AAC High Efficiency");	  codecs.Add(AOT_SBR);	      }
	if (sbrFlags & CAPF_SBR_PS_MPEG	) { combo_codec->AddEntry("MPEG AAC High Efficiency v2"); codecs.Add(AOT_PS);	      }

	if (mpegVersion == 0		) { combo_codec->AddEntry("MPEG AAC Low Delay");	  codecs.Add(AOT_ER_AAC_LD);  }
	if (mpegVersion == 0 && sbrFlags) { combo_codec->AddEntry("MPEG AAC Enhanced Low Delay"); codecs.Add(AOT_ER_AAC_ELD); }
}

Void BoCA::ConfigureFDKAAC::SetMPEGVersion()
{
	Int	 aacType = codecs.GetNth(combo_codec->GetSelectedEntryNumber());

	FillCodecs();

	if (mpegVersion == 127 && (aacType == AOT_ER_AAC_LD || aacType == AOT_ER_AAC_ELD)) aacType = AOT_AAC_LC;

	foreach (Int codec, codecs) if (aacType == codec) combo_codec->SelectNthEntry(foreachindex);
}

Void BoCA::ConfigureFDKAAC::SetCodec()
{
	Int	 aacType = codecs.GetNth(combo_codec->GetSelectedEntryNumber());

	if (aacType == AOT_ER_AAC_LD ||
	    aacType == AOT_ER_AAC_ELD) option_version_mpeg2->Deactivate();
	else			       option_version_mpeg2->Activate();

	/* Toggle bandwidth controls.
	 */
	if (aacType == AOT_AAC_LC || aacType == AOT_ER_AAC_LD)
	{
		if (!group_bandwidth->IsActive()) bandwidth = previousBandwidth;

		group_bandwidth->Activate();
	}
	else
	{
		if (group_bandwidth->IsActive()) previousBandwidth = bandwidth;

		group_bandwidth->Deactivate();

		bandwidth = 0;
	}

	SetQuality();
	SetBandwidth();

	/* Set bitrate range for AAC type.
	 */
	switch (aacType)
	{
		case AOT_AAC_LC:
			slider_bitrate->SetRange(8, 256);
			slider_quality->SetRange(3, 5);

			break;
		case AOT_SBR:
			slider_bitrate->SetRange(8, 64);
			slider_quality->SetRange(2, 5);

			break;
		case AOT_PS:
			slider_bitrate->SetRange(8, 32);
			slider_quality->SetRange(1, 5);

			break;
		case AOT_ER_AAC_LD:
			slider_bitrate->SetRange(8, 256);
			slider_quality->SetRange(3, 5);

			break;
		case AOT_ER_AAC_ELD:
			slider_bitrate->SetRange(8, 256);
			slider_quality->SetRange(1, 5);

			break;
	}

	SetBitrate();
}

Void BoCA::ConfigureFDKAAC::SetMode()
{
	if (mode == 0)	// bitrate
	{
		slider_quality->Deactivate();
		text_quality_level->Deactivate();
		text_quality_worse->Deactivate();
		text_quality_better->Deactivate();

		slider_bitrate->Activate();
		edit_bitrate->Activate();
		text_bitrate_kbps->Activate();
	}
	else		// quality
	{
		slider_bitrate->Deactivate();
		edit_bitrate->Deactivate();
		text_bitrate_kbps->Deactivate();

		slider_quality->Activate();
		text_quality_level->Activate();
		text_quality_worse->Activate();
		text_quality_better->Activate();
	}
}

Void BoCA::ConfigureFDKAAC::SetBitrate()
{
	if (!edit_bitrate->IsFocussed()) edit_bitrate->SetText(String::FromInt(bitrate));
}

Void BoCA::ConfigureFDKAAC::SetBitrateByEditBox()
{
	slider_bitrate->SetValue(edit_bitrate->GetText().ToInt());
}

Void BoCA::ConfigureFDKAAC::SetQuality()
{
	text_quality_level->SetText(String::FromInt(quality));
}

Void BoCA::ConfigureFDKAAC::SetBandwidth()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::AAC::Quality");

	if (bandwidth) text_bandwidth_hz->SetText(i18n->TranslateString("%1 Hz", "Technical").Replace("%1", String::FromInt(7900 + 100 * bandwidth)));
	else	       text_bandwidth_hz->SetText(i18n->TranslateString("auto"));
}

Void BoCA::ConfigureFDKAAC::SetFileFormat()
{
	if (fileFormat == 1)	// MP4 container
	{
		group_version->Deactivate();
		group_id3v2->Deactivate();

		group_extension->Activate();

		if (mpegVersion == 127) // MPEG2
		{
			mpegVersion = 0;

			SetMPEGVersion();
		}
	}
	else			// raw AAC file format
	{
		group_version->Activate();
		group_id3v2->Activate();

		group_extension->Deactivate();
	}
}
