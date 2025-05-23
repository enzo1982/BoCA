 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2025 Robert Kausch <robert.kausch@freac.org>
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

#include <signal.h>
#include <sys/wait.h>

using namespace smooth::IO;

namespace BoCA
{
	namespace AS
	{
		static String GetComponentInstructions(ComponentSpecs *specs, const String &arguments, Bool useStderr)
		{
			/* Start 3rd party command line executable.
			 */
			String	 command = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
			FILE	*rPipe	 = popen(String(command).Append(useStderr ? " 2>&1" : (specs->debug ? NIL : " 2> /dev/null")), "r");

			/* Read output into buffer.
			 */
			String		 output;
			Buffer<char>	 buffer(4096);

			while (True)
			{
				Int bytesRead = fread(buffer, 1, buffer.Size(), rPipe);

				if (bytesRead != buffer.Size() && (ferror(rPipe) || bytesRead == 0)) break;

				output.Append((char *) buffer);
			}

			/* Wait until the program exits.
			 */
			pclose(rPipe);

			/* Process and return output.
			 */
			output.Replace("\t", " ");
			output.Replace("\r", "");
			output.Replace("\n", "\n ");

			return output;
		}
	}
}

Void BoCA::AS::ExternalUtilities::CheckParameterRequirements(ComponentSpecs *specs)
{
	String	 instructions;

	for (Int i = specs->parameters.Length() - 1; i >= 0; i--)
	{
		Parameter				*parameter    = specs->parameters.GetNth(i);
		const Array<ParameterRequirement>	&requirements = parameter->GetRequirements();

		foreach (const ParameterRequirement &requirement, requirements)
		{
			if (instructions == NIL) instructions = GetComponentInstructions(specs, requirement.arguments, requirement.useStderr);

			if (!instructions.Contains(String(" ").Append(requirement.option).Append(" ")))
			{
				specs->parameters.RemoveNth(i);
				break;
			}
		}
	}
}

String BoCA::AS::ExternalUtilities::GetMD5(const ComponentSpecs *specs, const String &encFileName)
{
	if (specs->external_md5_arguments == NIL) return NIL;

	/* Start 3rd party command line decoder.
	 */
	String	 command   = String("\"").Append(specs->external_command).Append("\"").Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(specs->external_md5_arguments).Replace("%INFILE", String(encFileName).Replace("\\", "\\\\").Replace(" ", "\\ ")
													 .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
													 .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
													 .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*rPipe = popen(String(command).Append(" ").Append(arguments).Append(specs->external_md5_stderr ? " 2>&1" : (specs->debug ? NIL : " 2> /dev/null")), "r");

	/* Read output into buffer.
	 */
	Buffer<char>	 buffer(4096);
	Int		 bytesReadTotal = 0;
	Int		 bytesRead = 0;

	do
	{
		bytesRead = fread(buffer + bytesReadTotal, 1, 4096 - bytesReadTotal, rPipe);

		if (bytesRead != 4096 - bytesReadTotal && (ferror(rPipe) || bytesRead == 0)) break;

		bytesReadTotal += bytesRead;
	}
	while (bytesReadTotal < 4096);

	String	 output = (bytesReadTotal > 0 ? (char *) buffer : NIL);

	/* Wait until the decoder exits.
	 */
	unsigned long	 exitStatus = pclose(rPipe);
	unsigned long	 exitCode   = WIFEXITED(exitStatus)   ? WEXITSTATUS(exitStatus) : -1;
	unsigned long	 exitSignal = WIFSIGNALED(exitStatus) ? WTERMSIG(exitStatus)	: -1;

	/* Check if anything went wrong.
	 */
	if (!specs->external_ignoreExitCode && exitCode != 0 && exitCode != 0x80 + SIGPIPE && exitSignal != SIGPIPE) return NIL;

	/* Extract MD5 from output.
	 */
	String	 md5;

	if (output.Contains(specs->external_md5_require) &&
	    output.Contains(specs->external_md5_prefix)) md5 = output.SubString(output.Find(specs->external_md5_prefix) + specs->external_md5_prefix.Length(),
										output.Length() - output.Find(specs->external_md5_prefix) - specs->external_md5_prefix.Length()).Trim().Head(32).ToLower();

	if (md5.Length() != 32 || md5.Contains("\n") || md5.Contains(" ")) md5 = NIL;

	return md5;
}
