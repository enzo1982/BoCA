 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include <stdio.h>
#include <wchar.h>
#include <shlobj.h>

#ifdef __WINE__
#	include <unistd.h>
#	include <pwd.h>
#	include <sys/stat.h>

#	define wchar_t char
#	define wstring(s) s

#	define wcslen(x) strlen(x)
#	define wcscpy(x, y) strcpy(x, y)
#	define wcsncpy(x, y, z) strncpy(x, y, z)
#	define wcscat(x, y) strcat(x, y)
#	define wcscmp(x, y) strcmp(x, y)
#	define wcsncmp(x, y, z) strncmp(x, y, z)
#	define wcsrchr(x, y) strrchr(x, y)
#	define wcsstr(x, y) strstr(x, y)

#	define _wfopen(x, y) fopen(x, y)

#	define _wmkdir(x) mkdir(x, 0777)

#	define GetEnvironmentVariableW GetEnvironmentVariableA
#	define SetEnvironmentVariableW SetEnvironmentVariableA

#	define SHGetPathFromIDListW SHGetPathFromIDListA

#	define SetDllDirectoryW SetDllDirectoryA
#	define LoadLibraryW LoadLibraryA

#	define FindFirstFileW FindFirstFileA
#	define FindNextFileW FindNextFileA
#	define DeleteFileW DeleteFileA

#	define RegOpenKeyW RegOpenKeyA
#	define RegEnumKeyW RegEnumKeyA
#	define RegQueryValueExW RegQueryValueExA

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

#define PATH_LENGTH 2048

static bool FileSystemEntryExists(const wchar_t *path)
{
	WIN32_FIND_DATAW	 findData;
	HANDLE			 handle = FindFirstFileW(path, &findData);

	FindClose(handle);

	return (handle != INVALID_HANDLE_VALUE);
}

static const wchar_t *GetSystemDirectory(int id)
{
	static wchar_t	 commonFilesDir[PATH_LENGTH] = { 0 };

	ITEMIDLIST	*idlist;

	SHGetSpecialFolderLocation(NULL, id, &idlist);
	SHGetPathFromIDListW(idlist, commonFilesDir);

	CoTaskMemFree(idlist);

	if (wcslen(commonFilesDir) > 0 && commonFilesDir[wcslen(commonFilesDir) - 1] != '\\') wcscat(commonFilesDir, wstring("\\"));

	return commonFilesDir;
}

static const wchar_t *GetCacheFolder(const char *applicationPrefix)
{
	static wchar_t	 cacheFolder[PATH_LENGTH] = { 0 };

#ifdef __WINE__
	char	*cacheHome = getenv("XDG_CACHE_HOME");

	if (cacheHome == NULL)
	{
		passwd	*pw = getpwuid(getuid());

		if (pw != NULL)	strcpy(cacheFolder, pw->pw_dir);
		else		strcpy(cacheFolder, "~");

		strcat(cacheFolder, "/.cache/");
	}
	else
	{
		strcpy(cacheFolder, cacheHome);

		if (cacheFolder[strlen(cacheFolder) - 1] != '/') strcat(cacheFolder, "/");
	}

	strcat(cacheFolder, applicationPrefix);
	strcat(cacheFolder, "/");
#else
	GetModuleFileName(NULL, cacheFolder, PATH_LENGTH - 1);

	if (cacheFolder[0] != 0) wcsrchr(cacheFolder, '\\')[1] = 0;

	/* Don't use program files folder as cache folder.
	 */
	static wchar_t	 programFilesFolder[PATH_LENGTH];

	wcscpy(programFilesFolder, GetSystemDirectory(CSIDL_PROGRAM_FILES));

	if (wcsncmp(cacheFolder, programFilesFolder, wcslen(programFilesFolder)) != 0)
	{
		/* Test if application folder is writable.
		 */
		wcscat(cacheFolder, wstring("\\coreaudioconnect.cache"));

		fclose(_wfopen(cacheFolder, wstring("w")));

		if (FileSystemEntryExists(cacheFolder))
		{
			DeleteFileW(cacheFolder);

			wcsrchr(cacheFolder, '\\')[1] = 0;

			return cacheFolder;
		}
	}

	/* Otherwise use application data folder.
	 */
	wcscpy(cacheFolder, GetSystemDirectory(CSIDL_APPDATA));

	for (unsigned int offset = wcslen(cacheFolder), i = 0; i < strlen(applicationPrefix) + 1; i++) cacheFolder[offset + i] = applicationPrefix[i];

	wcscat(cacheFolder, wstring("\\"));
#endif

	return cacheFolder;
}

