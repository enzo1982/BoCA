 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2026 Shen-Ta Hsieh
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/main.h>

#include <boca.h>

#include "testcheck.h"

using namespace smooth;

using namespace BoCA;

/* This tests the DiscMetadata type and the metadata component type.
 */

/* All values of a key in the other list of an Info.
 */
static Array<String> OtherValues(const Info &info, const String &key)
{
	Array<String>	 values;

	foreach (const String &pair, info.other)
	{
		if (pair.StartsWith(String(key).Append(":"))) values.Add(pair.Tail(pair.Length() - key.Length() - 1));
	}

	return values;
}

static DiscMetadata SampleRecord()
{
	DiscMetadata	 record;

	record.provider = "fake-metadata";
	record.recordID = "rock/12345678";
	record.revision = 3;

	record.disc.album  = "Album";
	record.disc.offsets = "2+96+48D5+9402";
	record.disc.other.Add(String(INFO_MUSICBRAINZ_ALBUMARTISTID).Append(":a"));
	record.disc.other.Add(String(INFO_MUSICBRAINZ_ALBUMARTISTID).Append(":b"));

	Info	 track;

	track.track = 1;
	track.title = "Title 1";

	record.tracks.Add(track);

	track.track = 2;
	track.title = "Title 2";

	record.tracks.Add(track);

	record.extensions.Add("PLAYORDER=1,2");

	return record;
}

static Void CheckSame(const DiscMetadata &record, const String &context)
{
	Check(record.provider == "fake-metadata", String(context).Append(": provider"));
	Check(record.recordID == "rock/12345678", String(context).Append(": recordID"));
	Check(record.revision == 3, String(context).Append(": revision"));
	Check(record.disc.album == "Album", String(context).Append(": disc.album"));
	Check(record.disc.offsets == "2+96+48D5+9402", String(context).Append(": disc.offsets"));
	Check(Join(OtherValues(record.disc, INFO_MUSICBRAINZ_ALBUMARTISTID)) == "a|b", String(context).Append(": album artist IDs"));
	Check(record.tracks.Length() == 2, String(context).Append(": number of tracks"));
	Check(record.tracks.GetNth(0).title == "Title 1" && record.tracks.GetNth(0).track == 1, String(context).Append(": track 1"));
	Check(record.tracks.GetNth(1).title == "Title 2" && record.tracks.GetNth(1).track == 2, String(context).Append(": track 2"));
	Check(record.extensions.Length() == 1 && record.extensions.GetFirst() == "PLAYORDER=1,2", String(context).Append(": extensions"));
}

static Void CheckCopy()
{
	DiscMetadata	 record = SampleRecord();
	DiscMetadata	 copy(record);
	DiscMetadata	 assigned;

	assigned.provider = "other";
	assigned.tracks.Add(Info());
	assigned.tracks.Add(Info());
	assigned.tracks.Add(Info());
	assigned.extensions.Add("OLD=1");

	assigned = record;

	CheckSame(copy, "copy");
	CheckSame(assigned, "assignment");

	DiscMetadata	 empty;

	Check(empty.revision == -1, "a new record has revision -1");

	MetadataMatch	 match;

	Check(match.fuzzy == False, "a new match is not fuzzy");
}

static String GetPluginFile(const String &name)
{
#if defined __WIN32__
	return GUI::Application::GetApplicationDirectory().Append(name).Append(".dll");
#elif defined __APPLE__
	return GUI::Application::GetApplicationDirectory().Append(name).Append(".dylib");
#else
	return GUI::Application::GetApplicationDirectory().Append(name).Append(".so");
#endif
}

