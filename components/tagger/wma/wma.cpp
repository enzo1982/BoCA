 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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
#include <smooth/dll.h>

#include "wma.h"

const String &BoCA::TaggerWMA::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (wmvcoredll != NIL)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>Windows Media Tagger</name>			\
		    <version>1.0</version>				\
		    <id>wma-tag</id>					\
		    <type>tagger</type>					\
		    <format>						\
		      <name>Windows Media Audio Files</name>		\
		      <extension>wma</extension>			\
		    </format>						\
		    <tagspec>						\
		      <name>WMA Metadata</name>				\
		      <coverart supported=\"true\"/>			\
		      <prependzero allowed=\"true\" default=\"false\"/>	\
		      <encodings>					\
			<encoding>UTF-16LE</encoding>			\
		      </encodings>					\
		    </tagspec>						\
		  </component>						\
									\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWMVCoreDLL();

	/* Register initialization and cleanup handlers.
	 */
	if (wmvcoredll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Connect(&BoCA::TaggerWMA::Initialize);
		engine->onCleanup.Connect(&BoCA::TaggerWMA::Cleanup);
	}
}

Void smooth::DetachDLL()
{
	/* Unregister initialization and cleanup handlers.
	 */
	if (wmvcoredll != NIL)
	{
		BoCA::Engine	*engine = BoCA::Engine::Get();

		engine->onInitialize.Disconnect(&BoCA::TaggerWMA::Initialize);
		engine->onCleanup.Disconnect(&BoCA::TaggerWMA::Cleanup);
	}

	FreeWMVCoreDLL();
}

static String toUNCPath(const String &streamURI)
{
	if (streamURI.StartsWith("\\\\")) return streamURI;

	static const String	 uncPrefix = "\\\\?\\";

	return uncPrefix.Append(File(streamURI));
}

