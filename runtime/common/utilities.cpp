 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

Void BoCA::Utilities::InfoMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Info"), Message::Buttons::Ok, Message::Icon::Information);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Info")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

Void BoCA::Utilities::WarningMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Warning"), Message::Buttons::Ok, Message::Icon::Exclamation);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Warning")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Error"), Message::Buttons::Ok, Message::Icon::Hand);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Error")).Append(": ").Append(i18n->TranslateString(message).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

String BoCA::Utilities::GetBoCADirectory()
{
	Directory	 bocaDirectory(GUI::Application::GetApplicationDirectory().Append("boca"));

#ifndef __WIN32__
	if (!bocaDirectory.Exists()) bocaDirectory = GUI::Application::GetApplicationDirectory().Append("../lib/boca");

	if (!bocaDirectory.Exists()) bocaDirectory = String(BOCA_INSTALL_PREFIX).Append("/lib/boca");
#endif

	return String(bocaDirectory).Append(Directory::GetDirectoryDelimiter());
}

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

Bool BoCA::Utilities::FreeCodecDLL(DynamicLoader *loader)
{
	if (loader == NIL) return False;

	Object::DeleteObject(loader);

	return True;
}

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

	return tempDir.Append(".temp");
}

String BoCA::Utilities::ReplaceIncompatibleCharacters(const String &string)
{
	String	 rVal;

	for (Int k = 0, b = 0; k < string.Length(); k++)
	{
		if	(string[k] == '\"')			      { rVal[k + b] = '\''; rVal[k + ++b] = '\''; }
		else if (string[k] == '?')				b--;
		else if (string[k] == '|')				rVal[k + b] = '_';
		else if (string[k] == '*')				b--;
		else if (string[k] == '<')				rVal[k + b] = '(';
		else if (string[k] == '>')				rVal[k + b] = ')';
		else if (string[k] == ':')				b--;
		else if (string[k] == '/')				rVal[k + b] = '-';
		else if (string[k] == '\\')				rVal[k + b] = '-';
		else if (string[k] >= 256  && !Setup::enableUnicode)	rVal[k + b] = '#';
		else							rVal[k + b] = string[k];
	}

	return rVal;
}

String BoCA::Utilities::CreateDirectoryForFile(const String &fileName)
{
	File		 file(fileName);
	Directory	 directory(file.GetFilePath());

	directory.Create();

	return file;
}

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
