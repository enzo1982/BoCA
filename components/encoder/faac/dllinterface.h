 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

#include <faac.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*faacdll;

Bool			 LoadFAACDLL();
Void			 FreeFAACDLL();

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
