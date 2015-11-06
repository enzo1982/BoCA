 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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
#include <math.h>

namespace CA
{
#	include <CoreServices/CoreServices.h>
#	include <AudioUnit/AudioUnit.h>
};

#include "coreaudio.h"

const String &BoCA::OutputCoreAudio::GetComponentSpecs()
{
	static String	 componentSpecs;

	/* See if we can find an output audio unit.
	 */
	CA::ComponentDescription	 description;

	description.componentType	  = CA::kAudioUnitType_Output;
	description.componentSubType	  = CA::kAudioUnitSubType_DefaultOutput;
	description.componentManufacturer = CA::kAudioUnitManufacturer_Apple;
	description.componentFlags	  = 0;
	description.componentFlagsMask	  = 0;

	if (CA::FindNextComponent(NIL, &description) != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>CoreAudio Output Plugin</name>	\
		    <version>1.0</version>			\
		    <id>coreaudio-out</id>			\
		    <type>output</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

namespace BoCA
{
	CA::OSStatus	 AudioCallback(void *, CA::AudioUnitRenderActionFlags *, const CA::AudioTimeStamp *, CA::UInt32, CA::UInt32, CA::AudioBufferList *);
};

BoCA::OutputCoreAudio::OutputCoreAudio()
{
	audioUnit = NIL;
	paused	  = False;

	samplesBufferMutex = new Mutex();
}

BoCA::OutputCoreAudio::~OutputCoreAudio()
{
	delete samplesBufferMutex;
}

Bool BoCA::OutputCoreAudio::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format	= track.GetFormat();

	/* Find default output audio unit.
	 */
	CA::ComponentDescription	 description;
	CA::Component			 component;

	description.componentType	  = CA::kAudioUnitType_Output;
	description.componentSubType	  = CA::kAudioUnitSubType_DefaultOutput;
	description.componentManufacturer = CA::kAudioUnitManufacturer_Apple;
	description.componentFlags	  = 0;
	description.componentFlagsMask	  = 0;

	component = CA::FindNextComponent(NIL, &description);

	if (component == NIL) return False;

	/* Initialize audio unit.
	 */
	if (CA::OpenAComponent(component, &audioUnit) != 0) return False;
	if (CA::AudioUnitInitialize(audioUnit) != 0) return False;

	/* Set stream format.
	 */
	CA::AudioStreamBasicDescription	 streamFormat;

	streamFormat.mFormatID		= CA::kAudioFormatLinearPCM;
	streamFormat.mFormatFlags       = CA::kLinearPCMFormatFlagIsPacked | (format.bits > 8	      ? CA::kLinearPCMFormatFlagIsSignedInteger : 0) |
					 				     (endianness == EndianBig ? CA::kLinearPCMFormatFlagIsBigEndian	: 0);
	streamFormat.mSampleRate	= format.rate;
	streamFormat.mChannelsPerFrame	= format.channels;
	streamFormat.mBitsPerChannel	= format.bits;
	streamFormat.mFramesPerPacket	= 1;
	streamFormat.mBytesPerFrame	= streamFormat.mChannelsPerFrame * streamFormat.mBitsPerChannel / 8;
	streamFormat.mBytesPerPacket	= streamFormat.mFramesPerPacket * streamFormat.mBytesPerFrame;

	if (CA::AudioUnitSetProperty(audioUnit, CA::kAudioUnitProperty_StreamFormat, CA::kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat)) != 0) return False;

	/* Set audio callback.
	 */
	CA::AURenderCallbackStruct	 audioCallback;

	audioCallback.inputProc		= AudioCallback;
	audioCallback.inputProcRefCon	= this;

	if (CA::AudioUnitSetProperty(audioUnit, CA::kAudioUnitProperty_SetRenderCallback, CA::kAudioUnitScope_Input, 0, &audioCallback, sizeof(audioCallback)) != 0) return False;

	/* Preallocate sample buffer.
	 */
	samplesBuffer.Resize((format.rate / 4 + 2048) * format.channels * (format.bits / 8));
	samplesBuffer.Resize(0);

	/* Start audio unit.
	 */
	if (CA::AudioOutputUnitStart(audioUnit) != 0) return False;

	paused = False;

	return True;
}

Bool BoCA::OutputCoreAudio::Deactivate()
{
	/* Stop audio unit.
	 */
	if (CA::AudioOutputUnitStop(audioUnit) != 0) return False;

	/* Remove audio callback.
	 */
	struct CA::AURenderCallbackStruct	 audioCallback;

	audioCallback.inputProc		= 0;
	audioCallback.inputProcRefCon	= 0;

	if (CA::AudioUnitSetProperty(audioUnit, CA::kAudioUnitProperty_SetRenderCallback, CA::kAudioUnitScope_Input, 0, &audioCallback, sizeof(audioCallback)) != 0) return False;

	if (CA::CloseComponent(audioUnit) != 0) return False;

	audioUnit = NIL;

	return True;
}

Int BoCA::OutputCoreAudio::WriteData(Buffer<UnsignedByte> &data)
{
	samplesBufferMutex->Lock();

	Int	 bytes = samplesBuffer.Size();

	samplesBuffer.Resize(bytes + data.Size());

	memcpy(samplesBuffer + bytes, data, data.Size());

	samplesBufferMutex->Release();

	return data.Size();
}

Int BoCA::OutputCoreAudio::CanWrite()
{
	if (audioUnit == NIL || paused) return 0;

	const Format		&format = track.GetFormat();

	samplesBufferMutex->Lock();

	Int	 canWrite = (format.rate / 4 + 2048) * format.channels * (format.bits / 8) - samplesBuffer.Size();

	samplesBufferMutex->Release();

	return canWrite;
}

Int BoCA::OutputCoreAudio::SetPause(Bool pause)
{
	if (audioUnit == NIL) return Error();

	CA::OSStatus	 status = 0;

	if (pause) status = CA::AudioOutputUnitStop(audioUnit);
	else	   status = CA::AudioOutputUnitStart(audioUnit);

	if (status != 0) return Error();

	if (pause) paused = True;
	else	   paused = False;

	return Success();
}

Bool BoCA::OutputCoreAudio::IsPlaying()
{
	if (audioUnit == NIL) return False;

	samplesBufferMutex->Lock();

	Bool	 isPlaying = (samplesBuffer.Size() > 0);

	samplesBufferMutex->Release();

	return isPlaying;
}

CA::OSStatus BoCA::AudioCallback(void *inClientData, CA::AudioUnitRenderActionFlags *ioActionFlags, const CA::AudioTimeStamp *inTimeStamp, CA::UInt32 inBusNumber, CA::UInt32 inNumberFrames, CA::AudioBufferList *ioData)
{
	OutputCoreAudio	*component = (OutputCoreAudio *) inClientData;

	component->samplesBufferMutex->Lock();

	Int	 numBytes = Math::Min(component->samplesBuffer.Size(), ioData->mBuffers[0].mDataByteSize);

	/* Copy samples to output buffer.
	 */
	memcpy(ioData->mBuffers[0].mData, component->samplesBuffer, numBytes);

	ioData->mBuffers[0].mDataByteSize = numBytes;
	ioData->mNumberBuffers = 1;

	/* Move remaining samples to the start of the buffer.
	 */
	memmove(component->samplesBuffer, component->samplesBuffer + numBytes, component->samplesBuffer.Size() - numBytes);

	component->samplesBuffer.Resize(component->samplesBuffer.Size() - numBytes);

	component->samplesBufferMutex->Release();

	return CA::kAudioHardwareNoError;
}
