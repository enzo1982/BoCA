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

#include <smooth.h>

#ifdef callbacks
#	undef callbacks
#endif

#include <mp4v2/mp4v2.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mp4v2dll;

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef MP4FileHandle			(*MP4READ)			(const char *);
typedef MP4FileHandle			(*MP4READCALLBACKS)		(const MP4IOCallbacks *, void *);
typedef void				(*MP4CLOSE)			(MP4FileHandle, uint32_t);
typedef void				(*MP4FREE)			(void *);
typedef MP4TrackId			(*MP4FINDTRACKID)		(MP4FileHandle, uint16_t, const char *, uint8_t);
typedef MP4Duration			(*MP4GETTRACKDURATION)		(MP4FileHandle, MP4TrackId);
typedef bool				(*MP4HAVETRACKATOM)		(MP4FileHandle, MP4TrackId, const char *);
typedef bool				(*MP4GETTRACKBYTESPROPERTY)	(MP4FileHandle, MP4TrackId, const char *, uint8_t **, uint32_t *);
typedef uint32_t			(*MP4GETTRACKTIMESCALE)		(MP4FileHandle, MP4TrackId);
typedef MP4SampleId			(*MP4GETTRACKNUMBEROFSAMPLES)	(MP4FileHandle, MP4TrackId);
typedef MP4Timestamp			(*MP4GETSAMPLETIME)		(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef MP4SampleId			(*MP4GETSAMPLEIDFROMTIME)	(MP4FileHandle, MP4TrackId, MP4Timestamp, bool);
typedef uint32_t			(*MP4GETSAMPLESIZE)		(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef bool				(*MP4READSAMPLE)		(MP4FileHandle, MP4TrackId, MP4SampleId, uint8_t **, uint32_t *, MP4Timestamp *, MP4Duration *, MP4Duration *, bool *);
typedef MP4ItmfItemList *		(*MP4ITMFGETITEMSBYMEANING)	(MP4FileHandle, const char *, const char *);
typedef void				(*MP4ITMFITEMLISTFREE)		(MP4ItmfItemList *);

extern MP4READ				 ex_MP4Read;
extern MP4READCALLBACKS			 ex_MP4ReadCallbacks;
extern MP4CLOSE				 ex_MP4Close;
extern MP4FREE				 ex_MP4Free;
extern MP4FINDTRACKID			 ex_MP4FindTrackId;
extern MP4GETTRACKDURATION		 ex_MP4GetTrackDuration;
extern MP4HAVETRACKATOM			 ex_MP4HaveTrackAtom;
extern MP4GETTRACKBYTESPROPERTY		 ex_MP4GetTrackBytesProperty;
extern MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale;
extern MP4GETTRACKNUMBEROFSAMPLES	 ex_MP4GetTrackNumberOfSamples;
extern MP4GETSAMPLETIME			 ex_MP4GetSampleTime;
extern MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime;
extern MP4GETSAMPLESIZE			 ex_MP4GetSampleSize;
extern MP4READSAMPLE			 ex_MP4ReadSample;
extern MP4ITMFGETITEMSBYMEANING		 ex_MP4ItmfGetItemsByMeaning;
extern MP4ITMFITEMLISTFREE		 ex_MP4ItmfItemListFree;
