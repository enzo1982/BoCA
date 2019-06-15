 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/info.h>

BoCA::Info::Info() : mcdi(Buffer<UnsignedByte>())
{
	year		= -1;

	track		= -1;
	numTracks	= -1;

	disc		= -1;
	numDiscs	= -1;

	rating		= 0;
}

BoCA::Info::Info(const Info &oInfo) : mcdi(Buffer<UnsignedByte>())
{
	*this = oInfo;
}

BoCA::Info::~Info()
{
}

BoCA::Info &BoCA::Info::operator =(const Info &oInfo)
{
	if (&oInfo == this) return *this;

	artist		= oInfo.artist;
	title		= oInfo.title;
	album		= oInfo.album;
	genre		= oInfo.genre;
	year		= oInfo.year;
	comment		= oInfo.comment;

	track		= oInfo.track;
	numTracks	= oInfo.numTracks;

	disc		= oInfo.disc;
	numDiscs	= oInfo.numDiscs;

	rating		= oInfo.rating;

	label		= oInfo.label;
	isrc		= oInfo.isrc;

	track_gain	= oInfo.track_gain;
	track_peak	= oInfo.track_peak;
	album_gain	= oInfo.album_gain;
	album_peak	= oInfo.album_peak;

	mcdi		= oInfo.mcdi;
	offsets		= oInfo.offsets;

	other.RemoveAll();

	foreach (const String &string, oInfo.other) other.Add(string);

	return *this;
}

Bool BoCA::Info::HasBasicInfo() const
{
	if (artist != NIL || album != NIL || title != NIL || track > 0) return True;
	else								return False;
}

Bool BoCA::Info::HasOtherInfo(const String &key) const
{
	foreach (const String &info, other)
	{
		Int	 separator = info.Find(":");

		if (info.Head(separator) == key) return True;
	}

	return False;
}

String BoCA::Info::GetOtherInfo(const String &key) const
{
	foreach (const String &info, other)
	{
		Int	 separator = info.Find(":");

		if (info.Head(separator) == key) return info.Tail(info.Length() - separator - 1);
	}

	return NIL;
}

Bool BoCA::Info::SetOtherInfo(const String &key, const String &value)
{
	foreach (const String &info, other)
	{
		Int	 separator = info.Find(":");

		if (info.Head(separator) == key)
		{
			if (value != NIL) other.SetNth(foreachindex, String(key).Append(":").Append(value));
			else		  other.RemoveNth(foreachindex);

			return True;
		}
	}

	other.Add(String(key).Append(":").Append(value));

	return True;
}

Bool BoCA::Info::IsISRC(const String &isrc)
{
	/* List of assigned ISO 3166-1 ALPHA-2 codes.
	 */
	static const char	*countryCodes[] = { "AD", "AE", "AF", "AG", "AI", "AL", "AM", "AN", "AO", "AQ", "AR", "AS", "AT", "AU", "AW", "AX", "AZ",
						    "BA", "BB", "BD", "BE", "BF", "BG", "BH", "BI", "BJ", "BL", "BM", "BN", "BO", "BQ", "BR", "BS", "BT", "BU", "BV", "BW", "BY", "BZ",
						    "CA", "CC", "CD", "CF", "CG", "CH", "CI", "CK", "CL", "CM", "CN", "CO", "CR", "CS", "CU", "CV", "CW", "CX", "CY", "CZ",
						    "DE", "DJ", "DK", "DM", "DO", "DZ",
						    "EC", "EE", "EG", "EH", "ER", "ES", "ET",
						    "FI", "FJ", "FK", "FM", "FO", "FR",
						    "GA", "GB", "GD", "GE", "GF", "GG", "GH", "GI", "GL", "GM", "GN", "GP", "GQ", "GR", "GS", "GT", "GU", "GW", "GY",
						    "HK", "HM", "HN", "HR", "HT", "HU",
						    "ID", "IE", "IL", "IM", "IN", "IO", "IQ", "IR", "IS", "IT",
						    "JE", "JM", "JO", "JP",
						    "KE", "KG", "KH", "KI", "KM", "KN", "KP", "KR", "KW", "KY", "KZ",
						    "LA", "LB", "LC", "LI", "LK", "LR", "LS", "LT", "LU", "LV", "LY",
						    "MA", "MC", "MD", "ME", "MF", "MG", "MH", "MK", "ML", "MM", "MN", "MO", "MP", "MQ", "MR", "MS", "MT", "MU", "MV", "MW", "MX", "MY", "MZ",
						    "NA", "NC", "NE", "NF", "NG", "NI", "NL", "NO", "NP", "NR", "NT", "NU", "NZ",
						    "OM",
						    "PA", "PE", "PF", "PG", "PH", "PK", "PL", "PM", "PN", "PR", "PS", "PT", "PW", "PY",
						    "QA", "QM",
						    "RE", "RO", "RS", "RU", "RW",
						    "SA", "SB", "SC", "SD", "SE", "SG", "SH", "SI", "SJ", "SK", "SL", "SM", "SN", "SO", "SR", "SS", "ST", "SU", "SV", "SX", "SY", "SZ",
						    "TC", "TD", "TF", "TG", "TH", "TJ", "TK", "TL", "TM", "TN", "TO", "TP", "TR", "TT", "TV", "TW", "TZ",
						    "UA", "UG", "UK", "UM", "US", "UY", "UZ",
						    "VA", "VC", "VE", "VG", "VI", "VN", "VU",
						    "WF", "WS",
						    "YE", "YT", "YU",
						    "ZA", "ZM", "ZR", "ZW", "ZZ", NIL };

	if (isrc.Length() != 12) return False;

	/* First two letters make an ISO 3166-1 ALPHA-2 code.
	 */
	Bool	 found = False;

	for (Int i = 0; countryCodes[i] != NIL; i++)
	{
		if (isrc.Head(2) == countryCodes[i]) found = True;
	}

	if (!found) return False;

	/* Next three positions make an alphanumerical label code.
	 */
	for (Int i = 2; i < 5; i++)
	{
		if (isrc[i] < '0' || (isrc[i] > '9' && isrc[i] < 'A') || isrc[i] > 'Z') return False;
	}

	/* Next two digits represent year of release.
	 */
	for (Int i = 5; i < 7; i++)
	{
		if (isrc[i] < '0' || isrc[i] > '9') return False;
	}

	/* Last five digits identify individual record.
	 */
	for (Int i = 7; i < 12; i++)
	{
		if (isrc[i] < '0' || isrc[i] > '9') return False;
	}

	return True;
}