static Void CheckFakeComponent()
{
	AS::ComponentSpecs	*specs = new AS::ComponentSpecs();

	Check(specs->LoadFromDLL(GetPluginFile("fakemetadata")), "fake component loads");
	Check(specs->type == COMPONENT_TYPE_METADATA, "fake component has the metadata type");
	Check(specs->id == "fake-metadata", "fake component ID");

	if (specs->type != COMPONENT_TYPE_METADATA) { delete specs; return; }

	AS::MetadataComponent	*component = new AS::MetadataComponent(specs);

	Check(component->GetQueryKeys() == (METADATA_QUERY_KEY_TOC | METADATA_QUERY_KEY_ID), "GetQueryKeys");

	/* Query and read.
	 */
	DiscMetadata		 known;
	Array<MetadataMatch>	 matches;

	known.disc.offsets = "2+96+48D5+9402";

	Check(component->Query(known, matches) == METADATA_QUERY_RESULT_MULTIPLE, "Query result");
	Check(matches.Length() == 2, "Query returns two matches");

	if (matches.Length() == 2)
	{
		Check(matches.GetNth(0).recordID == "fake/1" && matches.GetNth(0).title == "First for 2+96+48D5+9402" && !matches.GetNth(0).fuzzy, "first match");
		Check(matches.GetNth(1).recordID == "fake/2" && matches.GetNth(1).title == "Second" && matches.GetNth(1).fuzzy, "second match");
	}

	DiscMetadata	 record;

	Check(component->Read("fake/1", record), "Read returns True");
	Check(record.provider == "fake-metadata" && record.recordID == "fake/1" && record.revision == 7, "Read record identity");
	Check(record.disc.album == "Fake album", "Read disc album");
	Check(Join(OtherValues(record.disc, INFO_MUSICBRAINZ_ALBUMARTISTID)) == "id1|id2", "Read keeps several values");
	Check(record.tracks.Length() == 1 && record.tracks.GetFirst().title == "Fake title", "Read tracks");
	Check(record.extensions.Length() == 1 && record.extensions.GetFirst() == "KEY=value", "Read extensions");
	Check(!component->Read("fake/2", record), "Read returns False");

	/* User IDs and submits.
	 */
	Check(component->NormalizeRecordID("fake:abc") == "abc", "NormalizeRecordID accepts");
	Check(component->NormalizeRecordID("other") == NIL, "NormalizeRecordID rejects");

	Check(component->GetSubmitData() == METADATA_SUBMIT_DATA_TOC, "GetSubmitData");
	Check(component->GetSubmitMethod() == METADATA_SUBMIT_BY_USER, "GetSubmitMethod");

	record.recordID = "fake/1";

	Check(component->Submit(record), "Submit");

	/* Pictures use the defaults.
	 */
	Array<Picture>	 pictures;

	Check(!component->CanReadPictures(), "fake CanReadPictures default");
	Check(!component->ReadPictures("fake/1", pictures) && pictures.Length() == 0, "fake ReadPictures default");

	delete component;
	delete specs;
}

static Void CheckMinimalComponent()
{
	AS::ComponentSpecs	*specs = new AS::ComponentSpecs();

	Check(specs->LoadFromDLL(GetPluginFile("minimalmetadata")), "minimal component loads");
	Check(specs->type == COMPONENT_TYPE_METADATA, "minimal component has the metadata type");

	if (specs->type != COMPONENT_TYPE_METADATA) { delete specs; return; }

	AS::MetadataComponent	*component = new AS::MetadataComponent(specs);
	DiscMetadata		 record;
	Array<Picture>		 pictures;

	Check(component->GetQueryKeys() == METADATA_QUERY_KEY_TEXT, "minimal GetQueryKeys");
	Check(component->NormalizeRecordID("anything") == NIL, "default NormalizeRecordID");
	Check(component->GetSubmitData() == METADATA_SUBMIT_DATA_NONE, "default GetSubmitData");
	Check(component->GetSubmitMethod() == METADATA_SUBMIT_BY_PROVIDER, "default GetSubmitMethod");
	Check(!component->Submit(record), "default Submit");
	Check(!component->CanReadPictures(), "default CanReadPictures");
	Check(!component->ReadPictures("x", pictures) && pictures.Length() == 0, "default ReadPictures");

	delete component;
	delete specs;
}

Int smooth::Main()
{
	CheckCopy();
	CheckFakeComponent();
	CheckMinimalComponent();

	return CheckSummary();
}
