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

#include <boca/application/componentspecs.h>
#include <boca/common/config.h>

static Array< Array<String> >				 formatCompanions;

static Array< Bool >					 parameterHidden;
static Array< Array<BoCA::ParameterRequirement> >	 parameterRequirements;
static Array< Array<BoCA::ParameterDependency> >	 parameterDependencies;

const Array<String> &BoCA::AS::FileFormat::GetCompanionExtensions() const
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	return formatCompanions.Get(index);
}

Void BoCA::AS::FileFormat::AddCompanionExtension(const String &nExt)
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	if (GetCompanionExtensions().Length() == 0) formatCompanions.Add(Array<String>(), index);

	formatCompanions.GetReference(index).Add(nExt);
}

Bool BoCA::AS::Parameter::GetHidden() const
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	return parameterHidden.Get(index);
}

Void BoCA::AS::Parameter::SetHidden(Bool nHidden)
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	if (!parameterHidden.Set(index, nHidden))
		parameterHidden.Add(nHidden, index);
}

const Array<BoCA::ParameterRequirement> &BoCA::AS::Parameter::GetRequirements() const
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	return parameterRequirements.Get(index);
}

Void BoCA::AS::Parameter::AddRequirement(const BoCA::ParameterRequirement &nReq)
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	if (GetRequirements().Length() == 0) parameterRequirements.Add(Array<ParameterRequirement>(), index);

	parameterRequirements.GetReference(index).Add(nReq);
}

const Array<BoCA::ParameterDependency> &BoCA::AS::Parameter::GetDependencies() const
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	return parameterDependencies.Get(index);
}

Void BoCA::AS::Parameter::AddDependency(const BoCA::ParameterDependency &nDep)
{
	Int	 index = Number(Int64(this)).ToHexString().ComputeCRC32();

	if (GetDependencies().Length() == 0) parameterDependencies.Add(Array<ParameterDependency>(), index);

	parameterDependencies.GetReference(index).Add(nDep);
}

BoCA::AS::ComponentSpecs::ComponentSpecs()
{
	library	   = NIL;

	type	   = COMPONENT_TYPE_UNKNOWN;
	mode	   = COMPONENT_MODE_INTERNAL;

	threadSafe = True;
	debug	   = False;

	external_ignoreExitCode = False;

	external_md5_stderr = False;

	/* Init component function pointers.
	 */
	func_GetComponentSpecs		= NIL;

	func_Create			= NIL;
	func_Delete			= NIL;

	func_GetConfigurationLayer	= NIL;

	func_GetErrorState		= NIL;
	func_GetErrorString		= NIL;

	func_GetConfiguration		= NIL;
	func_SetConfiguration		= NIL;

	func_CanOpenStream		= NIL;
	func_CanVerifyTrack		= NIL;

	func_GetStreamInfo		= NIL;
	func_GetFormatInfo		= NIL;

	func_SetAudioTrackInfo		= NIL;
	func_SetVendorString		= NIL;

	func_ParseBuffer		= NIL;
	func_ParseStreamInfo		= NIL;

	func_RenderBuffer		= NIL;
	func_RenderStreamInfo		= NIL;

	func_UpdateStreamInfo		= NIL;

	func_SetDriver			= NIL;

	func_GetInBytes			= NIL;

	func_Finish			= NIL;

	func_CanWrite			= NIL;

	func_SetPause			= NIL;
	func_IsPlaying			= NIL;

	func_SetOutputFormat		= NIL;
	func_GetOutputFileExtension	= NIL;

	func_GetNumberOfPasses		= NIL;

	func_IsThreadSafe		= NIL;

	func_IsLossless			= NIL;

	func_Activate			= NIL;
	func_Deactivate			= NIL;

	func_Seek			= NIL;
	func_NextPass			= NIL;

	func_ReadData			= NIL;
	func_WriteData			= NIL;
	func_TransformData		= NIL;
	func_ProcessData		= NIL;

	func_Flush			= NIL;

	func_Verify			= NIL;

	func_GetMainTabLayer		= NIL;
	func_GetStatusBarLayer		= NIL;

	func_GetNumberOfDevices		= NIL;
	func_GetNthDeviceInfo		= NIL;

	func_IsNthDeviceTrayOpen	= NIL;

	func_OpenNthDeviceTray		= NIL;
	func_CloseNthDeviceTray		= NIL;

	func_GetNthDeviceTrackList	= NIL;
	func_GetNthDeviceMCDI		= NIL;

	func_SetTrackList		= NIL;

	func_CanOpenFile		= NIL;

	func_ReadPlaylist		= NIL;
	func_WritePlaylist		= NIL;
}