static void CreateFolder(const wchar_t *path)
{
	wchar_t	 folder[PATH_LENGTH] = { 0 };

	for (unsigned int i = 0; i < wcslen(path); i++)
	{
		if (path[i] != '\\') continue;

		wcsncpy(folder, path, i);

		_wmkdir(folder);
	}

	_wmkdir(path);
}

static size_t GetFileSize(FILE *file)
{
	fseek(file, 0, SEEK_END);

	size_t	 size = ftell(file);

	fseek(file, 0, SEEK_SET);

	return size;
}

#  if defined __i386__	  || defined _M_IX86
static const wchar_t	*architecture = wstring("x86");
#elif defined __x86_64__  || defined _M_AMD64
static const wchar_t	*architecture = wstring("x64");
#elif defined __arm__	  || defined _M_ARM
static const wchar_t	*architecture = wstring("arm");
#elif defined __aarch64__ || defined _M_ARM64
static const wchar_t	*architecture = wstring("arm64");
#endif

#define NUM_FOLDERS 32

static void GetPackageFolders(const wchar_t *, wchar_t **);

static void FindDependenciesFolders(const wchar_t *packageFolder, wchar_t **packageFolders)
{
	wchar_t	 manifestPath[PATH_LENGTH];

	wcscpy(manifestPath, packageFolder);
	wcscat(manifestPath, wstring("AppxManifest.xml"));

	if (!FileSystemEntryExists(manifestPath)) return;

	/* Load app manifest.
	 */
	FILE	*manifest = _wfopen(manifestPath, wstring("r"));
	size_t	 size	  = GetFileSize(manifest);

	/* Find PackageDependency nodes.
	 */
	char	 line[1024];

	while (ftell(manifest) < (int) size)
	{
		if (!fgets(line, 1024, manifest)) break;
		if (!strstr(line, "<PackageDependency ")) continue;

		wchar_t	 name[256];
		int	 start = (strstr(line, "Name=\"") + 6) - line;

		for (int i = 0; i < 256; i++)
		{
			if (line[start + i] == '\"')
			{
				name[i] = 0;
				break;
			}

			name[i] = line[start + i];
		}

		GetPackageFolders(name, packageFolders);
	}

	/* Close manifest.
	 */
	fclose(manifest);
}