Void BoCA::TaggerWMA::Initialize()
{
	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

Void BoCA::TaggerWMA::Cleanup()
{
	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

const String	 BoCA::TaggerWMA::ConfigID = "Tags";

BoCA::TaggerWMA::TaggerWMA()
{
}

BoCA::TaggerWMA::~TaggerWMA()
{
}

Error BoCA::TaggerWMA::RenderStreamInfo(const String &fileName, const Track &track)
{
	/* Get configuration.
	 */ 
	const Config	*currentConfig = GetConfiguration();

	Bool		 prependZero		 = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroWMAMetadata", False);

	Bool		 writeChapters		 = currentConfig->GetIntValue(ConfigID, "WriteChapters", True);
	Bool		 writeMCDI		 = currentConfig->GetIntValue(ConfigID, "WriteMCDI", True);

	Bool		 coverArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 coverArtWriteToWMA	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToWMAMetadata", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Create metadata editor objects.
	 */
	IWMMetadataEditor	*metadataEditor	 = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	HRESULT	 hr = ex_WMCreateEditor(&metadataEditor);

	if (FAILED(hr)) return Error();

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	if (FAILED(hr))
	{
		metadataEditor->Release();

		return Error();
	}

	/* Open file and render tags.
	 */
	hr = metadataEditor2->OpenEx(toUNCPath(fileName), GENERIC_READ | GENERIC_WRITE, 0);

	if (!FAILED(hr))
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		/* Save basic information.
		 */
		const Info	&info = track.GetInfo();

		if (info.artist != NIL) RenderWMAStringItem(g_wszWMAuthor,     info.artist,		   pHeaderInfo);
		if (info.title  != NIL) RenderWMAStringItem(g_wszWMTitle,      info.title,		   pHeaderInfo);
		if (info.album  != NIL) RenderWMAStringItem(g_wszWMAlbumTitle, info.album,		   pHeaderInfo);
		if (info.year    >   0) RenderWMAStringItem(g_wszWMYear,       String::FromInt(info.year), pHeaderInfo);
		if (info.genre  != NIL) RenderWMAStringItem(g_wszWMGenre,      info.genre,		   pHeaderInfo);
		if (info.label  != NIL) RenderWMAStringItem(g_wszWMPublisher,  info.label,		   pHeaderInfo);
		if (info.isrc   != NIL) RenderWMAStringItem(g_wszWMISRC,       info.isrc,		   pHeaderInfo);

		if (info.track > 0)
		{
			RenderWMAStringItem(g_wszWMTrackNumber, String(prependZero && info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track)), pHeaderInfo);
		}

		if (info.disc > 0)
		{
			String	 discString = String(prependZero && info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc));

			if (info.numDiscs > 0) discString.Append("/").Append(prependZero && info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs));

			RenderWMAStringItem(g_wszWMPartOfSet, discString, pHeaderInfo);
		}

		if (info.rating > 0)
		{
			RenderWMAIntegerItem(g_wszWMSharedUserRating, Math::Min(99, info.rating), pHeaderInfo);
		}

		if	(info.comment != NIL && !replaceExistingComments) RenderWMAStringItem(g_wszWMDescription, info.comment, pHeaderInfo);
		else if (defaultComment != NIL)				  RenderWMAStringItem(g_wszWMDescription, defaultComment, pHeaderInfo);

		/* Save other text info.
		 */
		foreach (const String &pair, info.other)
		{
			String	 key   = pair.Head(pair.Find(":"));
			String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

			if (value == NIL) continue;

			if	(key == INFO_ALBUMARTIST)    RenderWMAStringItem(g_wszWMAlbumArtist,		 value, pHeaderInfo);

			else if	(key == INFO_CONTENTGROUP)   RenderWMAStringItem(g_wszWMContentGroupDescription, value, pHeaderInfo);
			else if	(key == INFO_SUBTITLE)	     RenderWMAStringItem(g_wszWMSubTitle,		 value, pHeaderInfo);

			else if	(key == INFO_CONDUCTOR)	     RenderWMAStringItem(g_wszWMConductor,		 value, pHeaderInfo);
			else if	(key == INFO_REMIXER)	     RenderWMAStringItem(g_wszWMModifiedBy,		 value, pHeaderInfo);
			else if	(key == INFO_COMPOSER)	     RenderWMAStringItem(g_wszWMComposer,		 value, pHeaderInfo);
			else if	(key == INFO_LYRICIST)	     RenderWMAStringItem(g_wszWMWriter,			 value, pHeaderInfo);
			else if	(key == INFO_PRODUCER)	     RenderWMAStringItem(g_wszWMProducer,		 value, pHeaderInfo);

			else if	(key == INFO_ORIG_ARTIST)    RenderWMAStringItem(g_wszWMOriginalArtist,		 value, pHeaderInfo);
			else if	(key == INFO_ORIG_ALBUM)     RenderWMAStringItem(g_wszWMOriginalAlbumTitle,	 value, pHeaderInfo);
			else if	(key == INFO_ORIG_LYRICIST)  RenderWMAStringItem(g_wszWMOriginalLyricist,	 value, pHeaderInfo);
			else if	(key == INFO_ORIG_YEAR)	     RenderWMAStringItem(g_wszWMOriginalReleaseYear,	 value, pHeaderInfo);

			else if	(key == INFO_BPM)	     RenderWMAStringItem(g_wszWMBeatsPerMinute,		 value, pHeaderInfo);
			else if	(key == INFO_INITIALKEY)     RenderWMAStringItem(g_wszWMInitialKey,		 value, pHeaderInfo);

			else if	(key == INFO_COPYRIGHT)	     RenderWMAStringItem(g_wszWMCopyright,		 value, pHeaderInfo);

			else if	(key == INFO_RADIOSTATION)   RenderWMAStringItem(g_wszWMRadioStationName,	 value, pHeaderInfo);
			else if	(key == INFO_RADIOOWNER)     RenderWMAStringItem(g_wszWMRadioStationOwner,	 value, pHeaderInfo);

			else if	(key == INFO_WEB_ARTIST)     RenderWMAStringItem(g_wszWMAuthorURL,		 value, pHeaderInfo);
			else if	(key == INFO_WEB_SOURCE)     RenderWMAStringItem(g_wszWMAudioSourceURL,		 value, pHeaderInfo);
			else if	(key == INFO_WEB_COPYRIGHT)  RenderWMAStringItem(g_wszWMCopyrightURL,		 value, pHeaderInfo);
			else if	(key == INFO_WEB_COMMERCIAL) RenderWMAStringItem(g_wszWMPromotionURL,		 value, pHeaderInfo);
		}

		/* Save CD table of contents.
		 */
		if (writeMCDI)
		{
			if (info.mcdi.GetData().Size() > 0)
			{
				RenderWMABinaryItem(g_wszWMMCDI, info.mcdi.GetData(), pHeaderInfo);
			}
		}

		/* Save encoder version.
		 */
		Application	*app = Application::Get();

		RenderWMAStringItem(g_wszWMToolName, app->getClientName.Call(), pHeaderInfo);
		RenderWMAStringItem(g_wszWMToolVersion, app->getClientVersion.Call(), pHeaderInfo);

		/* Save cover art.
		 */
		if (coverArtWriteToTags && coverArtWriteToWMA)
		{
			foreach (const Picture &picInfo, track.pictures)
			{
				WM_PICTURE	 picture = { 0 };

				picture.bPictureType	   = picInfo.type;
				picture.pwszMIMEType	   = new WCHAR [picInfo.mime.Length() + 1];
				picture.pwszMIMEType[0]	   = 0;
				picture.pwszDescription	   = new WCHAR [picInfo.description.Length() + 1];
				picture.pwszDescription[0] = 0;
				picture.dwDataLen	   = picInfo.data.Size();
				picture.pbData		   = (BYTE *) const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data);

				if (picInfo.mime	!= NIL) wcsncpy(picture.pwszMIMEType, picInfo.mime, picInfo.mime.Length() + 1);
				if (picInfo.description != NIL) wcsncpy(picture.pwszDescription, picInfo.description, picInfo.description.Length() + 1);

				pHeaderInfo->AddAttribute(0, g_wszWMPicture, NIL, WMT_TYPE_BINARY, 0, (BYTE *) &picture, sizeof(WM_PICTURE));

				delete [] picture.pwszMIMEType;
				delete [] picture.pwszDescription;
			}
		}

		/* Save chapters.
		 */
		if (track.tracks.Length() > 0 && writeChapters)
		{
			Int64	 offset = 0;

			for (Int i = 0; i < track.tracks.Length(); i++)
			{
				const Track	&chapterTrack  = track.tracks.GetNth(i);
				const Info	&chapterInfo   = chapterTrack.GetInfo();
				const Format	&chapterFormat = chapterTrack.GetFormat();

				pHeaderInfo->AddMarker(chapterInfo.title, offset * 10000000 / chapterFormat.rate);

				if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
				else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
			}
		}

		pHeaderInfo->Release();

		/* Flush metadata editor.
		 */
		hr = metadataEditor2->Flush();
	}

	metadataEditor->Release();
	metadataEditor2->Release();

	if (FAILED(hr)) return Error();
	else		return Success();
}

