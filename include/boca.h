 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_BOCA
#define H_BOCA_BOCA

#include <smooth.h>

using namespace smooth;

#if defined BOCA_COMPONENT_BUILD
# ifdef __WIN32__
#  define BOCA_EXPORT __declspec (dllexport)
# else
#  define BOCA_EXPORT __attribute__ ((visibility ("default")))
# endif

# include "boca/component/component.h"
# include "boca/component/decodercomponent.h"
# include "boca/component/encodercomponent.h"
# include "boca/component/deviceinfocomponent.h"
# include "boca/component/dspcomponent.h"
# include "boca/component/extensioncomponent.h"
# include "boca/component/outputcomponent.h"
# include "boca/component/playlistcomponent.h"
# include "boca/component/taggercomponent.h"
#else
# ifdef __WIN32__
#  define BOCA_EXPORT __declspec (dllimport)
# else
#  define BOCA_EXPORT __attribute__ ((visibility ("default")))
# endif

# include "boca/core/core.h"
#endif

#include "boca/application/component.h"
#include "boca/application/componentspecs.h"
#include "boca/application/decodercomponent.h"
#include "boca/application/encodercomponent.h"
#include "boca/application/deviceinfocomponent.h"
#include "boca/application/dspcomponent.h"
#include "boca/application/extensioncomponent.h"
#include "boca/application/outputcomponent.h"
#include "boca/application/playlistcomponent.h"
#include "boca/application/taggercomponent.h"
#include "boca/application/registry.h"

#include "boca/common/config.h"
#include "boca/common/configlayer.h"
#include "boca/common/i18n.h"
#include "boca/common/protocol.h"
#include "boca/common/utilities.h"

#include "boca/common/metadata/device.h"
#include "boca/common/metadata/mcdi.h"
#include "boca/common/metadata/track.h"

#include "boca/common/communication/application.h"
#include "boca/common/communication/joblist.h"
#include "boca/common/communication/menu.h"
#include "boca/common/communication/settings.h"

#define BoCA_BEGIN_COMPONENT(componentName)																													\
	extern "C" {																																\
		BOCA_EXPORT const char *BoCA_GetComponentName()										{ return (const char *) #componentName; }												\
	}

#define BoCA_DEFINE_DECODER_COMPONENT(componentName)										 																		\
	extern "C" {																																\
		BOCA_EXPORT bool BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)				{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }					\
		BOCA_EXPORT bool BoCA_##componentName##_CanOpenStream(void *component, const wchar_t *file)				{ return ((BoCA::componentName *) component)->CanOpenStream(file); }									\
		BOCA_EXPORT int BoCA_##componentName##_GetStreamInfo(void *component, const wchar_t *file, void *track)			{ return ((BoCA::componentName *) component)->GetStreamInfo(file, *((BoCA::Track *) track)); }						\
																																		\
		BOCA_EXPORT __int64 BoCA_##componentName##_GetInBytes(const void *component)						{ return ((const BoCA::componentName *) component)->GetInBytes(); }									\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)							{ return ((BoCA::componentName *) component)->Activate(); }										\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)							{ return ((BoCA::componentName *) component)->Deactivate(); }										\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_Seek(void *component, __int64 samplePosition)					{ return ((BoCA::componentName *) component)->Seek(samplePosition); }									\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_ReadData(void *component, void *buffer, int size)				{ return ((BoCA::componentName *) component)->ReadData(*((Buffer<UnsignedByte> *) buffer), size); }					\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)							{ return ((BoCA::componentName *) component)->GetPackageSize(); }									\
		BOCA_EXPORT int BoCA_##componentName##_SetDriver(void *component, void *driver)						{ return ((BoCA::componentName *) component)->SetDriver((IO::Driver *) driver); }							\
	}