static void GetPackageFolders(const wchar_t *packageName, wchar_t **packageFolders)
{
	/* Open Packages key.
	 */
	HKEY	 packagesKey;

	if (RegOpenKeyW(HKEY_CLASSES_ROOT, wstring("Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppModel\\Repository\\Packages"), &packagesKey) != ERROR_SUCCESS) return;

	/* Construct architecture search string.
	 */
	wchar_t	 arch[16];

	wcscpy(arch, wstring("_"));
	wcscat(arch, architecture);
	wcscat(arch, wstring("_"));

	/* Enumerate registered packages.
	 */
	int	 index = 0;
	wchar_t	 name[256];

	while (RegEnumKeyW(packagesKey, index++, name, sizeof(name) / sizeof(wchar_t)) == ERROR_SUCCESS)
	{
		if (wcsncmp(name, packageName, wcslen(packageName)) != 0) continue;
		if (!wcsstr(name, arch)) continue;

		/* Found package, open key.
		 */
		HKEY	 packageKey;

		if (RegOpenKeyW(packagesKey, name, &packageKey) != ERROR_SUCCESS) continue;

		/* Query folder.
		 */
		const DWORD	 size	= 2048;
		DWORD		 bytes	= size * sizeof(wchar_t);
		wchar_t		*folder	= (wchar_t *) malloc(bytes);

		RegQueryValueExW(packageKey, wstring("PackageRootFolder"), 0, 0, (BYTE *) folder, &bytes);
		RegCloseKey(packageKey);

		/* Add folder and dependencies to list.
		 */
		if (folder[wcslen(folder) - 1] != '\\') wcscat(folder, wstring("\\"));

		for (int i = 0; i < NUM_FOLDERS; i++)
		{
			if (packageFolders[i] != NULL)
			{
				if (wcscmp(packageFolders[i], folder) == 0)
				{
					free(folder);

					break;
				}

				continue;
			}

			packageFolders[i] = folder;

			FindDependenciesFolders(folder, packageFolders);

			break;
		}

		break;
	}

	RegCloseKey(packagesKey);
}

static void CopyLibrary(const wchar_t *libName, wchar_t **packageFolders, const wchar_t *cacheFolder)
{
	/* Find source and target file names.
	 */
	wchar_t	 sourceFile[PATH_LENGTH] = { 0 };
	wchar_t	 targetFile[PATH_LENGTH] = { 0 };

	wcscpy(targetFile, cacheFolder);
	wcscat(targetFile, libName);

	for (int i = 0; i < NUM_FOLDERS; i++)
	{
		if (packageFolders[i] == NULL) break;

		wchar_t	 libFile[PATH_LENGTH];

		wcscpy(libFile, packageFolders[i]);
		wcscat(libFile, libName);

		if (!FileSystemEntryExists(libFile)) continue;

		wcscpy(sourceFile, libFile);
	}

	/* Check file existence and skip already existing files.
	 */
	if (!FileSystemEntryExists(sourceFile) ||
	     FileSystemEntryExists(targetFile)) return;

	/* Prepare input and output files.
	 */
	FILE	*in	= _wfopen(sourceFile, wstring("rb"));
	FILE	*out	= _wfopen(targetFile, wstring("wb"));

	size_t	 inSize = GetFileSize(in);

	/* Copy chunks of 128kB.
	 */
	unsigned char	 buffer[131072];
	int		 bufferSize = sizeof(buffer);

	int		 overlap   = 0;
	bool		 foundSelf = false;

	for (size_t i = 0; i < inSize; i += bufferSize - overlap)
	{
		memcpy(buffer, buffer + bufferSize - overlap, overlap);

		int	 remaining = inSize - ftell(in);
		int	 bytes	   = fread(buffer + overlap, 1, bufferSize - overlap < remaining ? bufferSize - overlap : remaining, in);

		fwrite(buffer + overlap, bytes, 1, out);

		/* Check for referenced DLLs.
		 */
		for (int n = 0; n < overlap + bytes - 5; n++)
		{
			if (buffer[n] != '.' || buffer[n + 1] != 'd' || buffer[n + 2] != 'l' || buffer[n + 3] != 'l' || buffer[n + 4] != 0) continue;

			for (int m = n; m >= 0; m--)
			{
				if (buffer[m] >= 0x20) continue;

				/* Get DLL name and copy it if it looks like a reference.
				 */
				char	*dllName = (char *) buffer + m + 1;
				wchar_t	 dllNameW[256] = { 0 };

				for (unsigned int i = 0; i < strlen(dllName); i++) dllNameW[i] = dllName[i];

				if	(foundSelf)			 CopyLibrary(dllNameW, packageFolders, cacheFolder);
				else if (wcscmp(dllNameW, libName) == 0) foundSelf = true;

				break;
			}
		}

		/* Overlap 128 bytes when wrapping around so we do not miss any relevant DLLs.
		 */
		overlap = 128;
	}

	fclose(out);
	fclose(in);
}