Error BoCA::TaggerWMA::RenderWMAStringItem(const String &id, const String &value, Void *headerInfo)
{
	String	 data	  = value.Trim();
	Int	 dataSize = data != NIL ? wcslen(data) + 1 : 0;

	if (data == NIL) return Success();

	HRESULT	 hr = ((IWMHeaderInfo3 *) headerInfo)->AddAttribute(0, id, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) data, dataSize * 2);

	if (FAILED(hr)) return Error();
	else		return Success();
}

Error BoCA::TaggerWMA::RenderWMAIntegerItem(const String &id, Int value, Void *headerInfo)
{
	HRESULT	 hr = ((IWMHeaderInfo3 *) headerInfo)->AddAttribute(0, id, NIL, WMT_TYPE_DWORD, 0, (BYTE *) &value, sizeof(DWORD));

	if (FAILED(hr)) return Error();
	else		return Success();
}

Error BoCA::TaggerWMA::RenderWMABinaryItem(const String &id, const Buffer<UnsignedByte> &value, Void *headerInfo)
{
	HRESULT	 hr = ((IWMHeaderInfo3 *) headerInfo)->AddAttribute(0, id, NIL, WMT_TYPE_BINARY, 0, (BYTE *) (UnsignedByte *) value, value.Size());

	if (FAILED(hr)) return Error();
	else		return Success();
}

