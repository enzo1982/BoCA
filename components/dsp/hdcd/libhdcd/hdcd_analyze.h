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

#ifndef _HDCD_ANALYZE_H_
#define _HDCD_ANALYZE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** Analyze mode(s)
 *
 *   In analyze mode, the audio is replaced by a solid tone, and
 *   amplitude is changed to signal when the specified feature is
 *   used, or some decoding state exists.
 */

typedef enum {
    HDCD_ANA_OFF    = 0,
    HDCD_ANA_LLE    = 1,
    HDCD_ANA_PE     = 2,
    HDCD_ANA_CDT    = 3,
    HDCD_ANA_TGM    = 4,
    HDCD_ANA_PEL    = 5, /* added with simple api */
    HDCD_ANA_LTGM   = 6, /* added with simple api */
} hdcd_ana_mode;

#define HDCD_ANA_OFF_DESC "disabled"
#define HDCD_ANA_LLE_DESC "gain adjustment level at each sample"
#define HDCD_ANA_PE_DESC  "samples where peak extend occurs"
#define HDCD_ANA_CDT_DESC "samples where the code detect timer is active"
#define HDCD_ANA_TGM_DESC "samples where the target gain does not match between channels"
#define HDCD_ANA_PEL_DESC  "any samples above peak extend level"
#define HDCD_ANA_LTGM_DESC "gain adjustment level at each sample, in each channel"

/** get a nice description of what a mode does */
const char* hdcd_str_analyze_mode_desc(hdcd_ana_mode mode);

#ifdef __cplusplus
}
#endif

#endif
