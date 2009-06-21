 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagwma.h>
#include <boca/common/config.h>
#include <boca/common/utilities.h>
#include <boca/core/dllinterfaces.h>

using namespace smooth::IO;

BoCA::TagWMA::TagWMA()
{
}

BoCA::TagWMA::~TagWMA()
{
}

Int BoCA::TagWMA::Render(const Track &track, const String &fileName)
{
	Config		*currentConfig = Config::Get();

	CoInitialize(NIL);

	HRESULT			 hr = S_OK;

	IWMMetadataEditor	*metadataEditor = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	hr = ex_WMCreateEditor(&metadataEditor);

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	hr = metadataEditor2->OpenEx(fileName, GENERIC_READ | GENERIC_WRITE, 0);

	if (hr == S_OK)
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		hr = metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		const Info	&info = track.GetInfo();

		if	(info.artist != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMAuthor,	   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.artist,		      wcslen(info.artist)		  * 2 + 2);
		if	(info.title  != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMTitle,	   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.title,		      wcslen(info.title)		  * 2 + 2);
		if	(info.album  != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMAlbumTitle,  NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.album,		      wcslen(info.album)		  * 2 + 2);
		if	(info.year    >   0) hr = pHeaderInfo->AddAttribute(0, g_wszWMYear,	   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) String::FromInt(info.year),  wcslen(String::FromInt(info.year))  * 2 + 2);
		if	(info.genre  != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMGenre,	   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.genre,		      wcslen(info.genre)		  * 2 + 2);
		if	(info.label  != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMPublisher,   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.label,		      wcslen(info.label)		  * 2 + 2);
		if	(info.isrc   != NIL) hr = pHeaderInfo->AddAttribute(0, g_wszWMISRC,	   NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.isrc,		      wcslen(info.isrc)			  * 2 + 2);

		if	(info.track   >   0) hr = pHeaderInfo->AddAttribute(0, g_wszWMTrackNumber, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) String::FromInt(info.track), wcslen(String::FromInt(info.track)) * 2 + 2);

		if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
		{
			String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

			if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

			hr = pHeaderInfo->AddAttribute(0, g_wszWMPartOfSet, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) discString, wcslen(discString) * 2 + 2);
		}

		if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	hr = pHeaderInfo->AddAttribute(0, g_wszWMDescription, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) info.comment, wcslen(info.comment) * 2 + 2);
		else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				hr = pHeaderInfo->AddAttribute(0, g_wszWMDescription, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) currentConfig->GetStringValue("Tags", "DefaultComment", NIL), wcslen(currentConfig->GetStringValue("Tags", "DefaultComment", NIL)) * 2 + 2);

		/* Save other text info.
		 */
		for (Int i = 0; i < info.other.Length(); i++)
		{
			String	 value = info.other.GetNth(i);

			if	(value.StartsWith(String(INFO_PERFORMER).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMConductor,		 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMComposer,		 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMWriter,		 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);

			else if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))   hr = pHeaderInfo->AddAttribute(0, g_wszWMOriginalArtist,	 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))    hr = pHeaderInfo->AddAttribute(0, g_wszWMOriginalAlbumTitle,	 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":"))) hr = pHeaderInfo->AddAttribute(0, g_wszWMOriginalLyricist,	 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMOriginalReleaseYear, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);

			else if	(value.StartsWith(String(INFO_WEB_ARTIST).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMAuthorURL,		 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_WEB_SOURCE).Append(":")))	   hr = pHeaderInfo->AddAttribute(0, g_wszWMAudioSourceURL,	 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
			else if	(value.StartsWith(String(INFO_WEB_COPYRIGHT).Append(":"))) hr = pHeaderInfo->AddAttribute(0, g_wszWMCopyrightURL,	 NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value.Tail(value.Length() - value.Find(":") - 1), wcslen(value.Tail(value.Length() - value.Find(":") - 1)) * 2 + 2);
		}

		/* Save CD table of contents.
		 */
		if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
		{
			if (info.mcdi.Size() > 0)
			{
				hr = pHeaderInfo->AddAttribute(0, g_wszWMMCDI, NIL, WMT_TYPE_BINARY, 0, (BYTE *) (UnsignedByte *) info.mcdi, info.mcdi.Size());
			}
		}

		/* Save cover art.
		 */
		if (currentConfig->GetIntValue("Tags", "WriteCoverArt", True) && currentConfig->GetIntValue("Tags", "WriteCoverArtWMA", True))
		{
			foreach (const Picture &picInfo, track.pictures)
			{
				WM_PICTURE	 picture;

				picture.pwszMIMEType	= new WCHAR [picInfo.mime.Length() + 1];
				picture.bPictureType	= picInfo.type;
				picture.pwszDescription	= new WCHAR [picInfo.description.Length() + 1];
				picture.dwDataLen	= picInfo.data.Size();
				picture.pbData		= (BYTE *) (UnsignedByte *) picInfo.data;

				wcsncpy(picture.pwszMIMEType, picInfo.mime, picInfo.mime.Length() + 1);
				wcsncpy(picture.pwszDescription, picInfo.description, picInfo.description.Length() + 1);

				hr = pHeaderInfo->AddAttribute(0, g_wszWMPicture, NIL, WMT_TYPE_BINARY, 0, (BYTE *) &picture, sizeof(WM_PICTURE));

				delete [] picture.pwszMIMEType;
				delete [] picture.pwszDescription;
			}
		}

		pHeaderInfo->Release();
	}

	hr = metadataEditor2->Flush();

	metadataEditor->Release();
	metadataEditor2->Release();

	CoUninitialize();

	if (hr == S_OK) return Success();
	else		return Error();
}

