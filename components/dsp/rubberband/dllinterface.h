 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

#include "rubberband/rubberband-c.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*rbdll;

Bool			 LoadRubberBandDLL();
Void			 FreeRubberBandDLL();

typedef RubberBandState	(*RUBBERBAND_NEW)			(unsigned int, unsigned int, RubberBandOptions, double, double);
typedef void		(*RUBBERBAND_DELETE)			(RubberBandState);
typedef unsigned int	(*RUBBERBAND_GET_LATENCY)		(const RubberBandState);
typedef unsigned int	(*RUBBERBAND_GET_SAMPLES_REQUIRED)	(const RubberBandState);
typedef void		(*RUBBERBAND_PROCESS)			(RubberBandState, const float *const *, unsigned int, int);
typedef int		(*RUBBERBAND_AVAILABLE)			(const RubberBandState);
typedef unsigned int	(*RUBBERBAND_RETRIEVE)			(const RubberBandState, float *const *, unsigned int);

extern RUBBERBAND_NEW			 ex_rubberband_new;
extern RUBBERBAND_DELETE		 ex_rubberband_delete;
extern RUBBERBAND_GET_LATENCY		 ex_rubberband_get_latency;
extern RUBBERBAND_GET_SAMPLES_REQUIRED	 ex_rubberband_get_samples_required;
extern RUBBERBAND_PROCESS		 ex_rubberband_process;
extern RUBBERBAND_AVAILABLE		 ex_rubberband_available;
extern RUBBERBAND_RETRIEVE		 ex_rubberband_retrieve;
