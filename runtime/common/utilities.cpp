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

#include <boca/common/utilities.h>
#include <boca/common/config.h>
#include <boca/common/i18n.h>

#if defined __HAIKU__
#	include <dlfcn.h>
#endif

using namespace smooth::System;
using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

/* Print an informational message.
 */
Void BoCA::Utilities::InfoMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->GetIntValue("Settings", "EnableConsole", False)) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Info"), Message::Buttons::Ok, Message::Icon::Information);
	else							      Console::OutputString(String("\n").Append(i18n->TranslateString("Info")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Print a warning message.
 */
Void BoCA::Utilities::WarningMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->GetIntValue("Settings", "EnableConsole", False)) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Warning"), Message::Buttons::Ok, Message::Icon::Warning);
	else							      Console::OutputString(String("\n").Append(i18n->TranslateString("Warning")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Print an error message.
 */
Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Errors");

	if (!config->GetIntValue("Settings", "EnableConsole", False)) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Error"), Message::Buttons::Ok, Message::Icon::Error);
	else							      Console::OutputString(String("\n").Append(i18n->TranslateString("Error")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Returns to path to the BoCA components directory.
 */
String BoCA::Utilities::GetBoCADirectory()
{
	Directory	 bocaDirectory(GUI::Application::GetApplicationDirectory().Append("boca"));

#if defined __HAIKU__
	if (!bocaDirectory.Exists())
	{
		/* Query actual BoCA library path on Haiku.
		 */
		Dl_info	 info = { 0 };

		dladdr((void *) &BoCA::Utilities::GetBoCADirectory, &info);

		bocaDirectory = File(info.dli_fname).GetFilePath();
	}
#elif !defined __WIN32__
	if (!bocaDirectory.Exists()) bocaDirectory = GUI::Application::GetApplicationDirectory().Append("../lib/boca");

	if (!bocaDirectory.Exists()) bocaDirectory = String(BOCA_INSTALL_PREFIX).Append("/lib/boca");
#endif

	return String(bocaDirectory).Append(Directory::GetDirectoryDelimiter());
}

/* Locates and loads a codec library.
 */
DynamicLoader *BoCA::Utilities::LoadCodecDLL(const String &module)
{
	DynamicLoader	*loader = NIL;

	/* Try loading a custom version of the codec.
	 */
#ifdef __WIN32__
	if (File(GUI::Application::GetApplicationDirectory().Append("codecs\\").Append(module).Append(".dll")).Exists())
#endif

	loader = new DynamicLoader(String("codecs/").Append(module));

	if (loader != NIL && loader->GetSystemModuleHandle() == NIL)
	{
		Object::DeleteObject(loader);

		loader = NIL;
	}

	/* Try loading a system-wide version of the codec.
	 */
	if (loader == NIL)
	{
#ifdef __WIN32__
		if (File(GUI::Application::GetApplicationDirectory().Append(module).Append(".dll")).Exists())
#endif

		loader = new DynamicLoader(module);

		if (loader != NIL && loader->GetSystemModuleHandle() == NIL)
		{
			Object::DeleteObject(loader);

			loader = NIL;
		}
	}

	return loader;
}

/* Free a previously loaded codec library.
 */
Bool BoCA::Utilities::FreeCodecDLL(DynamicLoader *loader)
{
	if (loader == NIL) return False;

	Object::DeleteObject(loader);

	return True;
}

/* Computes a non-Unicode temporary file name for
 * the passed file name.
 */
String BoCA::Utilities::GetNonUnicodeTempFileName(const String &file)
{
	const String	&tempDir = S::System::System::GetTempDirectory();

	/* Replace Unicode characters in input file name and
	 * append CRC32 of orignal file name to avoid name clashes.
	 */
	String	 fileName = File(file).GetFileName().Head(259 - tempDir.Length() - 14);

	for (Int i = 0; i < fileName.Length(); i++) if (fileName[i] > 127) fileName[i] = '#';

	fileName.Append("-").Append(Number(Int64(file.ComputeCRC32())).ToHexString(8));

	/* Return temporary file name in temp folder.
	 */
	return tempDir.Append(fileName).Append(".temp");
}

/* Replaces or strips characters not allowed in file names.
 */
String BoCA::Utilities::ReplaceIncompatibleCharacters(const String &string, Bool useUnicode, Bool replaceSlashes, Bool replaceSpaces)
{
	String	 result;
	Int	 length = string.Length();

	for (Int i = 0, p = 0; i < length; i++, p++)
	{
		wchar_t	 character = string[i];

		/* Replace other characters.
		 */
		if	(character == '\"')		     { result[p] = '\''; result[++p] = '\''; }
		else if (character == '\n')		       p--;
		else if (character == '\r')		       p--;
		else if (character == '?')		       p--;
		else if (character == '|')		       result[p] = '_';
		else if (character == '*')		       p--;
		else if (character == '<')		       result[p] = '(';
		else if (character == '>')		       result[p] = ')';
		else if (character == ':')		       p--;
		else if (character == '/'  &&  replaceSlashes) result[p] = '-';
		else if (character == '\\' &&  replaceSlashes) result[p] = '-';
		else if (character == ' '  &&  replaceSpaces)  result[p] = '_';
		else if (character == '\t' &&  replaceSpaces)  result[p] = '_';
		else if (character == '\t')		       result[p] = ' ';
#ifndef __WIN32__
		else if (character == '.'  && !replaceSlashes && (p == 0 || result[p - 1] == '/' || result[p - 1] == '\\'))
		{
			if	(useUnicode)		       result[p] = 0x2024;
			else if (replaceSpaces)		       result[p] = ',';
			else				     { result[p] = ' '; result[++p] = character; }
		}
#endif
		else if (character >= 256  && !useUnicode)     result[p] = '#';
		else					       result[p] = character;
	}

	return result;
}

/* This function checks if the passed path is a relative one.
 */
Bool BoCA::Utilities::IsRelativePath(const String &path)
{
#ifdef __WIN32__
	return ( path[1] != ':'		 && // Absolute local path
		!path.StartsWith("\\\\") && // Network resource
		!path.Contains("://"));	    // URI
#else
	return (!path.StartsWith("/")	 && // Absolute path
		!path.StartsWith("~")	 && // Home directory
		!path.Contains("://"));	    // URI
#endif
}

/* This function checks if a folder can be written to by trying
 * to create a file inside it.
 */
Bool BoCA::Utilities::IsFolderWritable(const String &path)
{
	Math::RandomSeed();

	Bool		 result	= False;
	Directory	 folder = path;
	String		 file	= String(folder).Append(Directory::GetDirectoryDelimiter()).Append(String::FromInt(Math::Random())).Append(".temp");
	OutStream	 temp(STREAM_FILE, file, OS_REPLACE);

	if (temp.GetLastError() == IO_ERROR_OK) result = True;

	temp.Close();

	File(file).Delete();

	return result;
}

/* This function returns the absolute path corresponding to the passed
 * path. It may differ from the passed one due to use of the <installdrive>
 * or <profile> placeholders or because the passed path is a relative one.
 */
String BoCA::Utilities::GetAbsolutePathName(const String &path)
{
	String	 pathName = path;

	/* Replace <installdrive> placeholder.
	 */
#ifdef __WIN32__
	pathName.Replace("<installdrive>", GUI::Application::GetApplicationDirectory().Head(2));
#else
	pathName.Replace("<installdrive>", NIL);
#endif

	/* Replace <profile> placeholder.
	 */
	String	 personalFiles = S::System::System::GetPersonalFilesDirectory();

	if (personalFiles.EndsWith(Directory::GetDirectoryDelimiter())) personalFiles[personalFiles.Length() - 1] = 0;

	pathName.Replace("<profile>", personalFiles);

	/* Convert relative to absolute paths.
	 */
	if (IsRelativePath(pathName)) pathName = GUI::Application::GetApplicationDirectory().Append(pathName);

	if (!pathName.EndsWith(Directory::GetDirectoryDelimiter())) pathName.Append(Directory::GetDirectoryDelimiter());

	return pathName;
}

/* This function takes a file name and normalizes all the
 * directory names included in the path by removing spaces
 * and dots at the end. It also shortens each directory name
 * to a maximum of 255 characters and the file name to 246
 * characters (to leave room for file extensions).
 */
String BoCA::Utilities::NormalizeFileName(const String &fileName)
{
	Int	 maxFolderLength = 255;
	Int	 maxFileLength	 = 246;
	String	 path		 = fileName;

	/* Normalize directory delimiters.
	 */
	path.Replace("\\", Directory::GetDirectoryDelimiter());
	path.Replace("/",  Directory::GetDirectoryDelimiter());

	/* Split path into components and process those.
	 */
	const Array<String>	&components = path.Explode(Directory::GetDirectoryDelimiter());

	foreach (String component, components)
	{
		/* Shorten to at most maxLength characters.
		 */
		if (foreachindex < components.Length() - 1)
		{
			if (component.Length() > maxFolderLength) component[maxFolderLength] = 0;

			/* Remove trailing dots and spaces.
			 */
			if (component != ".." && component != ".") while (component.EndsWith(".") || component.EndsWith(" ")) component[component.Length() - 1] = 0;
		}
		else if (foreachindex == components.Length() - 1)
		{
			String	 trimmed = component;

			while (trimmed.EndsWith(" ")) trimmed[trimmed.Length() - 1] = 0;

			if (component.Length() > maxFileLength) component[maxFileLength] = 0;

			/* Remove trailing spaces if we cut the end off of the file name.
			 */
			if (component.Length() < trimmed.Length()) while (component.EndsWith(" ")) component[component.Length() - 1] = 0;
		}

		/* Append component back to path.
		 */
		if (foreachindex == 0) path = component;
		else		       path.Append(Directory::GetDirectoryDelimiter()).Append(component);
	}

	return path;
}

/* Creates the folder corresponding to the file name
 * passed in the first argument.
 */
String BoCA::Utilities::CreateDirectoryForFile(const String &fileName)
{
	File		 file(fileName);
	Directory	 directory(file.GetFilePath());

	directory.Create();

	return file;
}

/* Computes a track file name relative to a base file name.
 */
String BoCA::Utilities::GetRelativeFileName(const String &trackFileName, const String &baseFileName)
{
	String	 compTrackFileName = trackFileName;
	String	 compBaseFileName  = baseFileName;

#ifdef __WIN32__
	/* Ignore case on Windows systems.
	 */
	compTrackFileName = compTrackFileName.ToLower();
	compBaseFileName  = compBaseFileName.ToLower();
#endif

	Int	 equalBytes	   = 0;
	Int	 furtherComponents = 0;
	Bool	 found		   = False;

	for (Int i = 0; i < baseFileName.Length(); i++)
	{
		if (compBaseFileName[i] != compTrackFileName[i]) found = True;

		if (baseFileName[i] == '\\' || baseFileName[i] == '/')
		{
			if (!found) equalBytes = i + 1;
			else	    furtherComponents++;
		}
	}

	String	 relativeFileName = trackFileName;

	if (equalBytes > 0)
	{
		relativeFileName = NIL;

		for (Int m = 0; m < trackFileName.Length() - equalBytes; m++) relativeFileName[m] = trackFileName[m + equalBytes];
	}

	if ( relativeFileName[1] != ':' &&	    // Absolute local path
	    !relativeFileName.StartsWith("\\\\") && // Network resource
	    !relativeFileName.Contains("://"))	    // Protocol
	{
		for (Int m = 0; m < furtherComponents; m++) relativeFileName = String("..").Append(Directory::GetDirectoryDelimiter()).Append(relativeFileName);
	}

	return relativeFileName;
}

/* Gets the .cda file name corresponding to an audio
 * CD track on Windows systems.
 */
String BoCA::Utilities::GetCDTrackFileName(const Track &track)
{
	if (!track.isCDTrack) return track.fileName;

	/* Special handling for CD tracks on Windows.
	 */
	String	 fileName = track.fileName;

#ifdef __WIN32__
	for (Int drive = 2; drive < 26; drive++)
	{
		String	 trackCDA = String(" ").Append(":\\track").Append(track.cdTrack < 10 ? "0" : NIL).Append(String::FromInt(track.cdTrack)).Append(".cda");

		trackCDA[0] = drive + 'A';

		InStream	 in(STREAM_FILE, trackCDA, IS_READ);

		in.Seek(32);

		Int	 trackLength = in.InputNumber(4);

		if (track.length == (trackLength * 1176) / (track.GetFormat().bits / 8))
		{
			fileName = trackCDA;

			break;
		}
	}
#endif

	return fileName;
}

/* This function changes the byte order of the first argument
 * from big-endian to little-endian and vice versa.
 */
Bool BoCA::Utilities::SwitchByteOrder(UnsignedByte *value, Int bytes)
{
	if (value == NIL) return False;

	for (Int i = 0; i < bytes / 2; i++)
	{
		value[i] ^= value[bytes - 1 - i];
		value[bytes - 1 - i] ^= value[i];
		value[i] ^= value[bytes - 1 - i];
	}

	return True;
}

/* This function changes the byte order of audio samples in
 * a buffer from big-endian to little-endian and vice versa.
 */
Bool BoCA::Utilities::SwitchBufferByteOrder(Buffer<UnsignedByte> &buffer, Int bytesPerSample)
{
	for (Int i = 0; i < buffer.Size(); i += bytesPerSample)
	{
		for (Int j = 0; j < bytesPerSample / 2; j++)
		{
			buffer[i + j] ^= buffer[i + bytesPerSample - 1 - j];
			buffer[i + bytesPerSample - 1 - j] ^= buffer[i + j];
			buffer[i + j] ^= buffer[i + bytesPerSample - 1 - j];
		}
	}

	return True;
}

/* This function changes the order of channels of audio
 * samples in a buffer from inLayout to outLayout.
 */
Bool BoCA::Utilities::ChangeChannelOrder(Buffer<UnsignedByte> &buffer, const Format &format, const Channel::Layout inLayout, const Channel::Layout outLayout)
{
	Int	 bytesPerSample = format.bits / 8;

	/* Fail if buffer does not include full samples.
	 */
	if (buffer.Size() % (bytesPerSample * format.channels) != 0) return False;

	/* Change channel order.
	 */
	UnsignedByte	*intermediate = new UnsignedByte [bytesPerSample * 256];

	for (Int i = 0; i < buffer.Size(); i += bytesPerSample * format.channels)
	{
		for (Int c = 0; c < format.channels; c++) memcpy(intermediate + inLayout[c] * bytesPerSample, buffer + i + c * bytesPerSample, bytesPerSample);
		for (Int c = 0; c < format.channels; c++) memcpy(buffer + i + c * bytesPerSample, intermediate + outLayout[c] * bytesPerSample, bytesPerSample);
	}

	delete [] intermediate;

	return True;
}
