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
#include <rnnoise.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*rnnoisedll;

Bool			 LoadRNNoiseDLL();
Void			 FreeRNNoiseDLL();

typedef DenoiseState *	(*RNNOISE_CREATE)		(RNNModel *);
typedef void		(*RNNOISE_DESTROY)		(DenoiseState *);
typedef float		(*RNNOISE_PROCESS_FRAME)	(DenoiseState *, float *, const float *);
typedef RNNModel *	(*RNNOISE_MODEL_FROM_FILE)	(FILE *);
typedef void		(*RNNOISE_MODEL_FREE)		(RNNModel *);

extern RNNOISE_CREATE		 ex_rnnoise_create;
extern RNNOISE_DESTROY		 ex_rnnoise_destroy;
extern RNNOISE_PROCESS_FRAME	 ex_rnnoise_process_frame;
extern RNNOISE_MODEL_FROM_FILE	 ex_rnnoise_model_from_file;
extern RNNOISE_MODEL_FREE	 ex_rnnoise_model_free;
