 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

	track		= 0;
	numTracks	= 0;

	disc		= 0;
	numDiscs	= 0;

	rating		= -1;
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
	if (key == NIL) return False;

	/* Find the first entry with this key.
	 */
	Int	 keyIndex = -1;

	for (Int i = 0; i < other.Length(); i++)
	{
		const String	&info	   = other.GetNth(i);
		Int		 separator = info.Find(":");

		if (info.Head(separator) == key) { keyIndex = i; break; }
	}

	/* Remove every later entry with this key.
	 */
	for (Int i = other.Length() - 1; i > keyIndex; i--)
	{
		const String	&info	   = other.GetNth(i);
		Int		 separator = info.Find(":");

		if (info.Head(separator) == key) other.RemoveNth(i);
	}

	/* Replace the first entry in place, remove it, or append a new one.
	 */
	if	(keyIndex >= 0 && value != NIL) other.SetNth(keyIndex, String(key).Append(":").Append(value));
	else if (keyIndex >= 0)			 other.RemoveNth(keyIndex);
	else if (value != NIL)			 other.Add(String(key).Append(":").Append(value));

	return True;
}

Array<String> BoCA::Info::GetOtherInfoList(const Info *info, const String &key)
{
	Array<String>	 values;

	foreach (const String &entry, info->other)
	{
		Int	 separator = entry.Find(":");

		if (entry.Head(separator) == key) values.Add(entry.Tail(entry.Length() - separator - 1));
	}

	return values;
}

Array<String> BoCA::Info::GetOtherInfoKeys(const Info *info)
{
	Array<String>	 keys;

	foreach (const String &entry, info->other)
	{
		String	 key   = entry.Head(entry.Find(":"));
		Bool	 found = False;

		foreach (const String &listed, keys) if (listed == key) { found = True; break; }

		if (!found) keys.Add(key);
	}

	return keys;
}

Bool BoCA::Info::AddOtherInfo(Info *info, const String &key, const String &value)
{
	if (key == NIL || value == NIL) return False;

	/* Do not add a value that this key already has.
	 */
	foreach (const String &entry, info->other)
	{
		Int	 separator = entry.Find(":");

		if (entry.Head(separator) == key && entry.Tail(entry.Length() - separator - 1) == value) return False;
	}

	info->other.Add(String(key).Append(":").Append(value));

	return True;
}

Bool BoCA::Info::AddOtherInfo(Info *info, const String &key, const Array<String> &values)
{
	Bool	 added = False;

	foreach (const String &value, values) if (AddOtherInfo(info, key, value)) added = True;

	return added;
}

/* The basic field that a value list belongs to.
 */
static String *GetBasicField(BoCA::Info *info, const String &key)
{
	if	(key == BoCA::INFO_ARTISTS) return &info->artist;
	else if (key == BoCA::INFO_GENRES)  return &info->genre;
	else if (key == BoCA::INFO_LABELS)  return &info->label;
	else if (key == BoCA::INFO_ISRCS)   return &info->isrc;

	return NIL;
}

Bool BoCA::Info::IsBasicInfoList(const String &key)
{
	return key == INFO_ARTISTS || key == INFO_GENRES || key == INFO_LABELS || key == INFO_ISRCS;
}

static String GetOtherInfoGroup(const String &entry)
{
	Int	 separator = entry.Find(":");
	String	 key	   = entry.Head(separator);

	if (key != BoCA::INFO_USERTEXT && key != BoCA::INFO_WEB_USERURL) return key;

	String	 value	   = entry.Tail(entry.Length() - separator - 1);
	Int	 description = value.Find(":|:");

	return String(key).Append(":").Append(description >= 0 ? value.Head(description) : value);
}

Void BoCA::Info::AddMissingOtherInfo(Info *info, const Array<String> &entries)
{
	/* User defined texts and URLs hold one field per description, so
	 * they are compared by key and description.
	 */
	Array<String>	 listed;

	foreach (const String &entry, info->other) listed.Add(GetOtherInfoGroup(entry));

	/* A value list of a basic field from an earlier tag stays only if
	 * this tag left the field at its first value. Otherwise the field
	 * now holds a value of this tag, and the old list does not fit it.
	 */
	Array<String>	 dropped;
	Array<String>	 keys = GetOtherInfoKeys(info);

	foreach (const String &key, keys) if (IsBasicInfoList(key)) dropped.Add(key);

	foreach (const String &entry, entries)
	{
		String	 group = GetOtherInfoGroup(entry);
		Bool	 found = False;

		foreach (const String &name, listed) if (name == group) { found = True; break; }

		if (found) continue;

		if (IsBasicInfoList(group))
		{
			Bool	 skip = False;

			foreach (const String &name, dropped) if (name == group) { skip = True; break; }

			if (skip) continue;

			/* Check the first value of the earlier list once.
			 */
			Array<String>	 values;

			foreach (const String &previous, entries)
			{
				Int	 separator = previous.Find(":");

				if (previous.Head(separator) == group) values.Add(previous.Tail(previous.Length() - separator - 1));
			}

			dropped.Add(group);

			if (values.Length() == 0 || values.GetFirst() != *GetBasicField(info, group)) continue;

			AddOtherInfo(info, group, values);

			continue;
		}

		info->other.Add(entry);
	}
}

Array<String> BoCA::Info::GetInfoList(const Info *info, const String &key)
{
	if (!IsBasicInfoList(key)) return GetOtherInfoList(info, key);

	Array<String>	 values;
	const String	&field = *GetBasicField(const_cast<Info *>(info), key);

	if (field == NIL) return values;

	Array<String>	 list = GetOtherInfoList(info, key);

	if (list.Length() > 0 && list.GetFirst() == field) return list;

	values.Add(field);

	return values;
}

Bool BoCA::Info::SetInfoList(Info *info, const String &key, const Array<String> &values)
{
	if (key == NIL) return False;

	/* Drop empty values and values that come twice.
	 */
	Array<String>	 list;

	foreach (const String &value, values)
	{
		if (value == NIL) continue;

		Bool	 found = False;

		foreach (const String &listed, list) if (listed == value) { found = True; break; }

		if (!found) list.Add(value);
	}

	info->SetOtherInfo(key, NIL);

	if (IsBasicInfoList(key))
	{
		*GetBasicField(info, key) = (list.Length() > 0 ? list.GetFirst() : String());

		if (list.Length() <= 1) return True;
	}

	AddOtherInfo(info, key, list);

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
