/*
 *  Copyright (C) 2016, Burt P.,
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *    3. The names of its contributors may not be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _HDCD_SIMPLE_H_
#define _HDCD_SIMPLE_H_

#include <stdarg.h>
#include "hdcd_libversion.h"
#include "hdcd_detect.h"         /* enums for various detection values */
#include "hdcd_analyze.h"        /* enums and definitions for analyze modes */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hdcd_simple hdcd_simple;

/** create a new hdcd_simple context */
hdcd_simple *hdcd_new(void);
/** process 16-bit samples (stored in 32-bit), interlaced stereo.
 *  the samples will be converted in place to 32-bit samples. */
void hdcd_process(hdcd_simple *ctx, int *samples, int count);
/** on a song change or something, reset the decoding state */
void hdcd_reset(hdcd_simple *ctx);
/** version of hdcd_reset when not 44100Hz or 16-bit */
int hdcd_reset_ext(hdcd_simple *ctx, int rate, int bits);
/** free the context when finished */
void hdcd_free(hdcd_simple *ctx);

/** as hdcd_process(), but only scan. samples remain unprocessed.
 *  return expected value of hdcd_detected() after processing */
/*hdcd_dv*/
int hdcd_scan(hdcd_simple *ctx, int *samples, int count, int ignore_state);

/** is HDCD encoding detected? */
/*hdcd_dv*/ int hdcd_detected(hdcd_simple *ctx);                  /**< see hdcd_dv in hdcd_detect.h */
/** get a string with an HDCD detection summary */
void hdcd_detect_str(hdcd_simple *ctx, char *str, int maxlen); /* [256] should be enough */
/** get individual detection values */
/*hdcd_pf*/ int hdcd_detect_packet_type(hdcd_simple *ctx);           /**< see hdcd_pf in hdcd_detect.h */
            int hdcd_detect_total_packets(hdcd_simple *ctx);         /**< valid packets */
            int hdcd_detect_errors(hdcd_simple *ctx);                /**< detectable errors */
/*hdcd_pe*/ int hdcd_detect_peak_extend(hdcd_simple *ctx);           /**< see hdcd_pe in hdcd_detect.h */
            int hdcd_detect_uses_transient_filter(hdcd_simple *ctx); /**< bool, 1 if the tf flag was detected */
            float hdcd_detect_max_gain_adjustment(hdcd_simple *ctx); /**< in dB, expected in the range -7.5 to 0.0 */
            int hdcd_detect_cdt_expirations(hdcd_simple *ctx);       /**< -1 for never set, 0 for set but never expired */
            int hdcd_detect_lle_mismatch(hdcd_simple *ctx);          /**< number of samples with a mismatch in gain values between channels */


/** set a logging callback or use the default (print to stderr) */
typedef void (*hdcd_log_callback)(const void *priv, const char* fmt, va_list args);

int hdcd_logger_attach(hdcd_simple *ctx, hdcd_log_callback func, void *priv);
void hdcd_logger_default(hdcd_simple *ctx);
void hdcd_logger_detach(hdcd_simple *ctx);
void hdcd_logger_dump_state(hdcd_simple *s);


/** set the analyze mode */
int hdcd_analyze_mode(hdcd_simple *ctx, int mode);


#ifdef __cplusplus
}
#endif

#endif

