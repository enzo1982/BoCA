 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include "mp4v2/mp4v2.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mp4v2dll;

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef MP4FileHandle		(*MP4READPROVIDER)		(const char *, const MP4FileProvider *);
typedef void			(*MP4CLOSE)			(MP4FileHandle, uint32_t);
typedef void			(*MP4FREE)			(void *);
typedef uint32_t		(*MP4GETNUMBEROFTRACKS)		(MP4FileHandle, const char *, uint8_t);
typedef MP4TrackId		(*MP4FINDTRACKID)		(MP4FileHandle, uint16_t, const char *, uint8_t);
typedef const char *		(*MP4GETTRACKTYPE)		(MP4FileHandle, MP4TrackId);
typedef MP4Duration		(*MP4GETTRACKDURATION)		(MP4FileHandle, MP4TrackId);
typedef bool			(*MP4HAVETRACKATOM)		(MP4FileHandle, MP4TrackId, const char *);
typedef bool			(*MP4GETTRACKBYTESPROPERTY)	(MP4FileHandle, MP4TrackId, const char *, uint8_t **, uint32_t *);
typedef uint32_t		(*MP4GETTRACKTIMESCALE)		(MP4FileHandle, MP4TrackId);
typedef MP4Timestamp		(*MP4GETSAMPLETIME)		(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef MP4SampleId		(*MP4GETSAMPLEIDFROMTIME)	(MP4FileHandle, MP4TrackId, MP4Timestamp, bool);
typedef uint32_t		(*MP4GETSAMPLESIZE)		(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef bool			(*MP4READSAMPLE)		(MP4FileHandle, MP4TrackId, MP4SampleId, uint8_t **, uint32_t *, MP4Timestamp *, MP4Duration *, MP4Duration *, bool *);

extern MP4READPROVIDER		 ex_MP4ReadProvider;
extern MP4CLOSE			 ex_MP4Close;
extern MP4FREE			 ex_MP4Free;
extern MP4GETNUMBEROFTRACKS	 ex_MP4GetNumberOfTracks;
extern MP4FINDTRACKID		 ex_MP4FindTrackId;
extern MP4GETTRACKTYPE		 ex_MP4GetTrackType;
extern MP4GETTRACKDURATION	 ex_MP4GetTrackDuration;
extern MP4HAVETRACKATOM		 ex_MP4HaveTrackAtom;
extern MP4GETTRACKBYTESPROPERTY	 ex_MP4GetTrackBytesProperty;
extern MP4GETTRACKTIMESCALE	 ex_MP4GetTrackTimeScale;
extern MP4GETSAMPLETIME		 ex_MP4GetSampleTime;
extern MP4GETSAMPLEIDFROMTIME	 ex_MP4GetSampleIdFromTime;
extern MP4GETSAMPLESIZE		 ex_MP4GetSampleSize;
extern MP4READSAMPLE		 ex_MP4ReadSample;