Int BoCA::TagWMA::Parse(const String &fileName, Track *track)
{
	CoInitialize(NIL);

	HRESULT			 hr = S_OK;

	IWMMetadataEditor	*metadataEditor = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	hr = ex_WMCreateEditor(&metadataEditor);

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	hr = metadataEditor2->OpenEx(fileName, GENERIC_READ, FILE_SHARE_READ);

	if (hr == S_OK)
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		hr = metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		Info	&info = track->GetInfo();

		WORD	 langIndex = 0;
		WORD	 numIndices = 0;

		hr = pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, NIL, &numIndices);

		WORD	*indices = new WORD [numIndices];

		hr = pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, indices, &numIndices);

		for (Int i = 0; i < numIndices; i++)
		{
			WORD	 nameLen = 1024;
			LPWSTR	 name = new WCHAR [nameLen];
			DWORD	 cbLength = 0;

			hr = pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, NIL, NIL, NIL, &cbLength);

			BYTE	*pbValue = new BYTE [cbLength];

			hr = pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, NIL, NIL, pbValue, &cbLength);

			if	(String(name) == g_wszWMAuthor)			info.artist  = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMTitle)			info.title   = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMAlbumTitle)		info.album   = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMYear)			info.year    = String((LPWSTR) pbValue).ToInt();
			else if (String(name) == g_wszWMGenre)			info.genre   = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMDescription)		info.comment = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMPublisher)		info.label   = (LPWSTR) pbValue;
			else if (String(name) == g_wszWMISRC)			info.isrc    = (LPWSTR) pbValue;

			else if (String(name) == g_wszWMConductor)		info.other.Add(String(INFO_PERFORMER).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMComposer)		info.other.Add(String(INFO_COMPOSER).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMWriter)			info.other.Add(String(INFO_LYRICIST).Append(":").Append((LPWSTR) pbValue));

			else if (String(name) == g_wszWMOriginalArtist)		info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMOriginalAlbumTitle)	info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMOriginalLyricist)	info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMOriginalReleaseYear)	info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append((LPWSTR) pbValue));

			else if (String(name) == g_wszWMAuthorURL)		info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMAudioSourceURL)		info.other.Add(String(INFO_WEB_SOURCE).Append(":").Append((LPWSTR) pbValue));
			else if (String(name) == g_wszWMCopyrightURL)		info.other.Add(String(INFO_WEB_COPYRIGHT).Append(":").Append((LPWSTR) pbValue));

			else if (String(name) == g_wszWMTrackNumber)
			{
				String	 trackString = (LPWSTR) pbValue;

				info.track = trackString.ToInt();

				if (trackString.Find("/") >= 0) info.numTracks = trackString.Tail(trackString.Length() - trackString.Find("/") - 1).ToInt();
			}
			else if (String(name) == g_wszWMPartOfSet)
			{
				String	 discString = (LPWSTR) pbValue;

				info.disc = discString.ToInt();

				if (discString.Find("/") >= 0) info.numDiscs = discString.Tail(discString.Length() - discString.Find("/") - 1).ToInt();
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
					/* Found a binary MCDI field.
					 */
					info.mcdi.Resize(cbLength);

					memcpy(info.mcdi, pbValue, cbLength);
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
			else if (String(name) == g_wszWMPicture)
			{
				WM_PICTURE	*picData = (WM_PICTURE *) pbValue;
				Picture		 picture;

				picture.mime = picData->pwszMIMEType;

				if	(picture.mime.ToLower() == "jpeg" || picture.mime.ToLower() == "jpg") picture.mime = "image/jpeg";
				else if (picture.mime.ToLower() == "png")				      picture.mime = "image/png";

				picture.type = picData->bPictureType;
				picture.description = picData->pwszDescription;

				picture.data.Resize(picData->dwDataLen);

				memcpy(picture.data, picData->pbData, picture.data.Size());

				if (picture.data.Size() > 16 && picture.data[0] != 0 && picture.data[1] != 0) track->pictures.Add(picture);
			}

			delete [] pbValue;
			delete [] name;
		}

		delete [] indices;

		pHeaderInfo->Release();
	}

	hr = metadataEditor2->Close();

	metadataEditor->Release();
	metadataEditor2->Release();

	CoUninitialize();

	if (hr == S_OK) return Success();
	else		return Error();
}

