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

#include "faad2/neaacdec.h"
#include "mp4v2/mp4v2.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*faad2dll;
extern DynamicLoader	*mp4v2dll;

Bool			 LoadFAAD2DLL();
Void			 FreeFAAD2DLL();

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef NeAACDecHandle			(*NEAACDECOPEN)				();
typedef long				(*NEAACDECINIT)				(NeAACDecHandle, unsigned char *, unsigned long, unsigned long *, unsigned char *);
typedef long				(*NEAACDECINIT2)			(NeAACDecHandle, unsigned char *, unsigned long, unsigned long *, unsigned char *);
typedef NeAACDecConfigurationPtr	(*NEAACDECGETCURRENTCONFIGURATION)	(NeAACDecHandle);
typedef unsigned char			(*NEAACDECSETCONFIGURATION)		(NeAACDecHandle, NeAACDecConfigurationPtr);
typedef char				(*NEAACDECAUDIOSPECIFICCONFIG)		(unsigned char *, unsigned long, mp4AudioSpecificConfig *);
typedef void *				(*NEAACDECDECODE)			(NeAACDecHandle, NeAACDecFrameInfo *, unsigned char *, unsigned long);
typedef void				(*NEAACDECCLOSE)			(NeAACDecHandle);
typedef char *				(*NEAACDECGETERRORMESSAGE)		(unsigned char);
typedef int				(*NEAACDECGETVERSION)			(char **, char **);

extern NEAACDECOPEN			 ex_NeAACDecOpen;
extern NEAACDECINIT			 ex_NeAACDecInit;
extern NEAACDECINIT2			 ex_NeAACDecInit2;
extern NEAACDECGETCURRENTCONFIGURATION	 ex_NeAACDecGetCurrentConfiguration;
extern NEAACDECSETCONFIGURATION		 ex_NeAACDecSetConfiguration;
extern NEAACDECAUDIOSPECIFICCONFIG	 ex_NeAACDecAudioSpecificConfig;
extern NEAACDECDECODE			 ex_NeAACDecDecode;
extern NEAACDECCLOSE			 ex_NeAACDecClose;
extern NEAACDECGETERRORMESSAGE		 ex_NeAACDecGetErrorMessage;
extern NEAACDECGETVERSION		 ex_NeAACDecGetVersion;

typedef MP4FileHandle			(*MP4READPROVIDER)			(const char *, const MP4FileProvider *);
typedef void				(*MP4CLOSE)				(MP4FileHandle, uint32_t);
typedef void				(*MP4FREE)				(void *);
typedef uint32_t			(*MP4GETNUMBEROFTRACKS)			(MP4FileHandle, const char *, uint8_t);
typedef MP4TrackId			(*MP4FINDTRACKID)			(MP4FileHandle, uint16_t, const char *, uint8_t);
typedef const char *			(*MP4GETTRACKTYPE)			(MP4FileHandle, MP4TrackId);
typedef uint8_t				(*MP4GETTRACKAUDIOMPEG4TYPE)		(MP4FileHandle, MP4TrackId);
typedef bool				(*MP4GETTRACKESCONFIGURATION)		(MP4FileHandle, MP4TrackId, uint8_t **, uint32_t *);
typedef uint32_t			(*MP4GETTRACKTIMESCALE)			(MP4FileHandle, MP4TrackId);
typedef MP4SampleId			(*MP4GETTRACKNUMBEROFSAMPLES)		(MP4FileHandle, MP4TrackId);
typedef MP4Timestamp			(*MP4GETSAMPLETIME)			(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef MP4SampleId			(*MP4GETSAMPLEIDFROMTIME)		(MP4FileHandle, MP4TrackId, MP4Timestamp, bool);
typedef uint32_t			(*MP4GETSAMPLESIZE)			(MP4FileHandle, MP4TrackId, MP4SampleId);
typedef bool				(*MP4READSAMPLE)			(MP4FileHandle, MP4TrackId, MP4SampleId, uint8_t **, uint32_t *, MP4Timestamp *, MP4Duration *, MP4Duration *, bool *);
typedef MP4ItmfItemList *		(*MP4ITMFGETITEMSBYMEANING)		(MP4FileHandle, const char *, const char *);
typedef void				(*MP4ITMFITEMLISTFREE)			(MP4ItmfItemList *);

extern MP4READPROVIDER			 ex_MP4ReadProvider;
extern MP4CLOSE				 ex_MP4Close;
extern MP4FREE				 ex_MP4Free;
extern MP4GETNUMBEROFTRACKS		 ex_MP4GetNumberOfTracks;
extern MP4FINDTRACKID			 ex_MP4FindTrackId;
extern MP4GETTRACKTYPE			 ex_MP4GetTrackType;
extern MP4GETTRACKAUDIOMPEG4TYPE	 ex_MP4GetTrackAudioMpeg4Type;
extern MP4GETTRACKESCONFIGURATION	 ex_MP4GetTrackESConfiguration;
extern MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale;
extern MP4GETTRACKNUMBEROFSAMPLES	 ex_MP4GetTrackNumberOfSamples;
extern MP4GETSAMPLETIME			 ex_MP4GetSampleTime;
extern MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime;
extern MP4GETSAMPLESIZE			 ex_MP4GetSampleSize;
extern MP4READSAMPLE			 ex_MP4ReadSample;
extern MP4ITMFGETITEMSBYMEANING		 ex_MP4ItmfGetItemsByMeaning;
extern MP4ITMFITEMLISTFREE		 ex_MP4ItmfItemListFree;