BoCA::AS::ComponentSpecs::~ComponentSpecs()
{
	if (library != NIL) delete library;

	foreach (InputSpec *input, inputs) delete input;

	foreach (FileFormat *format, formats) delete format;
	foreach (TagSpec *tag, tags) delete tag;

	foreach (Parameter *parameter, parameters)
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

	if (BoCA_GetComponentName == NIL) return False;

	componentName = BoCA_GetComponentName();

	/* Get component function pointers.
	 */
	func_GetComponentSpecs		= (const char *(*)())					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetComponentSpecs"));

	func_Create			= (void *(*)())						library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Create"));
	func_Delete			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Delete"));

	func_GetConfigurationLayer	= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfigurationLayer"));

	func_GetErrorState		= (bool (*)(const void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorState"));
	func_GetErrorString		= (const void *(*)(const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetErrorString"));

	func_GetConfiguration		= (const void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetConfiguration"));
	func_SetConfiguration		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetConfiguration"));

	func_CanOpenStream		= (bool (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenStream"));
	func_CanVerifyTrack		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanVerifyTrack"));

	func_GetStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStreamInfo"));
	func_GetFormatInfo		= (const void *(*)(void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetFormatInfo"));

	func_SetAudioTrackInfo		= (bool (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetAudioTrackInfo"));
	func_SetVendorString		= (void (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetVendorString"));

	func_ParseBuffer		= (int (*)(void *, const void *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseBuffer"));
	func_ParseStreamInfo		= (int (*)(void *, const wchar_t *, void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ParseStreamInfo"));

	func_RenderBuffer		= (int (*)(void *, void *, const void *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderBuffer"));
	func_RenderStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_RenderStreamInfo"));

	func_UpdateStreamInfo		= (int (*)(void *, const wchar_t *, const void *))	library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_UpdateStreamInfo"));

	func_SetDriver			= (void (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetDriver"));

	func_GetInBytes			= (__int64 (*)(const void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetInBytes"));

	func_Finish			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Finish"));

	func_CanWrite			= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanWrite"));

	func_SetPause			= (int (*)(void *, bool))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetPause"));
	func_IsPlaying			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsPlaying"));

	func_SetOutputFormat		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetOutputFormat"));
	func_GetOutputFileExtension	= (char *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetOutputFileExtension"));

	func_GetNumberOfPasses		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNumberOfPasses"));

	func_IsThreadSafe		= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsThreadSafe"));

	func_IsLossless			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsLossless"));

	func_Activate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Activate"));
	func_Deactivate			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Deactivate"));

	func_Seek			= (bool (*)(void *, __int64))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Seek"));
	func_NextPass			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_NextPass"));

	func_ReadData			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadData"));
	func_WriteData			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WriteData"));
	func_TransformData		= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_TransformData"));
	func_ProcessData		= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ProcessData"));

	func_Flush			= (int (*)(void *, void *))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Flush"));

	func_Verify			= (bool (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_Verify"));

	func_GetMainTabLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetMainTabLayer"));
	func_GetStatusBarLayer		= (void *(*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetStatusBarLayer"));

	func_GetNumberOfDevices		= (int (*)(void *))					library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNumberOfDevices"));
	func_GetNthDeviceInfo		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceInfo"));

	func_IsNthDeviceTrayOpen	= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_IsNthDeviceTrayOpen"));

	func_OpenNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_OpenNthDeviceTray"));
	func_CloseNthDeviceTray		= (bool (*)(void *, int))				library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CloseNthDeviceTray"));

	func_GetNthDeviceTrackList	= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceTrackList"));
	func_GetNthDeviceMCDI		= (const void *(*)(void *, int))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_GetNthDeviceMCDI"));

	func_SetTrackList		= (void (*)(void *, const void *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_SetTrackList"));

	func_CanOpenFile		= (bool (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_CanOpenFile"));

	func_ReadPlaylist		= (const void *(*)(void *, const wchar_t *))		library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_ReadPlaylist"));
	func_WritePlaylist		= (int (*)(void *, const wchar_t *))			library->GetFunctionAddress(String("BoCA_").Append(componentName).Append("_WritePlaylist"));

	/* Parse component description.
	 */
	String::InputFormat	 inputFormat("UTF-8");

	return ParseXMLSpec(String(func_GetComponentSpecs()).Trim());
}

Bool BoCA::AS::ComponentSpecs::LoadFromXML(const String &file)
{
	/* Parse component description from external script.
	 */
	String::InputFormat	 inputFormat("UTF-8");
	IO::InStream		 in(IO::STREAM_FILE, file, IO::IS_READ);

	return ParseXMLSpec(in.InputString(in.Size()).Trim());
}

String BoCA::AS::ComponentSpecs::GetExternalArgumentsString(const Config *config)
{
	/* Get number of threads to use.
	 */
	Bool	 enableParallel = config->GetIntValue("Resources", "EnableParallelConversions", True);
	Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True);
	Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

	if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

	/* Build arguments string.
	 */
	String	 arguments;

	foreach (Parameter *param, parameters)
	{
		String	 paramName    = param->GetName();
		Bool	 paramEnabled = param->GetEnabled();
		String	 paramDefault = param->GetDefault().Replace("%THREADS", String::FromInt(numberOfThreads));

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				if (!config->GetIntValue(id, paramName, paramEnabled)) continue;

				arguments.Append(param->GetArgument()).Append(" ");

				break;
			case PARAMETER_TYPE_SELECTION:
				if (!config->GetIntValue(id, String("Set ").Append(paramName), paramEnabled)) continue;

				arguments.Append(param->GetArgument().Replace("%VALUE", config->GetStringValue(id, paramName, paramDefault))).Append(" ");

				break;
			case PARAMETER_TYPE_RANGE:
			{
				if (!config->GetIntValue(id, String("Set ").Append(paramName), paramEnabled)) continue;

				Float	 value = config->GetIntValue(id, paramName, Math::Round(paramDefault.ToFloat() / param->GetStepSize())) * param->GetStepSize();

				Float	 min   = 0;
				Float	 max   = 0;

				foreach(Option * option, param->GetOptions())
				{
					if	(option->GetType() == OPTION_TYPE_MIN) min = option->GetValue().ToInt();
					else if (option->GetType() == OPTION_TYPE_MAX) max = option->GetValue().ToInt();
				}

				arguments.Append(param->GetArgument().Replace("%VALUE", String::FromFloat(Math::Max(min, Math::Min(value, max))))).Append(" ");

				break;
			}
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	if (config->GetIntValue(id, "Set Additional arguments", False)) arguments.Append(" ").Append(config->GetStringValue(id, "Additional arguments", NIL));

	return arguments.Trim();
}

Bool BoCA::AS::ComponentSpecs::ParseXMLSpec(const String &xml)
{
	if (xml == NIL) return False;

	/* Places for external commands.
	 */
#if defined __WIN32__
	static const char	*places[] = { "%APPDIR\\codecs\\cmdline\\%COMMAND", "%APPDIR\\codecs\\cmdline\\%COMMAND.exe", NIL };
#elif defined __APPLE__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "%APPDIR/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/opt/local/bin/%COMMAND", "/sw/bin/%COMMAND", NIL };
#elif defined __HAIKU__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "%APPDIR/%COMMAND", "/system/bin/%COMMAND", "/system/non-packaged/bin/%COMMAND", NIL };
#elif defined __NetBSD__
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "%APPDIR/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", "/usr/pkg/bin/%COMMAND", NIL };
#else
	static const char	*places[] = { "%APPDIR/codecs/cmdline/%COMMAND", "%APPDIR/%COMMAND", "/usr/bin/%COMMAND", "/usr/local/bin/%COMMAND", NIL };
#endif

	XML::Document	 document;
	const char	*xmlUtf8 = xml.ConvertTo("UTF-8");

	document.ParseMemory(xmlUtf8, strlen(xmlUtf8));

	XML::Node	*root = document.GetRootNode();

	if (root->GetName() != "component") return False;

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

			if (node->GetAttributeByName("debug") != NIL) debug = (node->GetAttributeByName("debug")->GetContent() == "true");
		}
		else if (node->GetName() == "type")
		{
			if	(node->GetContent() == "decoder")	type = COMPONENT_TYPE_DECODER;
			else if (node->GetContent() == "encoder")	type = COMPONENT_TYPE_ENCODER;
			else if (node->GetContent() == "output")	type = COMPONENT_TYPE_OUTPUT;
			else if (node->GetContent() == "deviceinfo")	type = COMPONENT_TYPE_DEVICEINFO;
			else if (node->GetContent() == "dsp")		type = COMPONENT_TYPE_DSP;
			else if (node->GetContent() == "extension")	type = COMPONENT_TYPE_EXTENSION;
			else if (node->GetContent() == "playlist")	type = COMPONENT_TYPE_PLAYLIST;
			else if (node->GetContent() == "tagger")	type = COMPONENT_TYPE_TAGGER;
			else if (node->GetContent() == "verifier")	type = COMPONENT_TYPE_VERIFIER;
			else						type = COMPONENT_TYPE_UNKNOWN;

			if (node->GetAttributeByName("threadSafe") != NIL) threadSafe = (node->GetAttributeByName("threadSafe")->GetContent() == "true");
		}
		else if (node->GetName() == "require")
		{
			requireComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "replace")
		{
			replaceComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "conflict")
		{
			conflictComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "precede")
		{
			precedeComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "succeed")
		{
			succeedComponents.Add(node->GetContent());
		}
		else if (node->GetName() == "input")
		{
			InputSpec	*input = new InputSpec();

			input->SetFloat(False);
			input->SetSigned(True);

			if (node->GetAttributeByName("float")	 != NIL) input->SetFloat(node->GetAttributeByName("float")->GetContent() == "true");
			if (node->GetAttributeByName("signed")	 != NIL) input->SetSigned(node->GetAttributeByName("signed")->GetContent() == "true");

			input->SetBits(input->GetFloat() ? "32" : "8-32");
			input->SetChannels("1-255");
			input->SetRate("1-768000");

			if (node->GetAttributeByName("bits")	 != NIL) input->SetBits(node->GetAttributeByName("bits")->GetContent());
			if (node->GetAttributeByName("channels") != NIL) input->SetChannels(node->GetAttributeByName("channels")->GetContent());
			if (node->GetAttributeByName("rate")	 != NIL) input->SetRate(node->GetAttributeByName("rate")->GetContent());

			inputs.Add(input);
		}
		else if (node->GetName() == "format")
		{
			FileFormat	*format = new FileFormat();

			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

				if	(node2->GetName() == "name")	  format->SetName(node2->GetContent());
				else if (node2->GetName() == "lossless")  format->SetLossless(node2->GetContent() == "true");
				else if (node2->GetName() == "extension") format->AddExtension(node2->GetContent());
				else if (node2->GetName() == "companion") format->AddCompanionExtension(node2->GetContent());
				else if (node2->GetName() == "tag")
				{
					TagFormat	 tagFormat;

					tagFormat.SetName(node2->GetContent());

					if (node2->GetAttributeByName("id") != NIL) tagFormat.SetTagger(node2->GetAttributeByName("id")->GetContent());

					if (node2->GetAttributeByName("mode") != NIL)
					{
						if	(node2->GetAttributeByName("mode")->GetContent() == "prepend")	tagFormat.SetMode(TAG_MODE_PREPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "append")	tagFormat.SetMode(TAG_MODE_APPEND);
						else if (node2->GetAttributeByName("mode")->GetContent() == "other")	tagFormat.SetMode(TAG_MODE_OTHER);
					}

					format->AddTagFormat(tagFormat);
				}
			}

			formats.Add(format);
		}
		else if (node->GetName() == "tagspec")
		{
			TagSpec	*tag = new TagSpec();

			tag->SetDefault(True);

			tag->SetCoverArtSupported(False);
			tag->SetCoverArtDefault(True);

			tag->SetPrependZeroAllowed(False);
			tag->SetPrependZeroDefault(False);

			tag->SetFreeEncodingSupported(False);

			if (node->GetAttributeByName("default") != NIL) tag->SetDefault(node->GetAttributeByName("default")->GetContent() == "true");

			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

				if	(node2->GetName() == "name")	 tag->SetName(node2->GetContent());
				else if	(node2->GetName() == "coverart")
				{
					if (node2->GetAttributeByName("supported") != NIL) tag->SetCoverArtSupported(node2->GetAttributeByName("supported")->GetContent() == "true");
					if (node2->GetAttributeByName("default")   != NIL) tag->SetCoverArtDefault(node2->GetAttributeByName("default")->GetContent() == "true");
				}
				else if	(node2->GetName() == "prependzero")
				{
					if (node2->GetAttributeByName("allowed") != NIL) tag->SetPrependZeroAllowed(node2->GetAttributeByName("allowed")->GetContent() == "true");
					if (node2->GetAttributeByName("default") != NIL) tag->SetPrependZeroDefault(node2->GetAttributeByName("default")->GetContent() == "true");
				}
				else if (node2->GetName() == "encodings")
				{
					if (node2->GetAttributeByName("free") != NIL) tag->SetFreeEncodingSupported(node2->GetAttributeByName("free")->GetContent() == "true");

					for (Int i = 0; i < node2->GetNOfNodes(); i++)
					{
						XML::Node	*node3 = node2->GetNthNode(i);

						if (node3->GetName() == "encoding")
						{
							tag->AddEncoding(node3->GetContent());

							if (tag->GetEncodings().Length() == 1) tag->SetDefaultEncoding(node3->GetContent());

							if (node3->GetAttributeByName("default") != NIL)
							{
								if (node3->GetAttributeByName("default")->GetContent() == "true") tag->SetDefaultEncoding(node3->GetContent());
							}
						}
					}
				}
			}

			tags.Add(tag);
		}
		else if (node->GetName() == "external" && external_command == NIL)
		{
			/* Find external command name.
			 */
			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

				if (node2->GetName() == "command")
				{
					external_command = node2->GetContent();

					if (node2->GetAttributeByName("ignoreExitCode") != NIL) external_ignoreExitCode = (node2->GetAttributeByName("ignoreExitCode")->GetContent() == "true");
				}
			}

			/* Find absolute path to external command.
			 */
			if (external_command[0] != '/' && external_command[1] != ':')
			{
				for (Int i = 0; places[i] != NIL; i++)
				{
					String	 delimiter = Directory::GetDirectoryDelimiter();
					String	 file	   = String(places[i]).Replace("%APPDIR", GUI::Application::GetApplicationDirectory()).Replace("%COMMAND", external_command).Replace(String(delimiter).Append(delimiter), delimiter);

					if (File(file).Exists()) { external_command = file; break; }
				}

				if (external_command[0] != '/' && external_command[1] != ':') external_command = NIL;
			}

			/* Check if external command actually exists.
			 */
			if (!File(external_command).Exists())
			{
				mode		 = COMPONENT_MODE_EXTERNAL_STDIO;
				external_command = NIL;

				continue;
			}

			/* Find arguments and parameters.
			 */
			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

				if (node2->GetName() == "md5")
				{
					external_md5_arguments = node2->GetContent();

					if (node2->GetAttributeByName("stream")	 != NIL) external_md5_stderr  = (node2->GetAttributeByName("stream")->GetContent() == "stderr");
					if (node2->GetAttributeByName("require") != NIL) external_md5_require =  node2->GetAttributeByName("require")->GetContent();
					if (node2->GetAttributeByName("prefix")	 != NIL) external_md5_prefix  =  node2->GetAttributeByName("prefix")->GetContent();
				}
				else if (node2->GetName() == "arguments")  external_arguments	= node2->GetContent();
				else if (node2->GetName() == "informat")   external_informat	= node2->GetContent();
				else if (node2->GetName() == "outformat")  external_outformat	= node2->GetContent();
				else if (node2->GetName() == "mode")	   mode			= node2->GetContent() == "file" ? COMPONENT_MODE_EXTERNAL_FILE : COMPONENT_MODE_EXTERNAL_STDIO;
				else if (node2->GetName() == "parameters") ParseParameters(node2);
			}
		}
		else if (node->GetName() == "parameters")
		{
			ParseParameters(node);
		}
	}

	/* Check for presence of basic parameters.
	 */
	if (id == NIL || type == COMPONENT_TYPE_UNKNOWN) return False;

	/* Report an error if no external command could be found in external mode.
	 */
	if (mode != COMPONENT_MODE_INTERNAL && external_command == NIL) return False;

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseParameters(XML::Node *root)
{
	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if (node->GetName() != "switch" && node->GetName() != "selection" && node->GetName() != "range") continue;

		Parameter	*parameter = new Parameter();

		parameter->SetEnabled(False);

		if (node->GetAttributeByName("name")	 != NIL) parameter->SetName(node->GetAttributeByName("name")->GetContent());
		if (node->GetAttributeByName("argument") != NIL) parameter->SetArgument(node->GetAttributeByName("argument")->GetContent());
		if (node->GetAttributeByName("enabled")	 != NIL) parameter->SetEnabled(node->GetAttributeByName("enabled")->GetContent() == "true" ? True : False);
		if (node->GetAttributeByName("hidden")	 != NIL) parameter->SetHidden(node->GetAttributeByName("hidden")->GetContent() == "true" ? True : False);

		if (node->GetName() == "switch")
		{
			parameter->SetType(PARAMETER_TYPE_SWITCH);
		}
		else if (node->GetName() == "selection")
		{
			parameter->SetType(PARAMETER_TYPE_SELECTION);

			if (node->GetAttributeByName("default") != NIL) parameter->SetDefault(node->GetAttributeByName("default")->GetContent());

			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

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

			for (Int i = 0; i < node->GetNOfNodes(); i++)
			{
				XML::Node	*node2 = node->GetNthNode(i);

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

		ParseParameterRequirements(parameter, node);
		ParseParameterDependencies(parameter, node);

		parameters.Add(parameter);
	}

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseParameterRequirements(Parameter *parameter, XML::Node *node)
{
	for (Int i = 0; i < node->GetNOfNodes(); i++)
	{
		XML::Node	*node2 = node->GetNthNode(i);

		if (node2->GetName() == "requires")
		{
			ParameterRequirement	 requirement;

			requirement.option    = node2->GetAttributeByName("option")    != NIL ?	 node2->GetAttributeByName("option")->GetContent()		: String();
			requirement.arguments = node2->GetAttributeByName("arguments") != NIL ?	 node2->GetAttributeByName("arguments")->GetContent()		: String();
			requirement.useStderr = node2->GetAttributeByName("stream")    != NIL ? (node2->GetAttributeByName("stream")->GetContent() == "stderr")	: False;

			if (requirement.option != NIL) parameter->AddRequirement(requirement);
		}
	}

	return True;
}

Bool BoCA::AS::ComponentSpecs::ParseParameterDependencies(Parameter *parameter, XML::Node *node)
{
	for (Int i = 0; i < node->GetNOfNodes(); i++)
	{
		XML::Node	*node2 = node->GetNthNode(i);

		if (node2->GetName() == "depends")
		{
			ParameterDependency	 dependency;

			dependency.setting = node2->GetAttributeByName("setting") != NIL ? node2->GetAttributeByName("setting")->GetContent()		 : String();
			dependency.state   = node2->GetAttributeByName("state")	  != NIL ? node2->GetAttributeByName("state")->GetContent() == "enabled" : True;
			dependency.value   = node2->GetAttributeByName("value")	  != NIL ? node2->GetAttributeByName("value")->GetContent()		 : String();

			if (dependency.setting != NIL) parameter->AddDependency(dependency);
		}
	}

	return True;
}