Int BoCA::TagWMA::Update(const String &fileName, const Track &track)
{
	CoInitialize(NIL);

	HRESULT			 hr = S_OK;

	IWMMetadataEditor	*metadataEditor = NIL;
	IWMMetadataEditor2	*metadataEditor2 = NIL;

	hr = ex_WMCreateEditor(&metadataEditor);

	hr = metadataEditor->QueryInterface(IID_IWMMetadataEditor2, (void **) &metadataEditor2);

	hr = metadataEditor2->OpenEx(fileName, GENERIC_READ | GENERIC_WRITE, 0);

	if (hr == S_OK)
	{
		IWMHeaderInfo3	*pHeaderInfo = NIL;

		hr = metadataEditor2->QueryInterface(IID_IWMHeaderInfo3, (void **) &pHeaderInfo);

		/* Remove metadata first.
		 */
		WORD	 langIndex = 0;
		WORD	 numIndices = 0;

		hr = pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, NIL, &numIndices);

		WORD	*indices = new WORD [numIndices];

		hr = pHeaderInfo->GetAttributeIndices(0, NIL, &langIndex, indices, &numIndices);

		for (Int i = 0; i < numIndices; i++)
		{
			WORD	 nameLen = 1024;
			LPWSTR	 name = new WCHAR [nameLen];
			DWORD	 cbLength = 0;

			hr = pHeaderInfo->GetAttributeByIndexEx(0, indices[i], name, &nameLen, NIL, NIL, NIL, &cbLength);

			if (!FAILED(hr))
			{
				if (String(name) == g_wszWMAuthor		||
				    String(name) == g_wszWMTitle		||
				    String(name) == g_wszWMAlbumTitle		||
				    String(name) == g_wszWMYear			||
				    String(name) == g_wszWMGenre		||
				    String(name) == g_wszWMDescription		||
				    String(name) == g_wszWMPublisher		||
				    String(name) == g_wszWMISRC			||

				    String(name) == g_wszWMConductor		||
				    String(name) == g_wszWMComposer		||
				    String(name) == g_wszWMWriter		||

				    String(name) == g_wszWMOriginalArtist	||
				    String(name) == g_wszWMOriginalAlbumTitle	||
				    String(name) == g_wszWMOriginalLyricist	||
				    String(name) == g_wszWMOriginalReleaseYear	||

				    String(name) == g_wszWMAuthorURL		||
				    String(name) == g_wszWMAudioSourceURL	||
				    String(name) == g_wszWMCopyrightURL		||

				    String(name) == g_wszWMTrackNumber		||
				    String(name) == g_wszWMPartOfSet		||
				    String(name) == g_wszWMMCDI			||
				    String(name) == g_wszWMPicture) hr = pHeaderInfo->DeleteAttribute(0, indices[i]);
			}

			delete [] name;
		}

		delete [] indices;

		pHeaderInfo->Release();
	}

	hr = metadataEditor2->Flush();

	metadataEditor->Release();
	metadataEditor2->Release();

	CoUninitialize();

	if (hr == S_OK) Render(track, fileName);

	return Success();
}
