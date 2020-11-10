 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

#include <wchar.h>
#include <shlobj.h>

#ifdef __WINE__
#	define wchar_t char
#	define wstring(s) s

#	define wcslen(x)    strlen(x)
#	define wcscpy(x, y) strcpy(x, y)
#	define wcscat(x, y) strcat(x, y)

#	define GetEnvironmentVariableW GetEnvironmentVariableA
#	define SetEnvironmentVariableW SetEnvironmentVariableA

#	define SHGetPathFromIDListW SHGetPathFromIDListA

#	define SetDllDirectoryW SetDllDirectoryA
#	define LoadLibraryW LoadLibraryA

#	define FindFirstFileW FindFirstFileA

#	define WIN32_FIND_DATAW WIN32_FIND_DATAA
#else
#	define wstring(s) L##s
#endif

namespace CA
{
	AUDIOFILEINITIALIZEWITHCALLBACKS	 AudioFileInitializeWithCallbacks	= NULL;
	AUDIOFILECLOSE				 AudioFileClose				= NULL;
	AUDIOFILESETPROPERTY			 AudioFileSetProperty			= NULL;
	AUDIOFILEWRITEPACKETS			 AudioFileWritePackets			= NULL;

	AUDIOCONVERTERNEW			 AudioConverterNew			= NULL;
	AUDIOCONVERTERDISPOSE			 AudioConverterDispose			= NULL;
	AUDIOCONVERTERGETPROPERTY		 AudioConverterGetProperty		= NULL;
	AUDIOCONVERTERGETPROPERTYINFO		 AudioConverterGetPropertyInfo		= NULL;
	AUDIOCONVERTERSETPROPERTY		 AudioConverterSetProperty		= NULL;
	AUDIOCONVERTERFILLCOMPLEXBUFFER		 AudioConverterFillComplexBuffer	= NULL;

	AUDIOFORMATGETPROPERTY			 AudioFormatGetProperty			= NULL;
	AUDIOFORMATGETPROPERTYINFO		 AudioFormatGetPropertyInfo		= NULL;
};

using namespace CA;

HINSTANCE  coreaudiodll	= NULL;

static const wchar_t *GetSystemDirectory(int id)
{
	static wchar_t	 commonFilesDir[32768] = { 0 };

	ITEMIDLIST	*idlist;

	SHGetSpecialFolderLocation(NULL, id, &idlist);
	SHGetPathFromIDListW(idlist, commonFilesDir);

	CoTaskMemFree(idlist);

	if (wcslen(commonFilesDir) > 0 && commonFilesDir[wcslen(commonFilesDir) - 1] != '\\') wcscat(commonFilesDir, wstring("\\"));

	return commonFilesDir;
}

bool LoadCoreAudioDLL()
{
	wchar_t	 aasDir[32768];

	wcscpy(aasDir, GetSystemDirectory(CSIDL_PROGRAM_FILES_COMMON));
	wcscat(aasDir, wstring("Apple\\Apple Application Support"));

	WIN32_FIND_DATAW	 findData;
	HANDLE			 handle = FindFirstFileW(aasDir, &findData);

	if (handle == INVALID_HANDLE_VALUE)
	{
		wcscpy(aasDir, GetSystemDirectory(CSIDL_PROGRAM_FILES));
		wcscat(aasDir, wstring("iTunes"));
	}

	FindClose(handle);

	/* Add Apple Application Services directory to DLL search path.
	 */
	SetDllDirectoryW(aasDir);

	coreaudiodll	  = LoadLibraryW(wcscat(aasDir, wstring("\\CoreAudioToolbox.dll")));

	if (coreaudiodll == NULL) return false;

	AudioFileInitializeWithCallbacks	= (AUDIOFILEINITIALIZEWITHCALLBACKS) GetProcAddress(coreaudiodll, "AudioFileInitializeWithCallbacks");
	AudioFileClose				= (AUDIOFILECLOSE) GetProcAddress(coreaudiodll, "AudioFileClose");
	AudioFileSetProperty			= (AUDIOFILESETPROPERTY) GetProcAddress(coreaudiodll, "AudioFileSetProperty");
	AudioFileWritePackets			= (AUDIOFILEWRITEPACKETS) GetProcAddress(coreaudiodll, "AudioFileWritePackets");

	AudioConverterNew			= (AUDIOCONVERTERNEW) GetProcAddress(coreaudiodll, "AudioConverterNew");
	AudioConverterDispose			= (AUDIOCONVERTERDISPOSE) GetProcAddress(coreaudiodll, "AudioConverterDispose");
	AudioConverterGetProperty		= (AUDIOCONVERTERGETPROPERTY) GetProcAddress(coreaudiodll, "AudioConverterGetProperty");
	AudioConverterGetPropertyInfo		= (AUDIOCONVERTERGETPROPERTYINFO) GetProcAddress(coreaudiodll, "AudioConverterGetPropertyInfo");
	AudioConverterSetProperty		= (AUDIOCONVERTERSETPROPERTY) GetProcAddress(coreaudiodll, "AudioConverterSetProperty");
	AudioConverterFillComplexBuffer		= (AUDIOCONVERTERFILLCOMPLEXBUFFER) GetProcAddress(coreaudiodll, "AudioConverterFillComplexBuffer");

	AudioFormatGetProperty			= (AUDIOFORMATGETPROPERTY) GetProcAddress(coreaudiodll, "AudioFormatGetProperty");
	AudioFormatGetPropertyInfo		= (AUDIOFORMATGETPROPERTYINFO) GetProcAddress(coreaudiodll, "AudioFormatGetPropertyInfo");

	if (AudioFileInitializeWithCallbacks	== NULL ||
	    AudioFileClose			== NULL ||
	    AudioFileSetProperty		== NULL ||
	    AudioFileWritePackets		== NULL ||

	    AudioConverterNew			== NULL ||
	    AudioConverterDispose		== NULL ||
	    AudioConverterGetProperty		== NULL ||
	    AudioConverterGetPropertyInfo	== NULL ||
	    AudioConverterSetProperty		== NULL ||
	    AudioConverterFillComplexBuffer	== NULL ||

	    AudioFormatGetProperty		== NULL ||
	    AudioFormatGetPropertyInfo		== NULL) { FreeCoreAudioDLL(); return false; }

	return true;
}

void FreeCoreAudioDLL()
{
	FreeLibrary(coreaudiodll);

	coreaudiodll	  = NULL;
}