Error BoCA::TaggerWMA::ParseStreamInfo(const String &fileName, Track &track)
{
	const Config	*config = GetConfiguration();

	/* Create metadata editor objects.
	 */
	IWMMetadataEditor	*metadataEditor	 = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	HRESULT	 hr = ex_WMCreateEditor(&metadataEditor);

	if (FAILED(hr)) return Error();

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	if (FAILED(hr))
	{
		metadataEditor->Release();

		return Error();
	}

	/* Open file and parse tags.
	 */
	hr = metadataEditor2->OpenEx(toUNCPath(fileName), GENERIC_READ, FILE_SHARE_READ);

	if (!FAILED(hr))
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		Info	 info = track.GetInfo();

		WORD	 langIndex = 0;
		WORD	 numIndices = 0;

		pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, NIL, &numIndices);

		WORD	*indices = new WORD [numIndices];

		pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, indices, &numIndices);

		for (Int i = 0; i < numIndices; i++)
		{
			/* Read metadata values.
			 */
			WORD			 nameLen  = 1024;
			LPWSTR			 name	  = new WCHAR [nameLen];
			DWORD			 cbLength = 0;

			pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, NIL, NIL, NIL, &cbLength);

			WMT_ATTR_DATATYPE	 type	  = WMT_TYPE_DWORD;
			BYTE			*pbValue  = new BYTE [cbLength];

			pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, &type, NIL, pbValue, &cbLength);

			/* Parse string values.
			 */
			String	 value;

			if (type == WMT_TYPE_STRING)
			{
				value = String((LPWSTR) pbValue).Trim();

				if (value == NIL)
				{
					delete [] pbValue;
					delete [] name;

					continue;
				}
			}

			/* Assign values to metadata fields.
			 */
			if	(String(name) == g_wszWMAuthor)			 info.artist  = value;
			else if (String(name) == g_wszWMTitle)			 info.title   = value;
			else if (String(name) == g_wszWMAlbumTitle)		 info.album   = value;
			else if (String(name) == g_wszWMYear)			 info.year    = value.ToInt();
			else if (String(name) == g_wszWMGenre)			 info.genre   = value;
			else if (String(name) == g_wszWMDescription)		 info.comment = value;
			else if (String(name) == g_wszWMPublisher)		 info.label   = value;
			else if (String(name) == g_wszWMISRC)			 info.isrc    = value;

			else if (String(name) == g_wszWMAlbumArtist)		 info.SetOtherInfo(INFO_ALBUMARTIST,	value);

			else if (String(name) == g_wszWMContentGroupDescription) info.SetOtherInfo(INFO_CONTENTGROUP,	value);
			else if (String(name) == g_wszWMSubTitle)		 info.SetOtherInfo(INFO_SUBTITLE,	value);

			else if (String(name) == g_wszWMConductor)		 info.SetOtherInfo(INFO_CONDUCTOR,	value);
			else if (String(name) == g_wszWMModifiedBy)		 info.SetOtherInfo(INFO_REMIXER,	value);
			else if (String(name) == g_wszWMComposer)		 info.SetOtherInfo(INFO_COMPOSER,	value);
			else if (String(name) == g_wszWMWriter)			 info.SetOtherInfo(INFO_LYRICIST,	value);
			else if (String(name) == g_wszWMProducer)		 info.SetOtherInfo(INFO_PRODUCER,	value);

			else if (String(name) == g_wszWMOriginalArtist)		 info.SetOtherInfo(INFO_ORIG_ARTIST,	value);
			else if (String(name) == g_wszWMOriginalAlbumTitle)	 info.SetOtherInfo(INFO_ORIG_ALBUM,	value);
			else if (String(name) == g_wszWMOriginalLyricist)	 info.SetOtherInfo(INFO_ORIG_LYRICIST,	value);
			else if (String(name) == g_wszWMOriginalReleaseYear)	 info.SetOtherInfo(INFO_ORIG_YEAR,	value);

			else if (String(name) == g_wszWMBeatsPerMinute)		 info.SetOtherInfo(INFO_BPM,		value);
			else if (String(name) == g_wszWMInitialKey)		 info.SetOtherInfo(INFO_INITIALKEY,	value);

			else if (String(name) == g_wszWMCopyright)		 info.SetOtherInfo(INFO_COPYRIGHT,	value);

			else if (String(name) == g_wszWMRadioStationName)	 info.SetOtherInfo(INFO_RADIOSTATION,	value);
			else if (String(name) == g_wszWMRadioStationOwner)	 info.SetOtherInfo(INFO_RADIOOWNER,	value);

			else if (String(name) == g_wszWMAuthorURL)		 info.SetOtherInfo(INFO_WEB_ARTIST,	value);
			else if (String(name) == g_wszWMAudioSourceURL)		 info.SetOtherInfo(INFO_WEB_SOURCE,	value);
			else if (String(name) == g_wszWMCopyrightURL)		 info.SetOtherInfo(INFO_WEB_COPYRIGHT,	value);
			else if (String(name) == g_wszWMPromotionURL)		 info.SetOtherInfo(INFO_WEB_COMMERCIAL,	value);

			else if (String(name) == g_wszWMTrack)
			{
				if	(type == WMT_TYPE_DWORD)  info.track = 1 + ((DWORD *) pbValue)[0];
				else if (type == WMT_TYPE_STRING) info.track = 1 + value.ToInt();
			}
			else if (String(name) == g_wszWMTrackNumber)
			{
				if	(type == WMT_TYPE_DWORD)  info.track = ((DWORD *) pbValue)[0];
				else if (type == WMT_TYPE_STRING)
				{
					info.track = value.ToInt();

					if (value.Contains("/")) info.numTracks = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
				}
			}
			else if (String(name) == g_wszWMPartOfSet)
			{
				info.disc = value.ToInt();

				if (value.Contains("/")) info.numDiscs = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
			}
			else if (String(name) == g_wszWMSharedUserRating)
			{
				info.rating = (DWORD) *pbValue;

				if (info.rating == 99) info.rating = 100;
			}
			else if (String(name) == g_wszWMMCDI)
			{
				/* Use a heuristic to detect if this is a valid binary MCDI
				 * field or the commonly used track offset string.
				 */
				Bool	 binary = False;

				for (Int i = 0; unsigned(i) < cbLength; i++)
				{
					if (pbValue[i] > 0 && pbValue[i] < 0x20) { binary = True; break; }
				}

				if (binary)
				{
					Buffer<UnsignedByte>	 data(cbLength);

					memcpy(data, pbValue, cbLength);

					/* Check validity of MCDI data.
					 */
					MCDI	 mcdi(data);
					Bool	 valid = True;

					for (Int i = 1; i < mcdi.GetNumberOfEntries(); i++)
					{
						if (mcdi.GetNthEntryOffset(i - 1) >= mcdi.GetNthEntryOffset(i)) valid = False;
					}

					/* Found a binary MCDI field.
					 */
					if (valid) info.mcdi.SetData(data);
				}
				else
				{
					/* Found offset string.
					 */
					for (Int i = 0; unsigned(i) < cbLength / 2; i++)
					{
						info.offsets[i] = ((short *) pbValue)[i];

						if (info.offsets[i] == 0) break;
					}
				}
			}
			else if (String(name) == g_wszWMPicture && config->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
			{
				WM_PICTURE	*picData = (WM_PICTURE *) pbValue;
				Picture		 picture;

				picture.type	    = picData->bPictureType;
				picture.mime	    = picData->pwszMIMEType;
				picture.description = String(picData->pwszDescription).Trim();

				picture.data.Set(picData->pbData, picData->dwDataLen);

				if (picture.data.Size() >= 16)
				{
					if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
					else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
						 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
						 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
						 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

					if (picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
				}
			}

			delete [] pbValue;
			delete [] name;
		}

		delete [] indices;

		/* Set artist to album artist if artist is not filled.
		 */
		if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

		track.SetInfo(info);

		/* Read chapters.
		 */
		WORD	 numMarkers = 0;

		pHeaderInfo->GetMarkerCount(&numMarkers);

		if (numMarkers > 0 && config->GetIntValue(ConfigID, "ReadChapters", True))
		{
			for (Int i = 0; i < numMarkers; i++)
			{
				const Format	&format = track.GetFormat();

				QWORD	 cnsMarkerTime	  = 0;
				WORD	 cchMarkerNameLen = 0;

				pHeaderInfo->GetMarker(i, NIL, &cchMarkerNameLen, NIL);

				WCHAR	*pwszMarkerName = new WCHAR [cchMarkerNameLen];

				pHeaderInfo->GetMarker(i, pwszMarkerName, &cchMarkerNameLen, &cnsMarkerTime);

				/* Fill track data.
				 */
				Track	 rTrack;

				rTrack.fileName = track.fileName;
				rTrack.pictures = track.pictures;

				rTrack.sampleOffset = Math::Round(cnsMarkerTime * format.rate / 10000000.0);

				if	(track.length	    >= 0) rTrack.length	      = track.length	   - rTrack.sampleOffset;
				else if (track.approxLength >= 0) rTrack.approxLength = track.approxLength - rTrack.sampleOffset;
				else				  rTrack.approxLength = 240 * format.rate;

				if	(track.length	    > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * rTrack.length);
				else if (track.approxLength > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * rTrack.approxLength);
				else				 rTrack.fileSize = rTrack.approxLength * format.channels * (format.bits / 8);

				rTrack.SetFormat(format);

				/* Set track title.
				 */
				Info	 info = track.GetInfo();

				info.title = String(pwszMarkerName).Trim();
				info.track = i + 1;

				rTrack.SetInfo(info);

				/* Update previous track length.
				 */
				if (i > 0)
				{
					Track	&pTrack = track.tracks.GetNthReference(i - 1);

					pTrack.length	= rTrack.sampleOffset - pTrack.sampleOffset;

					if	(track.length	    > 0) pTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * pTrack.length);
					else if (track.approxLength > 0) pTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * pTrack.length);
					else				 pTrack.fileSize = pTrack.length * format.channels * (format.bits / 8);
				}

				/* Add track to track list.
				 */
				track.tracks.Add(rTrack);

				delete [] pwszMarkerName;
			}
		}

		pHeaderInfo->Release();

		/* Close metadata editor.
		 */
		hr = metadataEditor2->Close();
	}

	metadataEditor->Release();
	metadataEditor2->Release();

	if (FAILED(hr)) return Error();
	else		return Success();
}

