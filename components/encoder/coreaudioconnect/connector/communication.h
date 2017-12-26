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

#ifndef H_COREAUDIOCOMMUNICATION
#define H_COREAUDIOCOMMUNICATION

#include <stdint.h>

/* Status codes.
 */
const int32_t	 CommStatusIssued     = 0;	// Command has been issued by the host process.
const int32_t	 CommStatusProcessing = 1;	// Connector process is processing a command.
const int32_t	 CommStatusReady      = 2;	// Connector process finished processing a command.
const int32_t	 CommStatusError      = 3;	// Error while processing a command.

/* Adapter commands.
 */
const int32_t	 CommCommandHello     = 'HELO';	// Hello     ->	Say hello.
const int32_t	 CommCommandCodecs    = 'CDCS';	// Codecs   <-	Get available codecs.
const int32_t	 CommCommandSetup     = 'STUP';	// Setup     ->	Setup encoder.
const int32_t	 CommCommandEncode    = 'ENCD';	// Raw	     ->	Encode audio samples.
const int32_t	 CommCommandFinish    = 'FNSH'; //		Finish encoding.
const int32_t	 CommCommandQuit      = 'QUIT';	//		Quit adapter process.

/* General command data structure.
 */
struct CoreAudioCommBuffer
{
	int32_t		 status;
	int32_t		 command;

	int32_t		 length;
	int32_t		 data[65536];
};

/* Data structure for Hello command.
 */
struct CoreAudioCommHello
{
	int32_t		 version;
};

/* Data structure for Codecs command.
 */
struct CoreAudioCommCodecs
{
	uint32_t	 codecs[32];
	int32_t		 bitrates[32][128];
};

/* Data structure for Setup command.
 */
struct CoreAudioCommSetup
{
	uint32_t	 codec;
	int32_t		 bitrate;
	int32_t		 format;

	int32_t		 channels;
	int32_t		 rate;
	int32_t		 bits;

	int8_t		 fp;
	int8_t		 sign;

	char		 file[32768];
};

#endif
