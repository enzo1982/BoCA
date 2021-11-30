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

#include <boca.h>
#include "dllinterface.h"

#ifdef __WIN32__
#	include <windows.h>
#	include <shlobj.h>
#endif

using namespace BoCA;

using namespace smooth::IO;
using namespace smooth::XML;

#ifndef __APPLE__
using namespace CA;

namespace CA
{
	AUDIOFILEINITIALIZEWITHCALLBACKS	 AudioFileInitializeWithCallbacks	= NIL;
	AUDIOFILECLOSE				 AudioFileClose				= NIL;
	AUDIOFILESETPROPERTY			 AudioFileSetProperty			= NIL;
	AUDIOFILEWRITEPACKETS			 AudioFileWritePackets			= NIL;

	AUDIOCONVERTERNEW			 AudioConverterNew			= NIL;
	AUDIOCONVERTERDISPOSE			 AudioConverterDispose			= NIL;
	AUDIOCONVERTERGETPROPERTY		 AudioConverterGetProperty		= NIL;
	AUDIOCONVERTERGETPROPERTYINFO		 AudioConverterGetPropertyInfo		= NIL;
	AUDIOCONVERTERSETPROPERTY		 AudioConverterSetProperty		= NIL;
	AUDIOCONVERTERFILLCOMPLEXBUFFER		 AudioConverterFillComplexBuffer	= NIL;

	AUDIOFORMATGETPROPERTY			 AudioFormatGetProperty			= NIL;
	AUDIOFORMATGETPROPERTYINFO		 AudioFormatGetPropertyInfo		= NIL;
};
#endif

MP4OPTIMIZE	 ex_MP4Optimize = NIL;

DynamicLoader *coreaudiodll	= NIL;
DynamicLoader *mp4v2dll		= NIL;

#ifdef __WIN32__
static const String	 coreAudioToolbox = "CoreAudioToolbox.dll";

static String GetSystemDirectory(int id)
{
	String	 commonFilesDir;

	ITEMIDLIST	*idlist;
	Buffer<wchar_t>	 buffer(32768 + 1);

	SHGetSpecialFolderLocation(NIL, id, &idlist);
	SHGetPathFromIDList(idlist, buffer);

	commonFilesDir = buffer;

	CoTaskMemFree(idlist);

	if (commonFilesDir != NIL && !commonFilesDir.EndsWith("\\")) commonFilesDir.Append("\\");

	return commonFilesDir;
}

#  if defined __i386__	  || defined _M_IX86
static const String	 architecture = "x86";
#elif defined __x86_64__  || defined _M_AMD64
static const String	 architecture = "x64";
#elif defined __arm__	  || defined _M_ARM
static const String	 architecture = "arm";
#elif defined __aarch64__ || defined _M_ARM64
static const String	 architecture = "arm64";
#endif

static Void GetPackageFolders(const String &, Array<String> &);

static Void FindDependenciesFolders(const String &packageFolder, Array<String> &packageFolders)
{
	if (!File(packageFolder.Append("AppxManifest.xml")).Exists()) return;

	/* Load app manifest.
	 */
	Document	 document;

	document.LoadFile(packageFolder.Append("AppxManifest.xml"));

	/* Get root node.
	 */
	Node	*root = document.GetRootNode();

	if (root == NIL) return;

	/* Get Dependencies node.
	 */
	Node	*dependencies = root->GetNodeByName("Dependencies");

	if (dependencies == NIL) return;

	for (Int i = 0; i < dependencies->GetNOfNodes(); i++)
	{
		Node		*node = dependencies->GetNthNode(i);

		if (node->GetName() != "PackageDependency") continue;

		Attribute	*name = node->GetAttributeByName("Name");

		if (name == NIL) continue;

		GetPackageFolders(name->GetContent(), packageFolders);
	}
}

