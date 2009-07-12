 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "wma_tag.h"
#include "dllinterface.h"

const String &BoCA::WMATag::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (wmvcoredll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Windows Media Tagger</name>		\
		    <version>1.0</version>			\
		    <id>wma-tag</id>				\
		    <type>tagger</type>				\
		    <format>					\
		      <name>Windows Media Audio Files</name>	\
		      <extension>wma</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadWMVCoreDLL();
}

Void smooth::DetachDLL()
{
	FreeWMVCoreDLL();
}

BoCA::WMATag::WMATag()
{
	/* Init the Microsoft COM library.
	 */
	CoInitialize(NIL);
}

BoCA::WMATag::~WMATag()
{
	/* Uninit the Microsoft COM library.
	 */
	CoUninitialize();
}

Error BoCA::WMATag::RenderStreamInfo(const String &fileName, const Track &track)
{
	Config			*currentConfig = Config::Get();

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

		if (info.artist != NIL) RenderWMAItem(g_wszWMAuthor,	 info.artist,		     pHeaderInfo);
		if (info.title  != NIL) RenderWMAItem(g_wszWMTitle,	 info.title,		     pHeaderInfo);
		if (info.album  != NIL) RenderWMAItem(g_wszWMAlbumTitle, info.album,		     pHeaderInfo);
		if (info.year    >   0) RenderWMAItem(g_wszWMYear,	 String::FromInt(info.year), pHeaderInfo);
		if (info.genre  != NIL) RenderWMAItem(g_wszWMGenre,	 info.genre,		     pHeaderInfo);
		if (info.label  != NIL) RenderWMAItem(g_wszWMPublisher,	 info.label,		     pHeaderInfo);
		if (info.isrc   != NIL) RenderWMAItem(g_wszWMISRC,	 info.isrc,		     pHeaderInfo);

		if (info.track > 0)
		{
			RenderWMAItem(g_wszWMTrackNumber, String::FromInt(info.track), pHeaderInfo);
		}

		if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
		{
			String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

			if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

			RenderWMAItem(g_wszWMPartOfSet, discString, pHeaderInfo);
		}

		if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	RenderWMAItem(g_wszWMDescription, info.comment, pHeaderInfo);
		else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				RenderWMAItem(g_wszWMDescription, currentConfig->GetStringValue("Tags", "DefaultComment", NIL), pHeaderInfo);

		/* Save other text info.
		 */
		for (Int i = 0; i < info.other.Length(); i++)
		{
			String	 value = info.other.GetNth(i);

			if	(value.StartsWith(String(INFO_PERFORMER).Append(":")))	   RenderWMAItem(g_wszWMConductor,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   RenderWMAItem(g_wszWMComposer,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   RenderWMAItem(g_wszWMWriter,		     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);

			else if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))   RenderWMAItem(g_wszWMOriginalArtist,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))    RenderWMAItem(g_wszWMOriginalAlbumTitle,  value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":"))) RenderWMAItem(g_wszWMOriginalLyricist,    value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	   RenderWMAItem(g_wszWMOriginalReleaseYear, value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);

			else if	(value.StartsWith(String(INFO_WEB_ARTIST).Append(":")))	   RenderWMAItem(g_wszWMAuthorURL,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_WEB_SOURCE).Append(":")))	   RenderWMAItem(g_wszWMAudioSourceURL,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
			else if	(value.StartsWith(String(INFO_WEB_COPYRIGHT).Append(":"))) RenderWMAItem(g_wszWMCopyrightURL,	     value.Tail(value.Length() - value.Find(":") - 1), pHeaderInfo);
		}

		/* Save CD table of contents.
		 */
		if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
		{
			if (info.mcdi.Size() > 0)
			{
				RenderWMABinaryItem(g_wszWMMCDI, info.mcdi, pHeaderInfo);
			}
		}

		/* Save cover art.
		 */
		if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToWMAMetadata", True))
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

	if (hr == S_OK) return Success();
	else		return Error();
}

Error BoCA::WMATag::RenderWMAItem(const String &id, const String &value, Void *headerInfo)
{
	HRESULT	 hr = ((IWMHeaderInfo3 *) headerInfo)->AddAttribute(0, id, NIL, WMT_TYPE_STRING, 0, (BYTE *) (wchar_t *) value, wcslen(value) * 2 + 2);

	if (hr == S_OK) return Success();
	else		return Error();
}

Error BoCA::WMATag::RenderWMABinaryItem(const String &id, const Buffer<UnsignedByte> &value, Void *headerInfo)
{
	HRESULT	 hr = ((IWMHeaderInfo3 *) headerInfo)->AddAttribute(0, id, NIL, WMT_TYPE_BINARY, 0, (BYTE *) (UnsignedByte *) value, value.Size());

	if (hr == S_OK) return Success();
	else		return Error();
}

Error BoCA::WMATag::ParseStreamInfo(const String &fileName, Track &track)
{
	Config			*currentConfig = Config::Get();

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

		Info	&info = track.GetInfo();

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
			else if (String(name) == g_wszWMPicture && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
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

				if (picture.data.Size() > 16 && picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
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

	if (hr == S_OK) return Success();
	else		return Error();
}

Error BoCA::WMATag::UpdateStreamInfo(const String &fileName, const Track &track)
{
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
				String	 nameStr = name;

				if (name == g_wszWMAuthor		||
				    name == g_wszWMTitle		||
				    name == g_wszWMAlbumTitle		||
				    name == g_wszWMYear			||
				    name == g_wszWMGenre		||
				    name == g_wszWMDescription		||
				    name == g_wszWMPublisher		||
				    name == g_wszWMISRC			||

				    name == g_wszWMConductor		||
				    name == g_wszWMComposer		||
				    name == g_wszWMWriter		||

				    name == g_wszWMOriginalArtist	||
				    name == g_wszWMOriginalAlbumTitle	||
				    name == g_wszWMOriginalLyricist	||
				    name == g_wszWMOriginalReleaseYear	||

				    name == g_wszWMAuthorURL		||
				    name == g_wszWMAudioSourceURL	||
				    name == g_wszWMCopyrightURL		||

				    name == g_wszWMTrackNumber		||
				    name == g_wszWMPartOfSet		||
				    name == g_wszWMMCDI			||
				    name == g_wszWMPicture) hr = pHeaderInfo->DeleteAttribute(0, indices[i]);
			}

			delete [] name;
		}

		delete [] indices;

		pHeaderInfo->Release();
	}

	hr = metadataEditor2->Flush();

	metadataEditor->Release();
	metadataEditor2->Release();

	if (hr == S_OK) RenderStreamInfo(fileName, track);

	return Success();
}