#define BoCA_DEFINE_ENCODER_COMPONENT(componentName)											 																	\
	extern "C" {																																\
		BOCA_EXPORT bool BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)				{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }					\
																																		\
		BOCA_EXPORT char *BoCA_##componentName##_GetOutputFileExtension(void *component)					{ return ((BoCA::componentName *) component)->GetOutputFileExtension(); }								\
		BOCA_EXPORT int BoCA_##componentName##_GetNumberOfPasses(void *component)						{ return ((BoCA::componentName *) component)->GetNumberOfPasses(); }									\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)							{ return ((BoCA::componentName *) component)->Activate(); }										\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)							{ return ((BoCA::componentName *) component)->Deactivate(); }										\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_WriteData(void *component, void *buffer, int size)				{ return ((BoCA::componentName *) component)->WriteData(*((Buffer<UnsignedByte> *) buffer), size); }					\
		BOCA_EXPORT bool BoCA_##componentName##_NextPass(void *component)							{ return ((BoCA::componentName *) component)->NextPass(); }										\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)							{ return ((BoCA::componentName *) component)->GetPackageSize(); }									\
		BOCA_EXPORT int BoCA_##componentName##_SetDriver(void *component, void *driver)						{ return ((BoCA::componentName *) component)->SetDriver((IO::Driver *) driver); }							\
	}

#define BoCA_DEFINE_DEVICEINFO_COMPONENT(componentName)											 																	\
	extern "C" {																																\
		BOCA_EXPORT int BoCA_##componentName##_GetNumberOfDevices(void *component)						{ return ((BoCA::componentName *) component)->GetNumberOfDevices(); }									\
		BOCA_EXPORT const void *BoCA_##componentName##_GetNthDeviceInfo(void *component, int n)					{ return &((BoCA::componentName *) component)->GetNthDeviceInfo(n); }									\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_OpenNthDeviceTray(void *component, int n)					{ return ((BoCA::componentName *) component)->OpenNthDeviceTray(n); }									\
		BOCA_EXPORT bool BoCA_##componentName##_CloseNthDeviceTray(void *component, int n)					{ return ((BoCA::componentName *) component)->CloseNthDeviceTray(n); }									\
																																		\
		BOCA_EXPORT const void *BoCA_##componentName##_GetNthDeviceTrackList(void *component, int n)				{ return &((BoCA::componentName *) component)->GetNthDeviceTrackList(n); }								\
		BOCA_EXPORT const void *BoCA_##componentName##_GetNthDeviceMCDI(void *component, int n)					{ return &((BoCA::componentName *) component)->GetNthDeviceMCDI(n); }									\
	}

#define BoCA_DEFINE_DSP_COMPONENT(componentName)											 																	\
	extern "C" {																																\
		BOCA_EXPORT bool BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)				{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }					\
		BOCA_EXPORT void BoCA_##componentName##_GetFormatInfo(void *component, void *format)					{ return ((BoCA::componentName *) component)->GetFormatInfo(*((BoCA::Format *) format)); }						\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)							{ return ((BoCA::componentName *) component)->Activate(); }										\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)							{ return ((BoCA::componentName *) component)->Deactivate(); }										\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_TransformData(void *component, void *buffer, int size)				{ return ((BoCA::componentName *) component)->TransformData(*((Buffer<UnsignedByte> *) buffer), size); }				\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_Flush(void *component, void *buffer)						{ return ((BoCA::componentName *) component)->Flush(*((Buffer<UnsignedByte> *) buffer)); }						\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)							{ return ((BoCA::componentName *) component)->GetPackageSize(); }									\
	}

#define BoCA_DEFINE_EXTENSION_COMPONENT(componentName)										 																		\
	extern "C" {																																\
		BOCA_EXPORT void *BoCA_##componentName##_GetMainTabLayer(void *component)						{ return ((BoCA::componentName *) component)->getMainTabLayer.Emit(); }									\
		BOCA_EXPORT void *BoCA_##componentName##_GetStatusBarLayer(void *component)						{ return ((BoCA::componentName *) component)->getStatusBarLayer.Emit(); }								\
	}

#define BoCA_DEFINE_OUTPUT_COMPONENT(componentName)											 																	\
	extern "C" {																																\
		BOCA_EXPORT bool BoCA_##componentName##_SetAudioTrackInfo(void *component, const void *track)				{ return ((BoCA::componentName *) component)->SetAudioTrackInfo(*((const BoCA::Track *) track)); }					\
																																		\
		BOCA_EXPORT bool BoCA_##componentName##_Activate(void *component)							{ return ((BoCA::componentName *) component)->Activate(); }										\
		BOCA_EXPORT bool BoCA_##componentName##_Deactivate(void *component)							{ return ((BoCA::componentName *) component)->Deactivate(); }										\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_WriteData(void *component, void *buffer, int size)				{ return ((BoCA::componentName *) component)->WriteData(*((Buffer<UnsignedByte> *) buffer), size); }					\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_GetPackageSize(void *component)							{ return ((BoCA::componentName *) component)->GetPackageSize(); }									\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_CanWrite(void *component)							{ return ((BoCA::componentName *) component)->CanWrite(); }										\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_SetPause(void *component, bool pause)						{ return ((BoCA::componentName *) component)->SetPause(pause); }									\
		BOCA_EXPORT bool BoCA_##componentName##_IsPlaying(void *component)							{ return ((BoCA::componentName *) component)->IsPlaying(); }										\
	}

