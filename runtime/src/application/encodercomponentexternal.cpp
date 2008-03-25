 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/encodercomponentexternal.h>
#include <boca/common/utilities.h>

using namespace smooth::IO;

BoCA::AS::EncoderComponentExternal::EncoderComponentExternal(ComponentSpecs *specs) : EncoderComponent(specs)
{
	errorState	= False;
	errorString	= "Unknown error";

	out = NIL;
}

BoCA::AS::EncoderComponentExternal::~EncoderComponentExternal()
{
}

Bool BoCA::AS::EncoderComponentExternal::SetAudioTrackInfo(const Track &nFormat)
{
	format = nFormat;

	return True;
}

String BoCA::AS::EncoderComponentExternal::GetOutputFileExtension()
{
	return specs->formats.GetFirst()->GetExtensions().GetFirst();
}

Bool BoCA::AS::EncoderComponentExternal::Activate()
{
	/* Create temporary WAVE file
	 */
	nOfSamples = 0;

	wavFileName = Utilities::GetNonUnicodeTempFileName(format.outfile).Append(".wav");
	encFileName = Utilities::GetNonUnicodeTempFileName(format.outfile).Append(".").Append(GetOutputFileExtension());

	out = new OutStream(STREAM_FILE, wavFileName, OS_OVERWRITE);

	/* Write WAVE header
	 */
	out->OutputString("RIFF");
	out->OutputNumber(format.length * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");
	out->OutputString("fmt ");

	out->OutputNumber(16, 4);
	out->OutputNumber(1, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);

	out->OutputNumber(format.bits, 2);
	out->OutputString("data");
	out->OutputNumber(format.length * (format.bits / 8), 4);

	return True;
}

Bool BoCA::AS::EncoderComponentExternal::Deactivate()
{
	/* Finalize and close the uncompressed temporary file
	 */
	Int	 size = nOfSamples * (format.bits / 8) + 36;

	out->Seek(4);
	out->OutputData((unsigned char *) &size, 4);

	size -= 36;

	out->Seek(40);
	out->OutputData((unsigned char *) &size, 4);

	delete out;

	/* Start 3rd party command line encoder
	 */
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= specs->external_command;
	execInfo.lpParameters	= String(specs->external_arguments).Replace("%INFILE", String("\"").Append(wavFileName).Append("\"")).Replace("%OUTFILE", String("\"").Append(encFileName).Append("\""));
	execInfo.lpDirectory	= Application::GetApplicationDirectory();
	execInfo.nShow		= SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
	 */
	unsigned long	 exitCode = 0;

	do
	{
		Sleep(100);

		GetExitCodeProcess(execInfo.hProcess, &exitCode);
	}
	while (exitCode == STILL_ACTIVE);

	/* Stream contents of created file to output driver
	 */
	InStream		 in(STREAM_FILE, encFileName, IS_READONLY);
	Buffer<UnsignedByte>	 buffer(1024);
	Int			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		driver->WriteData((UnsignedByte *) in.InputData(buffer, Math::Min(1024, bytesLeft)), Math::Min(1024, bytesLeft));

		bytesLeft -= Math::Min(1024, bytesLeft);
	}

	in.Close();

	File(wavFileName).Delete();
	File(encFileName).Delete();

	return True;
}

Int BoCA::AS::EncoderComponentExternal::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	/* Hand data over to the output file
	 */
	nOfSamples += (size / (format.bits / 8));

	out->OutputData(data, size);

	return size;
}

BoCA::ConfigLayer *BoCA::AS::EncoderComponentExternal::GetConfigurationLayer()
{
	return NIL;
}

Void BoCA::AS::EncoderComponentExternal::FreeConfigurationLayer()
{
}
