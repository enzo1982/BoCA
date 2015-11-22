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

#include <boca/common/utilities.h>
#include <boca/common/config.h>
#include <boca/common/i18n.h>

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

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Info"), Message::Buttons::Ok, Message::Icon::Information);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Info")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Print a warning message.
 */
Void BoCA::Utilities::WarningMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Warning"), Message::Buttons::Ok, Message::Icon::Warning);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Warning")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Print an error message.
 */
Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Error"), Message::Buttons::Ok, Message::Icon::Error);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Error")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

/* Returns to path to the BoCA components directory.
 */
String BoCA::Utilities::GetBoCADirectory()
{
	Directory	 bocaDirectory(GUI::Application::GetApplicationDirectory().Append("boca"));

#ifndef __WIN32__
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
String BoCA::Utilities::GetNonUnicodeTempFileName(const String &fileName)
{
	String	 rVal	= fileName;
	Int	 lastBs	= -1;

	/* Replace Unicode characters in input file name
	 */
	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 127)			rVal[i] = '#';
		if (rVal[i] == '\\' || rVal[i] == '/')	lastBs = i;
	}

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append("-").Append(Number(Int64(fileName.ComputeCRC32())).ToHexString(8)).Append(".temp");
}

/* Replaces or strips characters not allowed in file names.
 */
String BoCA::Utilities::ReplaceIncompatibleCharacters(const String &string, Bool useUnicode, Bool replaceSlashes, Bool replaceSpaces)
{
	String	 rVal;

	for (Int k = 0, b = 0; k < string.Length(); k++)
	{
		if	(string[k] == '\"')		    { rVal[k + b] = '\''; rVal[k + ++b] = '\''; }
		else if (string[k] == '\n')		      b--;
		else if (string[k] == '\r')		      b--;
		else if (string[k] == '?')		      b--;
		else if (string[k] == '|')		      rVal[k + b] = '_';
		else if (string[k] == '*')		      b--;
		else if (string[k] == '<')		      rVal[k + b] = '(';
		else if (string[k] == '>')		      rVal[k + b] = ')';
		else if (string[k] == ':')		      b--;
		else if (string[k] == '/'  && replaceSlashes) rVal[k + b] = '-';
		else if (string[k] == '\\' && replaceSlashes) rVal[k + b] = '-';
		else if (string[k] == ' '  && replaceSpaces)  rVal[k + b] = '_';
		else if (string[k] == '\t' && replaceSpaces)  rVal[k + b] = '_';
		else if (string[k] == '\t')		      rVal[k + b] = ' ';
		else if (string[k] >= 256  && !useUnicode)    rVal[k + b] = '#';
		else					      rVal[k + b] = string[k];
	}

	return rVal;
}

/* This function returns the absolute path corresponding to the passed
 * path. It may differ from the passed one due to use of the <installdrive>
 * placeholder or because the passed path is a relative one.
 */
String BoCA::Utilities::GetAbsolutePathName(const String &pathName)
{
	String	 rPathName = pathName;

	/* Replace <installdrive> patter.
	 */
#ifdef __WIN32__
	rPathName.Replace("<installdrive>", GUI::Application::GetApplicationDirectory().Head(2));

	if ( rPathName[1] != ':' &&	   // Absolute local path
	    !rPathName.StartsWith("\\\\")) // Network resource
#else
	rPathName.Replace("<installdrive>", NIL);

	if (!rPathName.StartsWith("/") &&  // Absolute path
	    !rPathName.StartsWith("~"))	   // Home directory
#endif
	{
		rPathName = GUI::Application::GetApplicationDirectory().Append(rPathName);
	}

	return rPathName;
}

/* This function takes a file name and normalizes all the
 * directory names included in the path by removing spaces
 * and dots at the end. It also shortens each directory
 * and the file name to a maximum of 248 characters.
 */
String BoCA::Utilities::NormalizeFileName(const String &fileName)
{
	String	 rFileName = fileName;
	String	 dir	   = fileName;

	Int	 maxLength = 248;

	String	 tmpDir;
	Int	 lastBS	   = 0;

	for (Int i = 0; i < dir.Length(); i++)
	{
		if (dir[i] == '\\' || dir[i] == '/')
		{
			String	 tmpDir2 = tmpDir;

			/* Shorten to at most maxLength characters.
			 */
			if (tmpDir.Length() - lastBS > maxLength)
			{
				tmpDir2 = String().CopyN(tmpDir, lastBS + maxLength);

				i -= (tmpDir.Length() - lastBS - maxLength);
			}

			/* Replace trailing dots and spaces.
			 */
			while ((tmpDir2.Tail(tmpDir2.Length() - lastBS - 1) != ".." &&
				tmpDir2.Tail(tmpDir2.Length() - lastBS - 1) != ".") &&
			       (tmpDir2.EndsWith(".") || tmpDir2.EndsWith(" ")))
			{
				tmpDir2[tmpDir2.Length() - 1] = 0;

				i--;
			}

			if (tmpDir2 != tmpDir)
			{
				rFileName.Replace(tmpDir, tmpDir2);

				tmpDir = tmpDir2;
				dir = rFileName;
			}

			lastBS = i;
		}

		tmpDir[i] = dir[i];
	}

	/* Shorten file name to maxLength characters.
	 */
	if (rFileName.Length() - lastBS > maxLength) rFileName = String().CopyN(rFileName, lastBS + maxLength);

	/* Normalize directory delimiters.
	 */
	rFileName.Replace("\\",	Directory::GetDirectoryDelimiter());
	rFileName.Replace("/",	Directory::GetDirectoryDelimiter());

	/* Replace trailing spaces.
	 */
	while (rFileName.EndsWith(" ")) { rFileName[rFileName.Length() - 1] = 0; }

	return rFileName;
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
	/* Special handling for CD tracks on Windows.
	 */
	String	 fileName = track.origFilename;

#ifdef __WIN32__
	if (track.isCDTrack)
	{
		for (Int drive = 2; drive < 26; drive++)
		{
			String	 trackCDA = String(" ").Append(":\\track").Append(track.cdTrack < 10 ? "0" : NIL).Append(String::FromInt(track.cdTrack)).Append(".cda");

			trackCDA[0] = drive + 'A';

			InStream	*in = new InStream(STREAM_FILE, trackCDA, IS_READ);

			in->Seek(32);

			Int	 trackLength = in->InputNumber(4);

			delete in;

			if (track.length == (trackLength * 1176) / (track.GetFormat().bits / 8))
			{
				fileName = trackCDA;

				break;
			}
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
