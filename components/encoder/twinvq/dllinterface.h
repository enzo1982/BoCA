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

#include "twinvq/twinvq.h"
#include "twinvq/tvqenc.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*twinvqdll;

Bool			 LoadTwinVQDLL();
Void			 FreeTwinVQDLL();

typedef int			(*TVQGETVERSIONID)		(int, char *);
typedef int			(*TVQENCINITIALIZE)		(headerInfo *, encSpecificInfo *, INDEX *, int);
typedef void			(*TVQENCTERMINATE)		(INDEX *);
typedef int			(*TVQENCGETFRAMESIZE)		();
typedef int			(*TVQENCGETNUMCHANNELS)		();
typedef void			(*TVQENCGETCONFINFO)		(tvqConfInfo *);
typedef void			(*TVQENCGETVECTORINFO)		(int *[], int *[]);
typedef void			(*TVQENCUPDATEVECTORINFO)	(int, int *, int [], int []);
typedef void			(*TVQENCODEFRAME)		(float [], INDEX *);

extern TVQGETVERSIONID		 ex_TvqGetVersionID;
extern TVQENCINITIALIZE		 ex_TvqEncInitialize;
extern TVQENCTERMINATE		 ex_TvqEncTerminate;
extern TVQENCGETFRAMESIZE	 ex_TvqEncGetFrameSize;
extern TVQENCGETNUMCHANNELS	 ex_TvqEncGetNumChannels;
extern TVQENCGETCONFINFO	 ex_TvqEncGetConfInfo;
extern TVQENCGETVECTORINFO	 ex_TvqEncGetVectorInfo;
extern TVQENCUPDATEVECTORINFO	 ex_TvqEncUpdateVectorInfo;
extern TVQENCODEFRAME		 ex_TvqEncodeFrame;
