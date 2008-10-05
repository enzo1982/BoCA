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

#include "twinvq/twinvq.h"
#include "twinvq/tvqdec.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*twinvqdll;

Bool			 LoadTwinVQDLL();
Void			 FreeTwinVQDLL();

typedef int				(*TVQINITIALIZE)		(headerInfo *, INDEX *, int);
typedef void				(*TVQTERMINATE)			(INDEX *);
typedef int				(*TVQGETFRAMESIZE)		();
typedef int				(*TVQGETNUMCHANNELS)		();
typedef int				(*TVQGETBITRATE)		();
typedef float				(*TVQGETSAMPLINGRATE)		();
typedef void				(*TVQDECODEFRAME)		(INDEX *, float []);
typedef void				(*TVQGETVECTORINFO)		(int *[], int *[]);
typedef int				(*TVQWTYPETOBTYPE)		(int, int *);
typedef void				(*TVQUPDATEVECTORINFO)		(int, int *, int [], int []);
typedef int				(*TVQCHECKVERSION)		(char *);
typedef void				(*TVQGETCONFINFO)		(tvqConfInfo *);
typedef int				(*TVQGETNUMFIXEDBITSPERFRAME)	();

extern TVQINITIALIZE			 ex_TvqInitialize;
extern TVQTERMINATE			 ex_TvqTerminate;
extern TVQGETFRAMESIZE			 ex_TvqGetFrameSize;
extern TVQGETNUMCHANNELS		 ex_TvqGetNumChannels;
extern TVQGETBITRATE			 ex_TvqGetBitRate;
extern TVQGETSAMPLINGRATE		 ex_TvqGetSamplingRate;
extern TVQDECODEFRAME			 ex_TvqDecodeFrame;
extern TVQGETVECTORINFO			 ex_TvqGetVectorInfo;
extern TVQWTYPETOBTYPE			 ex_TvqWtypeToBtype;
extern TVQUPDATEVECTORINFO		 ex_TvqUpdateVectorInfo;
extern TVQCHECKVERSION			 ex_TvqCheckVersion;
extern TVQGETCONFINFO			 ex_TvqGetConfInfo;
extern TVQGETNUMFIXEDBITSPERFRAME	 ex_TvqGetNumFixedBitsPerFrame;
