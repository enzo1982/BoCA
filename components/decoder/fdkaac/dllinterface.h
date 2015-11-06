 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

#include "fdk-aac/aacdecoder_lib.h"
#include "mp4v2/mp4v2.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*fdkaacdll;
extern DynamicLoader	*mp4v2dll;

Bool			 LoadFDKAACDLL();
Void			 FreeFDKAACDLL();

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef HANDLE_AACDECODER		(LINKSPEC_H *AACDECODER_OPEN)		(TRANSPORT_TYPE, UINT);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_CONFIGRAW)	(HANDLE_AACDECODER, UCHAR *[], const UINT []);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_FILL)		(HANDLE_AACDECODER, UCHAR *[], const UINT [], UINT *);
typedef CStreamInfo *			(LINKSPEC_H *AACDECODER_GETSTREAMINFO)	(HANDLE_AACDECODER);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_DECODEFRAME)	(HANDLE_AACDECODER, INT_PCM *, const INT, const UINT);
typedef void				(LINKSPEC_H *AACDECODER_CLOSE)		(HANDLE_AACDECODER);

extern AACDECODER_OPEN			 ex_aacDecoder_Open;
extern AACDECODER_CONFIGRAW		 ex_aacDecoder_ConfigRaw;
extern AACDECODER_FILL			 ex_aacDecoder_Fill;
extern AACDECODER_GETSTREAMINFO		 ex_aacDecoder_GetStreamInfo;
extern AACDECODER_DECODEFRAME		 ex_aacDecoder_DecodeFrame;
extern AACDECODER_CLOSE			 ex_aacDecoder_Close;

typedef MP4FileHandle			(*MP4READ)				(const char *);
typedef void				(*MP4CLOSE)				(MP4FileHandle, uint32_t);
typedef void				(*MP4FREE)				(void *);
typedef uint32_t			(*MP4GETNUMBEROFTRACKS)			(MP4FileHandle, const char *, uint8_t);
typedef MP4TrackId			(*MP4FINDTRACKID)			(MP4FileHandle, uint16_t, const char *, uint8_t);
typedef const char *			(*MP4GETTRACKTYPE)			(MP4FileHandle, MP4TrackId);
typedef uint8_t				(*MP4GETTRACKAUDIOMPEG4TYPE)		(MP4FileHandle, MP4TrackId);
typedef bool				(*MP4GETTRACKESCONFIGURATION)		(MP4FileHandle, MP4TrackId, uint8_t **, uint32_t *);
typedef MP4Duration			(*MP4GETTRACKDURATION)			(MP4FileHandle, MP4TrackId);
typedef uint32_t			(*MP4GETTRACKTIMESCALE)			(MP4FileHandle, MP4TrackId);
typedef MP4Timestamp			(*MP4GETSAMPLETIME)			(MP4FileHandle, MP4TrackId, MP4SampleId); 
typedef MP4SampleId			(*MP4GETSAMPLEIDFROMTIME)		(MP4FileHandle, MP4TrackId, MP4Timestamp, bool);
typedef bool				(*MP4READSAMPLE)			(MP4FileHandle, MP4TrackId, MP4SampleId, uint8_t **, uint32_t *, MP4Timestamp *, MP4Duration *, MP4Duration *, bool *);
typedef MP4ItmfItemList *		(*MP4ITMFGETITEMSBYMEANING)		(MP4FileHandle, const char *, const char *);
typedef void				(*MP4ITMFITEMLISTFREE)			(MP4ItmfItemList *);

extern MP4READ				 ex_MP4Read;
extern MP4CLOSE				 ex_MP4Close;
extern MP4FREE				 ex_MP4Free;
extern MP4GETNUMBEROFTRACKS		 ex_MP4GetNumberOfTracks;
extern MP4FINDTRACKID			 ex_MP4FindTrackId;
extern MP4GETTRACKTYPE			 ex_MP4GetTrackType;
extern MP4GETTRACKAUDIOMPEG4TYPE	 ex_MP4GetTrackAudioMpeg4Type;
extern MP4GETTRACKESCONFIGURATION	 ex_MP4GetTrackESConfiguration;
extern MP4GETTRACKDURATION		 ex_MP4GetTrackDuration;
extern MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale;
extern MP4GETSAMPLETIME			 ex_MP4GetSampleTime;
extern MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime;
extern MP4READSAMPLE			 ex_MP4ReadSample;
extern MP4ITMFGETITEMSBYMEANING		 ex_MP4ItmfGetItemsByMeaning;
extern MP4ITMFITEMLISTFREE		 ex_MP4ItmfItemListFree;