Error BoCA::TaggerWMA::UpdateStreamInfo(const String &fileName, const Track &track)
{
	/* Create metadata editor objects.
	 */
	IWMMetadataEditor	*metadataEditor	 = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	HRESULT	 hr = ex_WMCreateEditor(&metadataEditor);

	if (FAILED(hr)) return Error();

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	if (FAILED(hr))
	{
		metadataEditor->Release();

		return Error();
	}

	/* Open file and remove tags.
	 */
	hr = metadataEditor2->OpenEx(toUNCPath(fileName), GENERIC_READ | GENERIC_WRITE, 0);

	if (!FAILED(hr))
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		/* Remove metadata first.
		 */
		WORD	 langIndex = 0;
		WORD	 numIndices = 0;

		pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, NIL, &numIndices);

		WORD	*indices = new WORD [numIndices];

		pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, indices, &numIndices);

		for (Int i = 0; i < numIndices; i++)
		{
			WORD	 nameLen = 1024;
			LPWSTR	 name = new WCHAR [nameLen];
			DWORD	 cbLength = 0;

			hr = pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, NIL, NIL, NIL, &cbLength);

			if (!FAILED(hr))
			{
				String	 nameStr = name;

				if (nameStr == g_wszWMAuthor			||
				    nameStr == g_wszWMTitle			||
				    nameStr == g_wszWMAlbumTitle		||
				    nameStr == g_wszWMYear			||
				    nameStr == g_wszWMGenre			||
				    nameStr == g_wszWMDescription		||
				    nameStr == g_wszWMPublisher			||
				    nameStr == g_wszWMISRC			||

				    nameStr == g_wszWMAlbumArtist		||

				    nameStr == g_wszWMContentGroupDescription	||
				    nameStr == g_wszWMSubTitle			||

				    nameStr == g_wszWMConductor			||
				    nameStr == g_wszWMComposer			||
				    nameStr == g_wszWMWriter			||

				    nameStr == g_wszWMOriginalArtist		||
				    nameStr == g_wszWMOriginalAlbumTitle	||
				    nameStr == g_wszWMOriginalLyricist		||
				    nameStr == g_wszWMOriginalReleaseYear	||

				    nameStr == g_wszWMBeatsPerMinute		||
				    nameStr == g_wszWMInitialKey		||

				    nameStr == g_wszWMCopyright			||

				    nameStr == g_wszWMRadioStationName		||
				    nameStr == g_wszWMRadioStationOwner		||

				    nameStr == g_wszWMAuthorURL			||
				    nameStr == g_wszWMAudioSourceURL		||
				    nameStr == g_wszWMCopyrightURL		||
				    nameStr == g_wszWMPromotionURL		||

				    nameStr == g_wszWMSharedUserRating		||

				    nameStr == g_wszWMTrack			||
				    nameStr == g_wszWMTrackNumber		||
				    nameStr == g_wszWMPartOfSet			||
				    nameStr == g_wszWMMCDI			||
				    nameStr == g_wszWMPicture) pHeaderInfo->DeleteAttribute(0, indices[i]);
			}

			delete [] name;
		}

		delete [] indices;

		pHeaderInfo->Release();

		/* Flush metadata editor.
		 */
		hr = metadataEditor2->Flush();
	}

	metadataEditor2->Release();
	metadataEditor->Release();

	if (FAILED(hr)) return Error();
	else		return RenderStreamInfo(fileName, track);
}
