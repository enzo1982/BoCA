 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/componentspecs.h>
#include <boca/common/config.h>

BoCA::AS::ComponentSpecs::ComponentSpecs()
{
	library = NIL;

	mode	= COMPONENT_MODE_INTERNAL;
	debug	= False;

	external_ignoreExitCode = False;
}

BoCA::AS::ComponentSpecs::~ComponentSpecs()
{
	if (library != NIL) delete library;

	foreach (FileFormat *format, formats) delete format;
	foreach (TagFormat *format, tag_formats) delete format;

	foreach (Parameter *parameter, external_parameters)
	{
		foreach (Option *option, parameter->GetOptions()) delete option;

		delete parameter;
	}
}

Bool BoCA::AS::ComponentSpecs::LoadFromDLL(const String &file)
{
	/* Try to load the DLL.
	 */
	library = new DynamicLoader(file);

	/* Bail out if the library could not be loaded.
	 */
	if (library->GetSystemModuleHandle() == NIL)
	{
		Object::DeleteObject(library);

		library = NIL;

		return False;
	}

	const char *(*BoCA_GetComponentName)() = (const char *(*)()) library->GetFunctionAddress("BoCA_GetComponentName");

	componentName = BoCA_GetComponentName();

	func_GetComponentSpecs		= (const char *(*)())					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetComponentSpecs"));

	func_Create			= (void *(*)())						library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Create"));
	func_Delete			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Delete"));

	func_GetConfigurationLayer	= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfigurationLayer"));

	func_GetErrorState		= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorState"));
	func_GetErrorString		= (const void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorString"));

	func_CanOpenStream		= (bool (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenStream"));
	func_GetStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStreamInfo"));

	func_SetVendorString		= (void (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetVendorString"));

	func_ParseBuffer		= (int (*)(void *, const void *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseBuffer"));
	func_ParseStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseStreamInfo"));

	func_RenderBuffer		= (int (*)(void *, void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderBuffer"));
	func_RenderStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderStreamInfo"));

	func_UpdateStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_UpdateStreamInfo"));

	func_GetPackageSize		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetPackageSize"));

	func_SetDriver			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetDriver"));

	func_GetInBytes			= (__int64 (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetInBytes"));

	func_CanWrite			= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanWrite"));

	func_SetPause			= (int (*)(void *, bool))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetPause"));
	func_IsPlaying			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsPlaying"));

	func_SetAudioTrackInfo		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetAudioTrackInfo"));
	func_GetFormatInfo		= (void (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetFormatInfo"));

	func_GetOutputFileExtension	= (char *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetOutputFileExtension"));

	func_Activate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Activate"));
	func_Deactivate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Deactivate"));

	func_Seek			= (bool (*)(void *, __int64))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Seek"));

	func_ReadData			= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadData"));
	func_WriteData			= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WriteData"));
	func_TransformData		= (int (*)(void *, void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_TransformData"));

	func_Flush			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Flush"));

	func_GetMainTabLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetMainTabLayer"));
	func_GetStatusBarLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStatusBarLayer"));

	func_GetNumberOfDevices		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNumberOfDevices"));
	func_GetNthDeviceInfo		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceInfo"));

	func_OpenNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_OpenNthDeviceTray"));
	func_CloseNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CloseNthDeviceTray"));

	func_GetNthDeviceTrackList	= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceTrackList"));
	func_GetNthDeviceMCDI		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceMCDI"));

	return ParseXMLSpec(String(func_GetComponentSpecs()).Trim());
}

Bool BoCA::AS::ComponentSpecs::LoadFromXML(const String &file)
{
	IO::InStream	 in(IO::STREAM_FILE, file, IO::IS_READ);
	String		 xml = in.InputString(in.Size());

	in.Close();

	return ParseXMLSpec(xml);
}

String BoCA::AS::ComponentSpecs::GetExternalArgumentsString()
{
	Config	*config = Config::Get();
	String	 arguments;

	foreach (Parameter *param, external_parameters)
	{
		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				if (!config->GetIntValue(id, param->GetName(), param->GetEnabled())) continue;

				arguments.Append(param->GetArgument()).Append(" ");

				break;
			case PARAMETER_TYPE_SELECTION:
				if (!config->GetIntValue(id, String("Set ").Append(param->GetName()), param->GetEnabled())) continue;

				arguments.Append(String(param->GetArgument()).Replace("%VALUE", config->GetStringValue(id, param->GetName(), param->GetDefault()))).Append(" ");

				break;
			case PARAMETER_TYPE_RANGE:
				if (!config->GetIntValue(id, String("Set ").Append(param->GetName()), param->GetEnabled())) continue;

				arguments.Append(String(param->GetArgument()).Replace("%VALUE", String::FromFloat(config->GetIntValue(id, param->GetName(), Math::Round(param->GetDefault().ToFloat() / param->GetStepSize())) * param->GetStepSize()))).Append(" ");

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	return arguments;
}

Bool BoCA::AS::ComponentSpecs::ParseXMLSpec(const String &xml)
{
	if (xml == NIL) return False;

	XML::Document	*document = new XML::Document();

	document->ParseMemory((void *) (char *) xml, xml.Length());

	XML::Node	*root = document->GetRootNode();

	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if (node->GetName() == "name")
		{
			name = node->GetContent();
		}
		else if (node->GetName() == "id")
		{
			id = node->GetContent();
		}
		else if (node->GetName() == "version")
		{
			version = node->GetContent();

			if (node->GetAttributeByName("debug") != NIL)
			{
				debug = (node->GetAttributeByName("debug")->GetContent() == "true");
			}
		}
		else if (node->GetName() == "type")
		{
			if	(node->GetContent() == "decoder")	type = COMPONENT_TYPE_DECODER;
			else if (node->GetContent() == "encoder")	type = COMPONENT_TYPE_ENCODER;
			else if (node->GetContent() == "output")	type = COMPONENT_TYPE_OUTPUT;
			else if (node->GetContent() == "deviceinfo")	type = COMPONENT_TYPE_DEVICEINFO;
			else if (node->GetContent() == "dsp")		type = COMPONENT_TYPE_DSP;
			else if (node->GetContent() == "extension")	type = COMPONENT_TYPE_EXTENSION;
			else if (node->GetContent() == "tagger")	type = COMPONENT_TYPE_TAGGER;
			else						type = COMPONENT_TYPE_UNKNOWN;
		}
		else if (node->GetName() == "format")
		{
			FileFormat	*format = new FileFormat();

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

			formats.Add(format);
		}
		else if (node->GetName() == "tagformat")
		{
			TagFormat	*format = new TagFormat();

			format->SetDefault(True);

			format->SetCoverArtSupported(False);
			format->SetCoverArtDefault(True);

			format->SetFreeEncodingSupported(False);

			if (node->GetAttributeByName("default") != NIL) format->SetDefault(node->GetAttributeByName("default")->GetContent() == "true");

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "name")	 format->SetName(node2->GetContent());
				else if	(node2->GetName() == "coverart")
				{
					if (node2->GetAttributeByName("supported") != NIL) format->SetCoverArtSupported(node2->GetAttributeByName("supported")->GetContent() == "true");
					if (node2->GetAttributeByName("default")   != NIL) format->SetCoverArtDefault(node2->GetAttributeByName("default")->GetContent() == "true");
				}
				else if (node2->GetName() == "encodings")
				{
					if (node2->GetAttributeByName("free") != NIL) format->SetFreeEncodingSupported(node2->GetAttributeByName("free")->GetContent() == "true");

					for (Int k = 0; k < node2->GetNOfNodes(); k++)
					{
						XML::Node	*node3 = node2->GetNthNode(k);
						
						if (node3->GetName() == "encoding")
						{
							format->AddEncoding(node3->GetContent());

							if (format->GetEncodings().Length() == 1) format->SetDefaultEncoding(node3->GetContent());

							if (node3->GetAttributeByName("default") != NIL)
							{
								if (node3->GetAttributeByName("default")->GetContent() == "true") format->SetDefaultEncoding(node3->GetContent());
							}
						}
					}
				}
			}

			tag_formats.Add(format);
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
				else if (node2->GetName() == "arguments")  external_arguments	= node2->GetContent();
				else if (node2->GetName() == "informat")   external_informat	= node2->GetContent();
				else if (node2->GetName() == "outformat")  external_outformat	= node2->GetContent();
				else if (node2->GetName() == "mode")	   mode			= node2->GetContent() == "file" ? COMPONENT_MODE_EXTERNAL_FILE : COMPONENT_MODE_EXTERNAL_STDIO;
				else if (node2->GetName() == "parameters") ParseExternalParameters(node2);
			}
		}
	}

	delete document;

	/* Check if external command actually exists.
	 */
	if (mode != COMPONENT_MODE_INTERNAL)
	{
		if (external_command[0] != '/' && external_command[1] != ':')
		{
#if defined __WIN32__
			static const char	*places[] = { "%APPDIR\\codecs\\cmdline\\%COMMAND", "%APPDIR\\codecs\\cmdline\\%COMMAND.exe", NIL };
#elif defined __APPLE__
			static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/opt/local/bin/%COMMAND", "/sw/bin/%COMMAND", NIL };
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

		if (!File(external_command).Exists()) return False;
	}

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseExternalParameters(XML::Node *root)
{
	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if (node->GetName() == "switch" || node->GetName() == "selection" || node->GetName() == "range")
		{
			Parameter	*parameter = new Parameter();

			parameter->SetEnabled(False);

			if (node->GetAttributeByName("name")	 != NIL) parameter->SetName(node->GetAttributeByName("name")->GetContent());
			if (node->GetAttributeByName("argument") != NIL) parameter->SetArgument(node->GetAttributeByName("argument")->GetContent());
			if (node->GetAttributeByName("enabled")	 != NIL) parameter->SetEnabled(node->GetAttributeByName("enabled")->GetContent() == "true" ? True : False);

			if (node->GetName() == "switch")
			{
				parameter->SetType(PARAMETER_TYPE_SWITCH);
			}
			else if (node->GetName() == "selection")
			{
				parameter->SetType(PARAMETER_TYPE_SELECTION);

				if (node->GetAttributeByName("default") != NIL) parameter->SetDefault(node->GetAttributeByName("default")->GetContent());

				for (Int j = 0; j < node->GetNOfNodes(); j++)
				{
					XML::Node	*node2 = node->GetNthNode(j);

					if (node2->GetName() == "option")
					{
						Option	 *option = new Option();

						option->SetValue(node2->GetContent());

						if (node2->GetAttributeByName("alias") != NIL)	option->SetAlias(node2->GetAttributeByName("alias")->GetContent());
						else						option->SetAlias(option->GetValue());

						option->SetType(OPTION_TYPE_OPTION);

						parameter->AddOption(option);
					}
				}
			}
			else if (node->GetName() == "range")
			{
				parameter->SetType(PARAMETER_TYPE_RANGE);

				if (node->GetAttributeByName("default") != NIL) parameter->SetDefault(node->GetAttributeByName("default")->GetContent());

				if (node->GetAttributeByName("step") != NIL) parameter->SetStepSize(node->GetAttributeByName("step")->GetContent().ToFloat());
				else					     parameter->SetStepSize(1.0);

				for (Int j = 0; j < node->GetNOfNodes(); j++)
				{
					XML::Node	*node2 = node->GetNthNode(j);

					if (node2->GetName() == "min" || node2->GetName() == "max")
					{
						Option	 *option = new Option();

						option->SetValue(node2->GetContent());

						if (node2->GetAttributeByName("alias") != NIL)	option->SetAlias(node2->GetAttributeByName("alias")->GetContent());
						else						option->SetAlias(option->GetValue());

						if	(node2->GetName() == "min") option->SetType(OPTION_TYPE_MIN);
						else if (node2->GetName() == "max") option->SetType(OPTION_TYPE_MAX);

						parameter->AddOption(option);
					}
				}
			}

			external_parameters.Add(parameter);
		}
	}

	return True;
}