static void CacheCoreAudioLibraries(const wchar_t *cacheFolder)
{
	wchar_t	*iTunesFolders[NUM_FOLDERS] = { NULL };

	GetPackageFolders(wstring("AppleInc.iTunes"), iTunesFolders);

	if (iTunesFolders[0] == NULL) return;

	/* Check if package ID has changed.
	 */
	wchar_t	 packageFolder[PATH_LENGTH];

	wcscpy(packageFolder, iTunesFolders[0]);
	wcsrchr(packageFolder, '\\')[0] = 0;

	wchar_t	*packageIdW = wcsrchr(packageFolder, '\\') + 1;
	char	 packageId[256] = { 0 };

	for (unsigned int i = 0; i < wcslen(packageIdW); i++) packageId[i] = packageIdW[i];

	wchar_t	 packageIdFile[PATH_LENGTH] = { 0 };

	wcscpy(packageIdFile, cacheFolder);
	wcscat(packageIdFile, wstring("PackageId"));

	if (FileSystemEntryExists(packageIdFile))
	{
		FILE	*in = _wfopen(packageIdFile, wstring("r"));
		char	 line[256] = { 0 };

		fgets(line, 256, in);
		fclose(in);

		if (strcmp(line, packageId) == 0) return;
	}

	/* Create cache folder and make sure it's empty.
	 */
	CreateFolder(cacheFolder);

	wchar_t	 pattern[PATH_LENGTH];

	wcscpy(pattern, cacheFolder);
	wcscat(pattern, wstring("*"));

	WIN32_FIND_DATAW	 findData;
	HANDLE			 handle = FindFirstFileW(pattern, &findData);

	do
	{
		if (wcscmp(findData.cFileName, wstring(".")) == 0 || wcscmp(findData.cFileName, wstring("..")) == 0) continue;

		DeleteFileW(findData.cFileName);
	}
	while (FindNextFileW(handle, &findData));

	FindClose(handle);

	/* Recursively copy CoreAudioToolbox.dll and referenced DLLs.
	 */
	CopyLibrary(wstring("CoreAudioToolbox.dll"), iTunesFolders, cacheFolder);

	for (int i = 0; i < NUM_FOLDERS; i++) free(iTunesFolders[i]);

	/* Store package ID.
	 */
	FILE	*out = _wfopen(packageIdFile, wstring("w"));

	fprintf(out, "%s\n", packageId);
	fclose(out);
}

bool LoadCoreAudioDLL(const char *applicationPrefix)
{
	wchar_t	 coreAudioDir[PATH_LENGTH];

	wcscpy(coreAudioDir, GetSystemDirectory(CSIDL_PROGRAM_FILES_COMMON));
	wcscat(coreAudioDir, wstring("Apple\\Apple Application Support\\"));

	if (!FileSystemEntryExists(coreAudioDir))
	{
		wcscpy(coreAudioDir, GetSystemDirectory(CSIDL_PROGRAM_FILES));
		wcscat(coreAudioDir, wstring("iTunes\\"));
	}

	if (!FileSystemEntryExists(coreAudioDir))
	{
		wcscpy(coreAudioDir, GetCacheFolder(applicationPrefix));
		wcscat(coreAudioDir, wstring("boca.encoder.coreaudio\\"));
		wcscat(coreAudioDir, architecture);
		wcscat(coreAudioDir, wstring("\\"));

		CacheCoreAudioLibraries(coreAudioDir);
	}

	/* Add Apple Application Services directory to DLL search path.
	 */
	SetDllDirectoryW(coreAudioDir);

	coreaudiodll	  = LoadLibraryW(wcscat(coreAudioDir, wstring("CoreAudioToolbox.dll")));

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