static Void GetPackageFolders(const String &packageName, Array<String> &packageFolders)
{
	/* Open Packages key.
	 */
	HKEY	 packagesKey;

	if (RegOpenKey(HKEY_CLASSES_ROOT, L"Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\AppModel\\Repository\\Packages", &packagesKey) != ERROR_SUCCESS) return;

	/* Enumerate registered packages.
	 */
	Int	 index = 0;
	wchar_t	 name[256];

	while (RegEnumKey(packagesKey, index++, name, sizeof(name) / sizeof(wchar_t)) == ERROR_SUCCESS)
	{
		if (!String(name).StartsWith(packageName)) continue;
		if (!String(name).Contains(String("_").Append(architecture).Append("_"))) continue;

		/* Found package, open key.
		 */
		HKEY	 packageKey;

		if (RegOpenKey(packagesKey, name, &packageKey) != ERROR_SUCCESS) continue;

		/* Query folder.
		 */
		const DWORD	 size	      = 2048;
		DWORD		 bytes	      = size * sizeof(wchar_t);
		wchar_t		 folder[size] = { 0 };

		RegQueryValueEx(packageKey, L"PackageRootFolder", 0, 0, (BYTE *) folder, &bytes);
		RegCloseKey(packageKey);

		/* Add folder and dependencies to list.
		 */
		String	 packageFolder = folder;

		if (!packageFolder.EndsWith("\\")) packageFolder.Append("\\");

		if (packageFolders.Add(packageFolder, packageFolder.ComputeCRC32())) FindDependenciesFolders(packageFolder, packageFolders);

		break;
	}

	RegCloseKey(packagesKey);
}

static Void CopyLibrary(const String &libName, const Array<String> &packageFolders, const String &cacheFolder)
{
	/* Find source and target file names.
	 */
	String	 sourceFile;
	String	 targetFile = cacheFolder.Append(libName);

	foreach (const String &packageFolder, packageFolders)
	{
		if (!File(packageFolder.Append(libName)).Exists()) continue;

		sourceFile = packageFolder.Append(libName);
	}

	/* Check file existence and skip already existing files.
	 */
	if (!File(sourceFile).Exists() ||
	     File(targetFile).Exists()) return;

	/* Prepare input and output files.
	 */
	InStream	 in(STREAM_FILE, sourceFile);
	OutStream	 out(STREAM_FILE, targetFile, OS_REPLACE);

	/* Copy chunks of 128kB.
	 */
	Buffer<UnsignedByte>	 buffer(131072);

	Int			 overlap   = 0;
	Bool			 foundSelf = False;

	for (Int i = 0; i < in.Size(); i += buffer.Size() - overlap)
	{
		memcpy(buffer, buffer + buffer.Size() - overlap, overlap);

		Int	 bytes = in.InputData(buffer + overlap, Math::Min(buffer.Size() - overlap, in.Size() - in.GetPos()));

		out.OutputData(buffer + overlap, bytes);

		/* Check for referenced DLLs.
		 */
		for (Int n = 0; n < overlap + bytes - 5; n++)
		{
			if (buffer[n] != '.' || buffer[n + 1] != 'd' || buffer[n + 2] != 'l' || buffer[n + 3] != 'l' || buffer[n + 4] != 0) continue;

			for (Int m = n; m >= 0; m--)
			{
				if (buffer[m] >= 0x20) continue;

				/* Get DLL name and copy it if it looks like a reference.
				 */
				String	 dllName = (char *) (UnsignedByte *) buffer + m + 1;

				if	(foundSelf)	     CopyLibrary(dllName, packageFolders, cacheFolder);
				else if (dllName == libName) foundSelf = True;

				break;
			}
		}

		/* Overlap 128 bytes when wrapping around so we do not miss any relevant DLLs.
		 */
		overlap = 128;
	}
}

static Void CacheCoreAudioLibraries(const String &cacheFolder)
{
	Array<String>	 iTunesFolders;

	GetPackageFolders("AppleInc.iTunes", iTunesFolders);

	if (iTunesFolders.Length() == 0) return;

	/* Check if package ID has changed.
	 */
	const String	 packageId     = Directory(iTunesFolders.GetFirst()).GetDirectoryName();
	const String	 packageIdFile = cacheFolder.Append("PackageId");

	if (File(packageIdFile).Exists())
	{
		InStream	 in(STREAM_FILE, packageIdFile);

		if (in.InputLine() == packageId) return;
	}

	/* Create cache folder and make sure it's empty.
	 */
	Directory(cacheFolder).Create();
	Directory(cacheFolder).Empty();

	/* Recursively copy CoreAudioToolbox.dll and referenced DLLs.
	 */
	CopyLibrary(coreAudioToolbox, iTunesFolders, cacheFolder);

	/* Store package ID.
	 */
	OutStream	 out(STREAM_FILE, packageIdFile, OS_REPLACE);

	out.OutputLine(packageId);
}
#endif

