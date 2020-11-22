/*
 *  Copyright (C) 2010, Chris Moeller,
 *  All rights reserved.
 *  Optimizations by Gumboot
 *  Additional work by Burt P.
 *  Original code reverse engineered from HDCD decoder library by Christopher Key,
 *  which was likely reverse engineered from Windows Media Player.
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

/*
 * HDCD is High Definition Compatible Digital
 * http://wiki.hydrogenaud.io/index.php?title=High_Definition_Compatible_Digital
 *
 * More information about HDCD-encoded audio CDs:
 * http://www.audiomisc.co.uk/HFN/HDCD/Enigma.html
 * http://www.audiomisc.co.uk/HFN/HDCD/Examined.html
 */

#ifndef _HDCD_DECODE2_H_
#define _HDCD_DECODE2_H_

#include <stdint.h>
#include <stdarg.h>

#include "hdcd_libversion.h"
#include "hdcd_detect.h"         /* enums for various detection values */
#include "hdcd_analyze.h"        /* enums and definitions for analyze modes */

#ifdef __cplusplus
extern "C" {
#endif


/********************* optional logging ************************/

typedef void (*hdcd_log_callback)(const void *priv, const char* fmt, va_list args);

typedef struct {
    uint32_t sid; /**< internal struct identity = HDCD_SID_LOGGER */

    int enable;
    void *priv;
    hdcd_log_callback log_func;
} hdcd_log;

int _hdcd_log_init(hdcd_log *log, hdcd_log_callback func, void *priv);
void _hdcd_log(hdcd_log *log, const char* fmt, ...);
void _hdcd_log_enable(hdcd_log *log);
void _hdcd_log_disable(hdcd_log *log);

/********************* decoding ********************************/

#define HDCD_FLAG_FORCE_PE         128
#define HDCD_FLAG_TGM_LOG_OFF       64

typedef struct {
    uint32_t sid; /**< internal struct identity = HDCD_SID_STATE */

    int decoder_options;  /**< as flags HDCD_FLAG_* */

    uint64_t window;
    unsigned char readahead;

    /** arg is set when a packet prefix is found.
     *  control is the active control code, where
     *  bit 0-3: target_gain, 4-bit (3.1) fixed-point value
     *  bit 4  : peak_extend
     *  bit 5  : transient_filter
     *  bit 6,7: always zero */
    uint8_t arg, control;
    unsigned int sustain, sustain_reset; /**< code detect timer */

    int running_gain; /**< 11-bit (3.8) fixed point, extended from target_gain */

    int bits;             /**< sample bit depth: 16, 20, 24 */
    int rate;             /**< sample rate */
    int cdt_period;       /**< cdt period in ms */

    /** counters */
    int code_counterA;            /**< 8-bit format packet */
    int code_counterA_almost;     /**< looks like an A code, but a bit expected to be 0 is 1 */
    int code_counterB;            /**< 16-bit format packet, 8-bit code, 8-bit XOR of code */
    int code_counterB_checkfails; /**< looks like a B code, but doesn't pass the XOR check */
    int code_counterC;            /**< packet prefix was found, expect a code */
    int code_counterC_unmatched;  /**< told to look for a code, but didn't find one */
    int count_peak_extend;        /**< valid packets where peak_extend was enabled */
    int count_transient_filter;   /**< valid packets where filter was detected */
    /** target_gain is a 4-bit (3.1) fixed-point value, always
     *  negative, but stored positive.
     *  The 16 possible values range from -7.5 to 0.0 dB in
     *  steps of 0.5, but no value below -6.0 dB should appear. */
    int gain_counts[16];
    int max_gain;
    /** occurences of code detect timer expiring without detecting
     *  a code. -1 for timer never set. */
    int count_sustain_expired;

    hdcd_log *log;              /**< optional logging */
    int sample_count;           /**< used in logging  */
    hdcd_ana_mode ana_mode;     /**< analyze mode     */
    int _ana_snb;               /**< used in the analyze mode tone generator */

} hdcd_state;

typedef struct {
    uint32_t sid; /**< internal struct identity = HDCD_SID_STATE_STEREO */

    hdcd_state channel[2];      /**< individual channel states       */
    hdcd_ana_mode ana_mode;     /**< analyze mode                    */
    int val_target_gain;        /**< last valid matching target_gain */
    int count_tg_mismatch;      /**< target_gain mismatch samples  */
} hdcd_state_stereo;

/* n-channel versions */
void _hdcd_reset(hdcd_state *state, unsigned rate, unsigned bits, int sustain_period_ms, int flags);
void _hdcd_process(hdcd_state *state, int *samples, int count, int stride);

/* stereo versions */
void _hdcd_reset_stereo(hdcd_state_stereo *state, unsigned rate, unsigned bits, int sustain_period_ms, int flags);
void _hdcd_process_stereo(hdcd_state_stereo *state, int *samples, int count);

/* hdcd_state* or hdcd_state_stereo* */
void _hdcd_attach_logger(void *state, hdcd_log *log); /* log = NULL to use the default logger */
void _hdcd_set_analyze_mode(void *state, hdcd_ana_mode mode);


/********************* optional detection and stats ************/

typedef struct {
    uint32_t sid; /**< internal struct identity = HDCD_SID_DETECTION_DATA */

    hdcd_dv hdcd_detected;
    hdcd_pf packet_type;
    int total_packets;         /**< valid packets */
    int errors;                /**< detectable errors */
    hdcd_pe peak_extend;
    int uses_transient_filter;
    float max_gain_adjustment; /**< in dB, expected in the range -7.5 to 0.0 */
    int cdt_expirations;       /**< -1 for never set, 0 for set but never expired */

    int _active_count;         /**< used internally */
} hdcd_detection_data;

void _hdcd_detect_reset(hdcd_detection_data *detect);

void _hdcd_detect_start(hdcd_detection_data *detect);
void _hdcd_detect_onech(hdcd_state *state, hdcd_detection_data *detect);
void _hdcd_detect_end(hdcd_detection_data *detect, int channels);
/* combines _start() _onech()(x2) _end */
void _hdcd_detect_stereo(hdcd_state_stereo *state, hdcd_detection_data *detect);

/* get a string with an HDCD detection summary */
void _hdcd_detect_str(hdcd_detection_data *detect, char *str, int maxlen); /* [256] should be enough */

/* dump the hdcd_state struct to the log */
void _hdcd_dump_state_to_log(hdcd_state *state, int channel);
/* ... in the ffmpeg af_hdcd style */
void _hdcd_dump_state_to_log_ffmpeg(hdcd_state *state, int channel);

#ifdef __cplusplus
}
#endif

#endif

