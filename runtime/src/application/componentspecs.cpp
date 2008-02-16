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

BoCA::AS::ComponentSpecs::ComponentSpecs()
{
	library = NIL;
}

BoCA::AS::ComponentSpecs::~ComponentSpecs()
{
	if (library != NIL) delete library;
}

Bool BoCA::AS::ComponentSpecs::LoadFromFile(const String &file)
{
	library = new DynamicLoader(file);

	const char *(*BoCA_GetComponentName)() = (const char *(*)()) library->GetFunctionAddress("BoCA_GetComponentName");

	componentName = BoCA_GetComponentName();

	func_GetComponentSpecs		= (const char *(*)())				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetComponentSpecs"));

	func_Create			= (void *(*)())					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Create"));
	func_Delete			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Delete"));

	func_GetConfigurationLayer	= (void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfigurationLayer"));

	func_GetErrorState		= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorState"));
	func_GetErrorString		= (const void *(*)(void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorString"));

	func_CanOpenStream		= (bool (*)(void *, const char *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenStream"));
	func_GetStreamInfo		= (int (*)(void *, const char *, void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStreamInfo"));
	func_SetInputFormat		= (void (*)(void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetInputFormat"));

	func_GetPackageSize		= (int (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetPackageSize"));

	func_SetDriver			= (int (*)(void *, void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetDriver"));

	func_GetInBytes			= (__int64 (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetInBytes"));

	func_CanWrite			= (int (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanWrite"));

	func_SetPause			= (int (*)(void *, bool))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetPause"));
	func_IsPlaying			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsPlaying"));

	func_SetAudioTrackInfo		= (bool (*)(void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetAudioTrackInfo"));

	func_Activate			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Activate"));
	func_Deactivate			= (bool (*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Deactivate"));

	func_ReadData			= (int (*)(void *, void *, int))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadData"));
	func_WriteData			= (int (*)(void *, void *, int))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WriteData"));

	ParseXMLSpec();

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseXMLSpec()
{
	String		 xml = String(func_GetComponentSpecs()).Trim();
	XML::Document	*document = new XML::Document();

	document->ParseMemory((void *) (char *) xml, xml.Length());

	XML::Node	*root = document->GetRootNode();

	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if	(node->GetName() == "name")	name	= node->GetContent();
		else if (node->GetName() == "version")	version	= node->GetContent();
		else if (node->GetName() == "id")	id	= node->GetContent();
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
	}

	delete document;

	return True;
}