Bool LoadCoreAudioDLL()
{
#ifndef __APPLE__
#	ifdef __WIN32__
	String	 coreAudioDir = GetSystemDirectory(CSIDL_PROGRAM_FILES_COMMON).Append("Apple\\Apple Application Support\\");

	if (!File(String(coreAudioDir).Append(coreAudioToolbox)).Exists())
	{
		coreAudioDir = GetSystemDirectory(CSIDL_PROGRAM_FILES).Append("iTunes\\");
	}

	if (!File(String(coreAudioDir).Append(coreAudioToolbox)).Exists())
	{
		coreAudioDir = String(BoCA::Config::Get()->cacheDir).Append("boca\\boca.encoder.coreaudio\\").Append(architecture).Append("\\");

		CacheCoreAudioLibraries(coreAudioDir);
	}

	/* Add Apple Application Services directory to DLL search path.
	 */
	SetDllDirectory(coreAudioDir);

	coreaudiodll = new DynamicLoader(String(coreAudioDir).Append(coreAudioToolbox));
#	endif

	if (coreaudiodll == NIL) return False;

	AudioFileInitializeWithCallbacks	= (AUDIOFILEINITIALIZEWITHCALLBACKS) coreaudiodll->GetFunctionAddress("AudioFileInitializeWithCallbacks");
	AudioFileClose				= (AUDIOFILECLOSE) coreaudiodll->GetFunctionAddress("AudioFileClose");
	AudioFileSetProperty			= (AUDIOFILESETPROPERTY) coreaudiodll->GetFunctionAddress("AudioFileSetProperty");
	AudioFileWritePackets			= (AUDIOFILEWRITEPACKETS) coreaudiodll->GetFunctionAddress("AudioFileWritePackets");

	AudioConverterNew			= (AUDIOCONVERTERNEW) coreaudiodll->GetFunctionAddress("AudioConverterNew");
	AudioConverterDispose			= (AUDIOCONVERTERDISPOSE) coreaudiodll->GetFunctionAddress("AudioConverterDispose");
	AudioConverterGetProperty		= (AUDIOCONVERTERGETPROPERTY) coreaudiodll->GetFunctionAddress("AudioConverterGetProperty");
	AudioConverterGetPropertyInfo		= (AUDIOCONVERTERGETPROPERTYINFO) coreaudiodll->GetFunctionAddress("AudioConverterGetPropertyInfo");
	AudioConverterSetProperty		= (AUDIOCONVERTERSETPROPERTY) coreaudiodll->GetFunctionAddress("AudioConverterSetProperty");
	AudioConverterFillComplexBuffer		= (AUDIOCONVERTERFILLCOMPLEXBUFFER) coreaudiodll->GetFunctionAddress("AudioConverterFillComplexBuffer");

	AudioFormatGetProperty			= (AUDIOFORMATGETPROPERTY) coreaudiodll->GetFunctionAddress("AudioFormatGetProperty");
	AudioFormatGetPropertyInfo		= (AUDIOFORMATGETPROPERTYINFO) coreaudiodll->GetFunctionAddress("AudioFormatGetPropertyInfo");

	if (AudioFileInitializeWithCallbacks	== NIL ||
	    AudioFileClose			== NIL ||
	    AudioFileSetProperty		== NIL ||
	    AudioFileWritePackets		== NIL ||

	    AudioConverterNew			== NIL ||
	    AudioConverterDispose		== NIL ||
	    AudioConverterGetProperty		== NIL ||
	    AudioConverterGetPropertyInfo	== NIL ||
	    AudioConverterSetProperty		== NIL ||
	    AudioConverterFillComplexBuffer	== NIL ||

	    AudioFormatGetProperty		== NIL ||
	    AudioFormatGetPropertyInfo		== NIL) { FreeCoreAudioDLL(); return False; }
#endif

	return True;
}

Void FreeCoreAudioDLL()
{
#ifndef __APPLE__
	BoCA::Utilities::FreeCodecDLL(coreaudiodll);
#endif

	coreaudiodll = NIL;
}

Bool LoadMP4v2DLL()
{
	mp4v2dll = BoCA::Utilities::LoadCodecDLL("mp4v2");

	if (mp4v2dll == NIL) return False;

	ex_MP4Optimize = (MP4OPTIMIZE) mp4v2dll->GetFunctionAddress("MP4Optimize");

	if (ex_MP4Optimize == NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	BoCA::Utilities::FreeCodecDLL(mp4v2dll);

	mp4v2dll = NIL;
}
