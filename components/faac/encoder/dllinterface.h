 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "faac/faac.h"
#include "mp4/mp4.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*faacdll;
extern DynamicLoader	*mp4v2dll;

Bool			 LoadFAACDLL();
Void			 FreeFAACDLL();

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef faacEncHandle			(FAACAPI *FAACENCOPEN)				(unsigned long, unsigned int, unsigned long *, unsigned long *);
typedef faacEncConfigurationPtr		(FAACAPI *FAACENCGETCURRENTCONFIGURATION)	(faacEncHandle);
typedef int				(FAACAPI *FAACENCSETCONFIGURATION)		(faacEncHandle, faacEncConfigurationPtr);
typedef int				(FAACAPI *FAACENCGETDECODERSPECIFICINFO)	(faacEncHandle, unsigned char **, unsigned long *);
typedef int				(FAACAPI *FAACENCENCODE)			(faacEncHandle, int32_t *, unsigned int, unsigned char *, unsigned int);
typedef int				(FAACAPI *FAACENCCLOSE)				(faacEncHandle);

extern FAACENCOPEN			 ex_faacEncOpen;
extern FAACENCGETCURRENTCONFIGURATION	 ex_faacEncGetCurrentConfiguration;
extern FAACENCSETCONFIGURATION		 ex_faacEncSetConfiguration;
extern FAACENCGETDECODERSPECIFICINFO	 ex_faacEncGetDecoderSpecificInfo;
extern FAACENCENCODE			 ex_faacEncEncode;
extern FAACENCCLOSE			 ex_faacEncClose;

typedef MP4FileHandle			(*MP4CREATEEX)					(const char *, u_int32_t, u_int32_t, int, int, char *, u_int32_t, char **, u_int32_t);
typedef bool				(*MP4CLOSE)					(MP4FileHandle);
typedef bool				(*MP4OPTIMIZE)					(const char *, const char *, u_int32_t);
typedef bool				(*MP4SETMETADATANAME)				(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATAARTIST)				(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATACOMMENT)			(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATAYEAR)				(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATAALBUM)				(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATAGENRE)				(MP4FileHandle, const char *);
typedef bool				(*MP4SETMETADATATRACK)				(MP4FileHandle, u_int16_t, u_int16_t);
typedef bool				(*MP4SETTRACKESCONFIGURATION)			(MP4FileHandle, MP4TrackId, const u_int8_t *, u_int32_t);
typedef void				(*MP4SETAUDIOPROFILELEVEL)			(MP4FileHandle, u_int8_t);
typedef MP4TrackId			(*MP4ADDAUDIOTRACK)				(MP4FileHandle, u_int32_t, MP4Duration, u_int8_t);
typedef bool				(*MP4WRITESAMPLE)				(MP4FileHandle, MP4TrackId, const u_int8_t *, u_int32_t, MP4Duration,
 MP4Duration, bool);

extern MP4CREATEEX			 ex_MP4CreateEx;
extern MP4CLOSE				 ex_MP4Close;
extern MP4OPTIMIZE			 ex_MP4Optimize;
extern MP4SETMETADATANAME		 ex_MP4SetMetadataName;
extern MP4SETMETADATAARTIST		 ex_MP4SetMetadataArtist;
extern MP4SETMETADATACOMMENT		 ex_MP4SetMetadataComment;
extern MP4SETMETADATAYEAR		 ex_MP4SetMetadataYear;
extern MP4SETMETADATAALBUM		 ex_MP4SetMetadataAlbum;
extern MP4SETMETADATAGENRE		 ex_MP4SetMetadataGenre;
extern MP4SETMETADATATRACK		 ex_MP4SetMetadataTrack;
extern MP4SETTRACKESCONFIGURATION	 ex_MP4SetTrackESConfiguration;
extern MP4SETAUDIOPROFILELEVEL		 ex_MP4SetAudioProfileLevel;
extern MP4ADDAUDIOTRACK			 ex_MP4AddAudioTrack;
extern MP4WRITESAMPLE			 ex_MP4WriteSample;
