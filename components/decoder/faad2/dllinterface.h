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

#include <neaacdec.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*faad2dll;

Bool			 LoadFAAD2DLL();
Void			 FreeFAAD2DLL();

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