#define BoCA_DEFINE_PLAYLIST_COMPONENT(componentName)										 																		\
	extern "C" {																																\
		BOCA_EXPORT void BoCA_##componentName##_SetTrackList(void *component, const void *trackList)				{ return ((BoCA::componentName *) component)->SetTrackList(*((const Array<BoCA::Track> *) trackList)); }				\
		BOCA_EXPORT bool BoCA_##componentName##_CanOpenFile(void *component, const wchar_t *file)				{ return ((BoCA::componentName *) component)->CanOpenFile(file); }									\
																																		\
		BOCA_EXPORT const void *BoCA_##componentName##_ReadPlaylist(void *component, const wchar_t *file)			{ return &((BoCA::componentName *) component)->ReadPlaylist(file); }									\
		BOCA_EXPORT int BoCA_##componentName##_WritePlaylist(void *component, const wchar_t *file)				{ return ((BoCA::componentName *) component)->WritePlaylist(file); }									\
	}

#define BoCA_DEFINE_TAGGER_COMPONENT(componentName)										 																		\
	extern "C" {																																\
		BOCA_EXPORT void BoCA_##componentName##_SetVendorString(void *component, const wchar_t *vendor)				{ return ((BoCA::componentName *) component)->SetVendorString(vendor); }								\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_ParseBuffer(void *component, const void *buffer, void *track)			{ return ((BoCA::componentName *) component)->ParseBuffer(*((const Buffer<UnsignedByte> *) buffer), *((BoCA::Track *) track)); }	\
		BOCA_EXPORT int BoCA_##componentName##_ParseStreamInfo(void *component, const wchar_t *file, void *track)		{ return ((BoCA::componentName *) component)->ParseStreamInfo(file, *((BoCA::Track *) track)); }					\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_RenderBuffer(void *component, void *buffer, const void *track)			{ return ((BoCA::componentName *) component)->RenderBuffer(*((Buffer<UnsignedByte> *) buffer), *((const BoCA::Track *) track)); }	\
		BOCA_EXPORT int BoCA_##componentName##_RenderStreamInfo(void *component, const wchar_t *file, const void *track)	{ return ((BoCA::componentName *) component)->RenderStreamInfo(file, *((const BoCA::Track *) track)); }					\
																																		\
		BOCA_EXPORT int BoCA_##componentName##_UpdateStreamInfo(void *component, const wchar_t *file, const void *track)	{ return ((BoCA::componentName *) component)->UpdateStreamInfo(file, *((const BoCA::Track *) track)); }					\
	}

#define BoCA_END_COMPONENT(componentName)										 																			\
	extern "C" {																																\
		BOCA_EXPORT const char *BoCA_##componentName##_GetComponentSpecs()							{ return BoCA::componentName::GetComponentSpecs(); }											\
		BOCA_EXPORT void *BoCA_##componentName##_Create()									{ return new BoCA::componentName(); }													\
		BOCA_EXPORT bool BoCA_##componentName##_Delete(void *component)								{ delete ((BoCA::componentName *) component); return True; }										\
		BOCA_EXPORT void *BoCA_##componentName##_GetConfigurationLayer(void *component)						{ return ((BoCA::componentName *) component)->GetConfigurationLayer(); }								\
		BOCA_EXPORT bool BoCA_##componentName##_GetErrorState(const void *component)						{ return ((const BoCA::componentName *) component)->GetErrorState(); }									\
		BOCA_EXPORT const void *BoCA_##componentName##_GetErrorString(const void *component)					{ return &((const BoCA::componentName *) component)->GetErrorString(); }								\
	}

#endif
