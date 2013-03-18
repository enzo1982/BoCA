 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "converter.h"

#ifdef __WIN32__
#	include <windows.h>
#else
#	include <sys/wait.h>
#endif

Array<BoCA::Converter *>	 BoCA::Converter::converters;

Array<BoCA::Converter *> &BoCA::Converter::Get()
{
	if (converters.Length() == 0)
	{
		Directory		 dir(Utilities::GetBoCADirectory().Append("boca/boca.extension.youtube"));
		const Array<File>	&files = dir.GetFilesByPattern("converter_*.xml");

		foreach (const File &file, files)
		{
			Converter	*converter = new Converter(file);

			if (converter->IsSane())
			{
				converters.Add(converter);
			}
			else
			{
				delete converter;
			}
		}
	}

	return converters;
}

Bool BoCA::Converter::Free()
{
	foreach (Converter *converter, converters) delete converter;

	converters.RemoveAll();

	return True;
}

BoCA::Converter::Converter(const String &fileName)
{
	format = new AS::FileFormat();

	ParseXML(fileName);
}

BoCA::Converter::~Converter()
{
	delete format;
}

Bool BoCA::Converter::IsSane()
{
	if (!File(external_command).Exists()) return False;
	else				      return True;
}

Int BoCA::Converter::ParseXML(const String &fileName)
{
	XML::Document	*document = new XML::Document();

	document->LoadFile(fileName);

	XML::Node	*root = document->GetRootNode();

	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if 	(node->GetName() == "name")    name    = node->GetContent();
		else if (node->GetName() == "version") version = node->GetContent();
		else if (node->GetName() == "decoder") decoder = node->GetContent();
		else if (node->GetName() == "format")
		{
			format->SetTagMode(TAG_MODE_NONE);

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "name")	  format->SetName(node2->GetContent());
				else if (node2->GetName() == "extension") format->AddExtension(node2->GetContent());
				else if (node2->GetName() == "tag")
				{
					format->SetTagFormat(node2->GetContent());

					if (node2->GetAttributeByName("id") != NIL) format->SetTaggerID(node2->GetAttributeByName("id")->GetContent());

					if (node2->GetAttributeByName("mode") != NIL)
					{
						if	(node2->GetAttributeByName("mode")->GetContent() == "prepend")	format->SetTagMode(TAG_MODE_PREPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "append")	format->SetTagMode(TAG_MODE_APPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "other")	format->SetTagMode(TAG_MODE_OTHER);
					}
				}
			}
		}
		else if (node->GetName() == "external")
		{
			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if (node2->GetName() == "command")
				{
					external_command = node2->GetContent();

					if (node2->GetAttributeByName("ignoreExitCode") != NIL)
					{
						external_ignoreExitCode = (node2->GetAttributeByName("ignoreExitCode")->GetContent() == "true");
					}
				}
				else if (node2->GetName() == "arguments")
				{
					external_arguments = node2->GetContent();
				}
			}
		}
	}

	delete document;

	/* Check if external command actually exists.
	 */
	if (external_command[0] != '/' && external_command[1] != ':')
	{
#if defined __WIN32__
		static const char	*places[] = { "%APPDIR\\codecs\\cmdline\\%COMMAND", "%APPDIR\\codecs\\cmdline\\%COMMAND.exe", NIL };
#elif defined __APPLE__
		static const char	*places[] = { "%APPDIR\\codecs\\cmdline\\%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/opt/local/bin/%COMMAND", "/sw/bin/%COMMAND", NIL };
#elif defined __HAIKU__
		static const char	*places[] = { "/boot/common/bin/%COMMAND", NIL };
#elif defined __NetBSD__
		static const char	*places[] = { "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/usr/pkg/bin/%COMMAND", NIL };
#else
		static const char	*places[] = { "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", NIL };
#endif

		for (Int i = 0; places[i] != NIL; i++)
		{
			String	 file = String(places[i]).Replace("%APPDIR", GUI::Application::GetApplicationDirectory()).Replace("%COMMAND", external_command).Replace("\\\\", "\\");

			if (File(file).Exists()) { external_command = file; break; }
		}

		if (external_command[0] != '/' && external_command[1] != ':') return False;
	}

	if (!File(external_command).Exists()) return Error();

	return Success();
}

Int BoCA::Converter::Run(const String &inFile, const String &outFile)
{
	/* Start 3rd party command line encoder
	 */
#if defined __WIN32__
	SHELLEXECUTEINFOA	 execInfo;

	ZeroMemory(&execInfo, sizeof(execInfo));

	execInfo.cbSize		= sizeof(execInfo);
	execInfo.fMask		= SEE_MASK_NOCLOSEPROCESS;
	execInfo.lpVerb		= "open";
	execInfo.lpFile		= String(external_command).Replace("/", Directory::GetDirectoryDelimiter());
	execInfo.lpParameters	= String(external_arguments).Replace("%INFILE", String("\"").Append(inFile).Append("\""))
							    .Replace("%OUTFILE", String("\"").Append(outFile).Append("\""));

	execInfo.lpDirectory	= GUI::Application::GetApplicationDirectory();
	execInfo.nShow		= SW_HIDE;

	ShellExecuteExA(&execInfo);

	/* Wait until the encoder exits
	 */
	unsigned long	 exitCode = 0;

	while (True)
	{
		GetExitCodeProcess(execInfo.hProcess, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		S::System::System::Sleep(10);
	}
#else
	String	 command   = String(external_command).Replace("/", Directory::GetDirectoryDelimiter());
	String	 arguments = String(external_arguments).Replace("%INFILE", String(inFile).Replace("\\", "\\\\").Replace(" ", "\\ ")
											 .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
											 .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
											 .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"))
						       .Replace("%OUTFILE", String(outFile).Replace("\\", "\\\\").Replace(" ", "\\ ")
											   .Replace("\"", "\\\"").Replace("\'", "\\\'").Replace("`", "\\`")
											   .Replace("(", "\\(").Replace(")", "\\)").Replace("<", "\\<").Replace(">", "\\>")
											   .Replace("&", "\\&").Replace(";", "\\;").Replace("$", "\\$").Replace("|", "\\|"));

	FILE	*pipe	   = popen(String(command).Append(" ").Append(arguments), "r");

	/* Wait until the encoder exits
	 */
	unsigned long	 exitStatus = pclose(pipe);
	unsigned long	 exitCode   = WEXITSTATUS(exitStatus);

#endif

	/* Check if anything went wrong
	 */
	if (!external_ignoreExitCode && exitCode != 0)
	{
		/* Remove output file
		 */
		File(outFile).Delete();

		return Error();
	}

	return Success();
}
