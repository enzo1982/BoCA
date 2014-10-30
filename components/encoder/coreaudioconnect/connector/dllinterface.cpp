 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

#include <shlobj.h>

namespace CA
{
	CFSTRINGCREATEWITHCSTRING	 CFStringCreateWithCString		= NULL;
	CFURLCREATEWITHFILESYSTEMPATH	 CFURLCreateWithFileSystemPath		= NULL;
	CFRELEASE			 CFRelease				= NULL;

	AUDIOFILECREATEWITHURL		 AudioFileCreateWithURL			= NULL;
	AUDIOFILECLOSE			 AudioFileClose				= NULL;
	AUDIOFILESETPROPERTY		 AudioFileSetProperty			= NULL;
	AUDIOFILEWRITEPACKETS		 AudioFileWritePackets			= NULL;

	AUDIOCONVERTERNEW		 AudioConverterNew			= NULL;
	AUDIOCONVERTERDISPOSE		 AudioConverterDispose			= NULL;
	AUDIOCONVERTERGETPROPERTY	 AudioConverterGetProperty		= NULL;
	AUDIOCONVERTERGETPROPERTYINFO	 AudioConverterGetPropertyInfo		= NULL;
	AUDIOCONVERTERSETPROPERTY	 AudioConverterSetProperty		= NULL;
	AUDIOCONVERTERFILLCOMPLEXBUFFER	 AudioConverterFillComplexBuffer	= NULL;

	AUDIOFORMATGETPROPERTY		 AudioFormatGetProperty			= NULL;
	AUDIOFORMATGETPROPERTYINFO	 AudioFormatGetPropertyInfo		= NULL;
};

using namespace CA;

HINSTANCE  corefoundationdll	= NULL;
HINSTANCE  coreaudiodll		= NULL;

const wchar_t *GetCommonFilesDirectory()
{
	static wchar_t	 commonFilesDir[32768] = { 0 };

	if (commonFilesDir[0] != 0) return commonFilesDir;

	ITEMIDLIST	*idlist;

	SHGetSpecialFolderLocation(NULL, CSIDL_PROGRAM_FILES_COMMON, &idlist);
	SHGetPathFromIDList(idlist, commonFilesDir);

	CoTaskMemFree(idlist);

	if (wcslen(commonFilesDir) > 0 && commonFilesDir[wcslen(commonFilesDir) - 1] != '\\') wcscat(commonFilesDir, L"\\");

	return commonFilesDir;
}

bool LoadCoreFoundationDLL()
{
	wchar_t	 aasDir[32768];

	wcscpy(aasDir, GetCommonFilesDirectory());
	wcscat(aasDir, L"Apple\\Apple Application Support\\");

	/* Add Apple Application Services directory to path.
	 */
	wchar_t	 buffer[32768];

	GetEnvironmentVariableW(L"PATH", buffer, 32768);
	SetEnvironmentVariableW(L"PATH", wcscat(wcscat(buffer, L";"), aasDir));

	corefoundationdll = LoadLibraryW(wcscat(aasDir, L"CoreFoundation.dll"));

	if (corefoundationdll == NULL) return false;

	CFStringCreateWithCString	= (CFSTRINGCREATEWITHCSTRING) GetProcAddress(corefoundationdll, "CFStringCreateWithCString");
	CFURLCreateWithFileSystemPath	= (CFURLCREATEWITHFILESYSTEMPATH) GetProcAddress(corefoundationdll, "CFURLCreateWithFileSystemPath");
	CFRelease			= (CFRELEASE) GetProcAddress(corefoundationdll, "CFRelease");

	if (CFStringCreateWithCString		== NULL ||
	    CFURLCreateWithFileSystemPath	== NULL ||
	    CFRelease				== NULL) { FreeCoreFoundationDLL(); return false; }

	return true;
}

void FreeCoreFoundationDLL()
{
	FreeLibrary(corefoundationdll);

	corefoundationdll = NULL;
}

bool LoadCoreAudioDLL()
{
	wchar_t	 aasDir[32768];

	wcscpy(aasDir, GetCommonFilesDirectory());
	wcscat(aasDir, L"Apple\\Apple Application Support\\");

	/* Add Apple Application Services directory to path.
	 */
	wchar_t	 buffer[32768];

	GetEnvironmentVariableW(L"PATH", buffer, 32768);
	SetEnvironmentVariableW(L"PATH", wcscat(wcscat(buffer, L";"), aasDir));

	coreaudiodll	  = LoadLibrary(wcscat(aasDir, L"CoreAudioToolbox.dll"));

	if (coreaudiodll == NULL) return false;

	AudioFileCreateWithURL		= (AUDIOFILECREATEWITHURL) GetProcAddress(coreaudiodll, "AudioFileCreateWithURL");
	AudioFileClose			= (AUDIOFILECLOSE) GetProcAddress(coreaudiodll, "AudioFileClose");
	AudioFileSetProperty		= (AUDIOFILESETPROPERTY) GetProcAddress(coreaudiodll, "AudioFileSetProperty");
	AudioFileWritePackets		= (AUDIOFILEWRITEPACKETS) GetProcAddress(coreaudiodll, "AudioFileWritePackets");

	AudioConverterNew		= (AUDIOCONVERTERNEW) GetProcAddress(coreaudiodll, "AudioConverterNew");
	AudioConverterDispose		= (AUDIOCONVERTERDISPOSE) GetProcAddress(coreaudiodll, "AudioConverterDispose");
	AudioConverterGetProperty	= (AUDIOCONVERTERGETPROPERTY) GetProcAddress(coreaudiodll, "AudioConverterGetProperty");
	AudioConverterGetPropertyInfo	= (AUDIOCONVERTERGETPROPERTYINFO) GetProcAddress(coreaudiodll, "AudioConverterGetPropertyInfo");
	AudioConverterSetProperty	= (AUDIOCONVERTERSETPROPERTY) GetProcAddress(coreaudiodll, "AudioConverterSetProperty");
	AudioConverterFillComplexBuffer	= (AUDIOCONVERTERFILLCOMPLEXBUFFER) GetProcAddress(coreaudiodll, "AudioConverterFillComplexBuffer");

	AudioFormatGetProperty		= (AUDIOFORMATGETPROPERTY) GetProcAddress(coreaudiodll, "AudioFormatGetProperty");
	AudioFormatGetPropertyInfo	= (AUDIOFORMATGETPROPERTYINFO) GetProcAddress(coreaudiodll, "AudioFormatGetPropertyInfo");

	if (AudioFileCreateWithURL		== NULL ||
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
