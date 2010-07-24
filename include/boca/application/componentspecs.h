 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_COMPONENTSPECS
#define H_BOCA_COMPONENTSPECS

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;
using namespace smooth::System;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT FileFormat
		{
			private:
				String			 name;
				Array<String>		 extensions;

				String			 taggerID;
				String			 tagFormat;
				Int			 tagMode;
			public:
				const String		&GetName()				{ return name; }
				Void			 SetName(const String &nName)		{ name = nName; }

				const Array<String>	&GetExtensions()			{ return extensions; }
				Void			 AddExtension(const String &nExt)	{ extensions.Add(nExt); }

				const String		&GetTaggerID()				{ return taggerID; }
				Void			 SetTaggerID(const String &nTaggerID)	{ taggerID = nTaggerID; }

				const String		&GetTagFormat()				{ return tagFormat; }
				Void			 SetTagFormat(const String &nTagFormat)	{ tagFormat = nTagFormat; }

				Int			 GetTagMode()				{ return tagMode; }
				Void			 SetTagMode(Int nTagMode)		{ tagMode = nTagMode; }
		};

		class BOCA_DLL_EXPORT TagFormat
		{
			private:
				String			 name;
				Bool			 defaultEnabled;

				Bool			 supportsCoverArt;
				Bool			 coverArtDefault;

				Bool			 supportsFreeEncoding;

				Array<String>		 encodings;
				String			 defaultEncoding;
			public:
				const String		&GetName()				{ return name; }
				Void			 SetName(const String &nName)		{ name = nName; }

				Bool			 IsDefault()				{ return defaultEnabled; }
				Void			 SetDefault(Bool nDefault)		{ defaultEnabled = nDefault; }

				Bool			 IsCoverArtSupported()			{ return supportsCoverArt; }
				Void			 SetCoverArtSupported(Bool nCoverArt)	{ supportsCoverArt = nCoverArt; }

				Bool			 IsCoverArtDefault()			{ return coverArtDefault; }
				Void			 SetCoverArtDefault(Bool nCoverArt)	{ coverArtDefault = nCoverArt; }
				
				Bool			 IsFreeEncodingSupported()		{ return supportsFreeEncoding; }
				Void			 SetFreeEncodingSupported(Bool nAnyEnc)	{ supportsFreeEncoding = nAnyEnc; }

				const Array<String>	&GetEncodings()				{ return encodings; }
				Void			 AddEncoding(const String &nEnc)	{ encodings.Add(nEnc); }

				const String		&GetDefaultEncoding()			{ return defaultEncoding; }
				Void			 SetDefaultEncoding(const String &nDE)	{ defaultEncoding = nDE; }
		};

		class BOCA_DLL_EXPORT Option
		{
			private:
				Int			 type;
				String			 alias;
				String			 value;
			public:
				Int			 GetType()				{ return type; }
				Void			 SetType(Int nType)			{ type = nType; }

				const String		&GetAlias()				{ return alias; }
				Void			 SetAlias(const String &nAlias)		{ alias = nAlias; }

				const String		&GetValue()				{ return value; }
				Void			 SetValue(const String &nValue)		{ value = nValue; }
		};

		class BOCA_DLL_EXPORT Parameter
		{
			private:
				Int			 type;
				String			 name;
				String			 argument;
				Bool			 enabled;
				Float			 stepSize;
				String			 defaultValue;
				Array<Option *>		 options;
			public:
				Int			 GetType()				{ return type; }
				Void			 SetType(Int nType)			{ type = nType; }

				const String		&GetName()				{ return name; }
				Void			 SetName(const String &nName)		{ name = nName; }

				const String		&GetArgument()				{ return argument; }
				Void			 SetArgument(const String &nArgument)	{ argument = nArgument; }

				Bool			 GetEnabled()				{ return enabled; }
				Void			 SetEnabled(Bool nEnabled)		{ enabled = nEnabled; }

				Float			 GetStepSize()				{ return stepSize; }
				Void			 SetStepSize(Float nStepSize)		{ stepSize = nStepSize; }

				const String		&GetDefault()				{ return defaultValue; }
				Void			 SetDefault(const String &nDefault)	{ defaultValue = nDefault; }

				const Array<Option *>	&GetOptions()				{ return options; }
				Void			 AddOption(Option *nOption)		{ options.Add(nOption); }
		};

		class BOCA_DLL_EXPORT ComponentSpecs
		{
			private:
				DynamicLoader		*library;

				String			 componentName;

				Bool			 ParseXMLSpec(const String &);

				Bool			 ParseExternalParameters(XML::Node *);
			public:
				String			 id;
				Int			 type;

				Int			 mode;

				String			 name;
				String			 version;

				Bool			 debug;

				String			 external_command;
				Bool			 external_ignoreExitCode;

				String			 external_arguments;

				String			 external_informat;
				String			 external_outformat;

				Array<Parameter *>	 external_parameters;

				Array<FileFormat *>	 formats;
				Array<TagFormat *>	 tag_formats;

							 ComponentSpecs();
							~ComponentSpecs();

				Bool			 LoadFromDLL(const String &);
				Bool			 LoadFromXML(const String &);

				String			 GetExternalArgumentsString();

				const char		*(*func_GetComponentSpecs)();

				void			*(*func_Create)();
				bool			 (*func_Delete)(void *);

				void			*(*func_GetConfigurationLayer)(void *);
				void			 (*func_FreeConfigurationLayer)(void *);

				bool			 (*func_GetErrorState)(void *);
				const void		*(*func_GetErrorString)(void *);

				bool			 (*func_CanOpenStream)(void *, const wchar_t *);
				int			 (*func_GetStreamInfo)(void *, const wchar_t *, void *);

				void			 (*func_SetVendorString)(void *, const wchar_t *);

				int			 (*func_ParseBuffer)(void *, const void *, void *);
				int			 (*func_ParseStreamInfo)(void *, const wchar_t *, void *);

				int			 (*func_RenderBuffer)(void *, void *, const void *);
				int			 (*func_RenderStreamInfo)(void *, const wchar_t *, const void *);

				int			 (*func_UpdateStreamInfo)(void *, const wchar_t *, const void *);

				int			 (*func_GetPackageSize)(void *);

				int			 (*func_SetDriver)(void *, void *);

				__int64			 (*func_GetInBytes)(void *);

				int			 (*func_CanWrite)(void *);

				int			 (*func_SetPause)(void *, bool);
				bool			 (*func_IsPlaying)(void *);

				bool			 (*func_SetAudioTrackInfo)(void *, const void *);
				void			 (*func_GetFormatInfo)(void *, void *);

				char			*(*func_GetOutputFileExtension)(void *);

				bool			 (*func_Activate)(void *);
				bool			 (*func_Deactivate)(void *);

				int			 (*func_ReadData)(void *, void *, int);
				int			 (*func_WriteData)(void *, void *, int);
				int			 (*func_TransformData)(void *, void *, int);

				int			 (*func_Flush)(void *, void *);

				void			*(*func_GetMainTabLayer)(void *);
				void			*(*func_GetStatusBarLayer)(void *);

				int			 (*func_GetNumberOfDevices)(void *);
				const void		*(*func_GetNthDeviceInfo)(void *, int);

				bool			 (*func_OpenNthDeviceTray)(void *, int);
				bool			 (*func_CloseNthDeviceTray)(void *, int);

				const void		*(*func_GetNthDeviceTrackList)(void *, int);
				const void		*(*func_GetNthDeviceMCDI)(void *, int);
		};
	};

	const Int	 OPTION_TYPE_OPTION		= 0;
	const Int	 OPTION_TYPE_MIN		= 1;
	const Int	 OPTION_TYPE_MAX		= 2;

	const Int	 PARAMETER_TYPE_SWITCH		= 0;
	const Int	 PARAMETER_TYPE_SELECTION	= 1;
	const Int	 PARAMETER_TYPE_RANGE		= 2;

	const Int	 COMPONENT_TYPE_UNKNOWN		= 0;
	const Int	 COMPONENT_TYPE_DECODER		= 1;
	const Int	 COMPONENT_TYPE_ENCODER		= 2;
	const Int	 COMPONENT_TYPE_TAGGER		= 3;
	const Int	 COMPONENT_TYPE_EXTENSION	= 4;
	const Int	 COMPONENT_TYPE_DSP		= 5;
	const Int	 COMPONENT_TYPE_OUTPUT		= 6;
	const Int	 COMPONENT_TYPE_DEVICEINFO	= 7;

	const Int	 INTERNAL			= 0;
	const Int	 EXTERNAL_FILE			= 1;
	const Int	 EXTERNAL_STDIO			= 2;

	const Int	 TAG_MODE_NONE			= 0;
	const Int	 TAG_MODE_PREPEND		= 1;
	const Int	 TAG_MODE_APPEND		= 2;
	const Int	 TAG_MODE_OTHER			= 3;
};

#endif
