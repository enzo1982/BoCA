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

#include "hdcd_analyze.h"
#include "hdcd_detect.h"

const char* hdcd_str_analyze_mode_desc(hdcd_ana_mode mode)
{
    static const char * const ana_mode_str[] = {
        HDCD_ANA_OFF_DESC,
        HDCD_ANA_LLE_DESC,
        HDCD_ANA_PE_DESC,
        HDCD_ANA_CDT_DESC,
        HDCD_ANA_TGM_DESC,
        HDCD_ANA_PEL_DESC,
        HDCD_ANA_LTGM_DESC,
    };
    if (mode < 0 || mode > 6) return "";
    return ana_mode_str[mode];
}

const char* hdcd_str_detect(hdcd_dv v) {
    static const char * const det_str[] = {
        "not detected",
        "detected (no effect)",
        "detected"
    };
    if (v < 0 || v > 2) return "";
    return det_str[v];
}

const char* hdcd_str_pe(hdcd_pe v) {
    static const char * const pe_str[] = {
        "never enabled",
        "enabled intermittently",
        "enabled permanently"
    };
    if (v < 0 || v > 2) return "";
    return pe_str[v];
}

const char* hdcd_str_pformat(hdcd_pf v) {
    static const char * const pf_str[] = {
        "?", "A", "B", "A+B"
    };
    if (v < 0 || v > 3) return "";
    return pf_str[v];
}
