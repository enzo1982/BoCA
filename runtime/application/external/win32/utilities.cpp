 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/utilities.h>

#include <boca/common/utilities.h>

#include <windows.h>

using namespace smooth::IO;

String BoCA::AS::ExternalUtilities::GetMD5(const ComponentSpecs *specs, const String &encFileName)
{
	if (specs->external_md5_arguments == NIL) return NIL;

	/* Set up security attributes.
	 */
	SECURITY_ATTRIBUTES	 secAttr;

	ZeroMemory(&secAttr, sizeof(secAttr));

	secAttr.nLength		= sizeof(secAttr);
	secAttr.bInheritHandle	= True;

	HANDLE	 rPipe = NIL;
	HANDLE	 wPipe = NIL;

	CreatePipe(&rPipe, &wPipe, &secAttr, 131072);
	SetHandleInformation(rPipe, HANDLE_FLAG_INHERIT, 0);

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String("\"").Append(encFileName).Append("\""));

	if (specs->debug) AllocConsole();

	STARTUPINFOA		 startupInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));

	startupInfo.cb		= sizeof(startupInfo);
	startupInfo.dwFlags	= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	startupInfo.wShowWindow	= specs->debug ? SW_SHOW : SW_HIDE;
	startupInfo.hStdInput	= GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput	= specs->external_md5_stderr ? (specs->debug ? GetStdHandle(STD_OUTPUT_HANDLE) : NIL) : wPipe;
	startupInfo.hStdError	= specs->external_md5_stderr ? wPipe : (specs->debug ? GetStdHandle(STD_ERROR_HANDLE) : NIL);

	PROCESS_INFORMATION	 processInfo;

	ZeroMemory(&processInfo, sizeof(processInfo));

	CreateProcessA(NIL, String(command).Append(" ").Append(arguments), NIL, NIL, True, 0, NIL, NIL, &startupInfo, &processInfo);

	/* Close stdio pipe write handle.
	 */
	CloseHandle(wPipe);

	/* Check process handle.
	 */
	if (processInfo.hProcess == NIL)
	{
		CloseHandle(rPipe);

		return NIL;
	}

	/* Read output into buffer.
	 */
	Buffer<char>		 buffer(4096);
	Int			 bytesReadTotal = 0;
	DWORD			 bytesRead = 0;

	do
	{
		if (!ReadFile(rPipe, buffer + bytesReadTotal, 4096 - bytesReadTotal, &bytesRead, NIL) || bytesRead == 0) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	String	 output = (bytesReadTotal > 0 ? (char *) buffer : NIL);

	CloseHandle(rPipe);

	TerminateProcess(processInfo.hProcess, 0);

	/* Wait until the decoder exits.
	 */
	while (WaitForSingleObject(processInfo.hProcess, 0) == WAIT_TIMEOUT) S::System::System::Sleep(10);

	if (specs->debug)
	{
		BoCA::Utilities::InfoMessage("Click OK to close console window.");

		FreeConsole();
	}

	/* Check if anything went wrong.
	 */
	unsigned long	 exitCode = 0;

	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	if (!specs->external_ignoreExitCode && exitCode != 0) return NIL;

	/* Extract MD5 from output.
	 */
	String	 md5;

	if (output.Contains(specs->external_md5_require) &&
	    output.Contains(specs->external_md5_prefix)) md5 = output.SubString(output.Find(specs->external_md5_prefix) + specs->external_md5_prefix.Length(),
										output.Length() - output.Find(specs->external_md5_prefix) - specs->external_md5_prefix.Length()).Trim().Head(32).ToLower();

	if (md5.Length() != 32 || md5.Contains("\n") || md5.Contains(" ")) md5 = NIL;

	return md5;
}
