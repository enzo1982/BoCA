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

#include <windows.h>

namespace CA
{
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AudioFormat.h>
};

extern HINSTANCE	 coreaudiodll;

bool			 LoadCoreAudioDLL(const char *);
void			 FreeCoreAudioDLL();

namespace CA
{
	typedef OSStatus			(*AUDIOFILEINITIALIZEWITHCALLBACKS)	(void *, AudioFile_ReadProc, AudioFile_WriteProc, AudioFile_GetSizeProc, AudioFile_SetSizeProc, AudioFileTypeID, const AudioStreamBasicDescription *, UInt32, AudioFileID *);
	typedef OSStatus			(*AUDIOFILECLOSE)			(AudioFileID);
	typedef OSStatus			(*AUDIOFILESETPROPERTY)			(AudioFileID, AudioFilePropertyID, UInt32, const void *);
	typedef OSStatus			(*AUDIOFILEWRITEPACKETS)		(AudioFileID, Boolean, UInt32, const AudioStreamPacketDescription *, SInt64, UInt32 *, const void *);

	typedef OSStatus			(*AUDIOCONVERTERNEW)			(const AudioStreamBasicDescription *, const AudioStreamBasicDescription *, AudioConverterRef *);
	typedef OSStatus			(*AUDIOCONVERTERDISPOSE)		(AudioConverterRef);
	typedef OSStatus			(*AUDIOCONVERTERGETPROPERTY)		(AudioConverterRef, AudioConverterPropertyID, UInt32 *, void *);
	typedef OSStatus			(*AUDIOCONVERTERGETPROPERTYINFO)	(AudioConverterRef, AudioConverterPropertyID, UInt32 *, Boolean *);
	typedef OSStatus			(*AUDIOCONVERTERSETPROPERTY)		(AudioConverterRef, AudioConverterPropertyID, UInt32, const void *);
	typedef OSStatus			(*AUDIOCONVERTERFILLCOMPLEXBUFFER)	(AudioConverterRef, AudioConverterComplexInputDataProc, void *, UInt32 *, AudioBufferList *, AudioStreamPacketDescription *);

	typedef OSStatus			(*AUDIOFORMATGETPROPERTY)		(AudioFormatPropertyID, UInt32, const void *, UInt32 *, void *);
	typedef OSStatus			(*AUDIOFORMATGETPROPERTYINFO)		(AudioFormatPropertyID, UInt32, const void *, UInt32 *);

	extern AUDIOFILEINITIALIZEWITHCALLBACKS	 AudioFileInitializeWithCallbacks;
	extern AUDIOFILECLOSE			 AudioFileClose;
	extern AUDIOFILESETPROPERTY		 AudioFileSetProperty;
	extern AUDIOFILEWRITEPACKETS		 AudioFileWritePackets;

	extern AUDIOCONVERTERNEW		 AudioConverterNew;
	extern AUDIOCONVERTERDISPOSE		 AudioConverterDispose;
	extern AUDIOCONVERTERGETPROPERTY	 AudioConverterGetProperty;
	extern AUDIOCONVERTERGETPROPERTYINFO	 AudioConverterGetPropertyInfo;
	extern AUDIOCONVERTERSETPROPERTY	 AudioConverterSetProperty;
	extern AUDIOCONVERTERFILLCOMPLEXBUFFER	 AudioConverterFillComplexBuffer;

	extern AUDIOFORMATGETPROPERTY		 AudioFormatGetProperty;
	extern AUDIOFORMATGETPROPERTYINFO	 AudioFormatGetPropertyInfo;
};
