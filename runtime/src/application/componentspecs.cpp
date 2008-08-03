 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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

	mode = INTERNAL;
	debug = False;
}

BoCA::AS::ComponentSpecs::~ComponentSpecs()
{
	if (library != NIL) delete library;

	foreach (Format *format, formats) delete format;

	foreach (Parameter *parameter, external_parameters)
	{
		foreach (Option *option, parameter->GetOptions()) delete option;

		delete parameter;
	}
}

Bool BoCA::AS::ComponentSpecs::LoadFromDLL(const String &file)
{
	library = new DynamicLoader(file);

	const char *(*BoCA_GetComponentName)() = (const char *(*)()) library->GetFunctionAddress("BoCA_GetComponentName");

	componentName = BoCA_GetComponentName();

	func_GetComponentSpecs		= (const char *(*)())				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetComponentSpecs"));

	func_Create			= (void *(*)())					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Create"));
	func_Delete			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Delete"));

	func_GetConfigurationLayer	= (void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfigurationLayer"));
	func_FreeConfigurationLayer	= (void (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_FreeConfigurationLayer"));

	func_GetErrorState		= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorState"));
	func_GetErrorString		= (const void *(*)(void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorString"));

	func_CanOpenStream		= (bool (*)(void *, const wchar_t *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenStream"));
	func_GetStreamInfo		= (int (*)(void *, const wchar_t *, void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStreamInfo"));
	func_SetInputFormat		= (void (*)(void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetInputFormat"));

	func_GetPackageSize		= (int (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetPackageSize"));

	func_SetDriver			= (int (*)(void *, void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetDriver"));

	func_GetInBytes			= (__int64 (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetInBytes"));

	func_CanWrite			= (int (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanWrite"));

	func_SetPause			= (int (*)(void *, bool))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetPause"));
	func_IsPlaying			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsPlaying"));

	func_SetAudioTrackInfo		= (bool (*)(void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetAudioTrackInfo"));

	func_GetOutputFileExtension	= (char *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetOutputFileExtension"));

	func_Activate			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Activate"));
	func_Deactivate			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Deactivate"));

	func_ReadData			= (int (*)(void *, void *, int))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadData"));
	func_WriteData			= (int (*)(void *, void *, int))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WriteData"));

	func_GetMainTabLayer		= (void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetMainTabLayer"));

	return ParseXMLSpec(String(func_GetComponentSpecs()).Trim());
}

Bool BoCA::AS::ComponentSpecs::LoadFromXML(const String &file)
{
	IO::InStream	 in(IO::STREAM_FILE, file, IO::IS_READONLY);
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

				arguments.Append(String(param->GetArgument()).Replace("%VALUE", String::FromInt(config->GetIntValue(id, param->GetName(), param->GetDefault().ToInt())))).Append(" ");

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
			else if (node->GetContent() == "dsp")		type = COMPONENT_TYPE_DSP;
			else if (node->GetContent() == "extension")	type = COMPONENT_TYPE_EXTENSION;
			else						type = COMPONENT_TYPE_UNKNOWN;
		}
		else if (node->GetName() == "format")
		{
			Format		*format = new Format();

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "name")	  format->SetName(node2->GetContent());
				else if (node2->GetName() == "extension") format->AddExtension(node2->GetContent());
			}

			formats.Add(format);
		}
		else if (node->GetName() == "external")
		{
			external_tagmode = TAG_MODE_NONE;

			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*node2 = node->GetNthNode(j);

				if	(node2->GetName() == "command")		external_command	= node2->GetContent();
				else if (node2->GetName() == "arguments")	external_arguments	= node2->GetContent();
				else if (node2->GetName() == "informat")	external_informat	= node2->GetContent();
				else if (node2->GetName() == "outformat")	external_outformat	= node2->GetContent();
				else if (node2->GetName() == "mode")		mode			= node2->GetContent() == "file" ? EXTERNAL_FILE : EXTERNAL_STDIO;
				else if (node2->GetName() == "parameters")	ParseExternalParameters(node2);
				else if (node2->GetName() == "tag")
				{
					external_tag	 = node2->GetContent();

					if (node2->GetAttributeByName("mode") != NIL)
					{
						if	(node2->GetAttributeByName("mode")->GetContent() == "prepend")	external_tagmode = TAG_MODE_PREPEND;
						else if (node2->GetAttributeByName("mode")->GetContent() == "append")	external_tagmode = TAG_MODE_APPEND;
						else if (node2->GetAttributeByName("mode")->GetContent() == "other")	external_tagmode = TAG_MODE_OTHER;
					}
				}
			}
		}
	}

	delete document;

	if (mode != INTERNAL)
	{
		if (external_command[1] == ':' && !File(external_command).Exists()) return False;
		if (external_command[1] != ':' && !File(GUI::Application::GetApplicationDirectory().Append(external_command)).Exists()) return False;
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

				if (node->GetAttributeByName("step") != NIL) parameter->SetStepSize(node->GetAttributeByName("step")->GetContent().ToInt());
				else					     parameter->SetStepSize(1);

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
