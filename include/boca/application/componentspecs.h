 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
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
	enum ComponentType
	{
		COMPONENT_TYPE_UNKNOWN = 0,
		COMPONENT_TYPE_DECODER,
		COMPONENT_TYPE_ENCODER,
		COMPONENT_TYPE_TAGGER,
		COMPONENT_TYPE_EXTENSION,
		COMPONENT_TYPE_DSP,
		COMPONENT_TYPE_OUTPUT,
		COMPONENT_TYPE_DEVICEINFO,
		COMPONENT_TYPE_PLAYLIST,
		COMPONENT_TYPE_VERIFIER,

		NUM_COMPONENT_TYPES
	};

	enum ComponentMode
	{
		COMPONENT_MODE_INTERNAL = 0,
		COMPONENT_MODE_EXTERNAL_FILE,
		COMPONENT_MODE_EXTERNAL_STDIO,

		NUM_COMPONENT_MODES
	};

	enum TagMode
	{
		TAG_MODE_NONE = 0,
		TAG_MODE_PREPEND,
		TAG_MODE_APPEND,
		TAG_MODE_OTHER,

		NUM_TAG_MODES
	};

	enum OptionType
	{
		OPTION_TYPE_OPTION = 0,
		OPTION_TYPE_MIN,
		OPTION_TYPE_MAX,

		NUM_OPTION_TYPES
	};

	enum ParameterType
	{
		PARAMETER_TYPE_SWITCH = 0,
		PARAMETER_TYPE_SELECTION,
		PARAMETER_TYPE_RANGE,

		NUM_PARAMETER_TYPES
	};

	namespace AS
	{
		class BOCA_DLL_EXPORT TagFormat
		{
			private:
				String			 name;
				String			 tagger;
				TagMode			 mode;
			public:
							 TagFormat(Int n = 0) : mode(TAG_MODE_NONE)	{ }
							~TagFormat()					{ }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				const String		&GetTagger() const				{ return tagger; }
				Void			 SetTagger(const String &nTagger)		{ tagger = nTagger; }

				TagMode			 GetMode() const				{ return mode; }
				Void			 SetMode(TagMode nMode)				{ mode = nMode; }
		};

		class BOCA_DLL_EXPORT FileFormat
		{
			private:
				String			 name;
				Bool			 lossless;

				Array<String>		 extensions;
				Array<TagFormat>	 tagFormats;
			public:
							 FileFormat(Int n = 0) : lossless(False)	{ }
							~FileFormat()					{ }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				const Bool		&IsLossless() const				{ return lossless; }
				Void			 SetLossless(Bool nLossless)			{ lossless = nLossless; }

				const Array<String>	&GetExtensions() const				{ return extensions; }
				Void			 AddExtension(const String &nExt)		{ extensions.Add(nExt); }

				const Array<TagFormat>	&GetTagFormats() const				{ return tagFormats; }
				Void			 AddTagFormat(const TagFormat &nFormat)		{ tagFormats.Add(nFormat); }
		};

		class BOCA_DLL_EXPORT TagSpec
		{
			private:
				String			 name;
				Bool			 defaultEnabled;

				Bool			 supportsFreeEncoding;

				Array<String>		 encodings;
				String			 defaultEncoding;

				Bool			 supportsCoverArt;
				Bool			 coverArtDefault;
			public:
							 TagSpec(Int n = 0) : defaultEnabled(True),
									      supportsFreeEncoding(True),
									      supportsCoverArt(False),
									      coverArtDefault(True)	{ }
							~TagSpec()					{ }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				Bool			 IsDefault() const				{ return defaultEnabled; }
				Void			 SetDefault(Bool nDefault)			{ defaultEnabled = nDefault; }

				Bool			 IsCoverArtSupported() const			{ return supportsCoverArt; }
				Void			 SetCoverArtSupported(Bool nCoverArt)		{ supportsCoverArt = nCoverArt; }

				Bool			 IsCoverArtDefault() const			{ return coverArtDefault; }
				Void			 SetCoverArtDefault(Bool nCoverArt)		{ coverArtDefault = nCoverArt; }

				Bool			 IsFreeEncodingSupported() const		{ return supportsFreeEncoding; }
				Void			 SetFreeEncodingSupported(Bool nAnyEnc)		{ supportsFreeEncoding = nAnyEnc; }

				const Array<String>	&GetEncodings() const				{ return encodings; }
				Void			 AddEncoding(const String &nEnc)		{ encodings.Add(nEnc); }

				const String		&GetDefaultEncoding() const			{ return defaultEncoding; }
				Void			 SetDefaultEncoding(const String &nDE)		{ defaultEncoding = nDE; }
		};

		class BOCA_DLL_EXPORT Option
		{
			private:
				OptionType		 type;
				String			 alias;
				String			 value;
			public:
							 Option(Int n = 0) : type(OPTION_TYPE_OPTION)	{ }
							~Option()					{ }

				OptionType		 GetType() const				{ return type; }
				Void			 SetType(OptionType nType)			{ type = nType; }

				const String		&GetAlias() const				{ return alias; }
				Void			 SetAlias(const String &nAlias)			{ alias = nAlias; }

				const String		&GetValue() const				{ return value; }
				Void			 SetValue(const String &nValue)			{ value = nValue; }
		};

		class BOCA_DLL_EXPORT Parameter
		{
			private:
				ParameterType		 type;
				String			 name;
				String			 argument;
				Bool			 enabled;
				Float			 stepSize;
				String			 defaultValue;
				Array<Option *>		 options;
			public:
							 Parameter(Int n = 0) : type(PARAMETER_TYPE_SWITCH),
										enabled(False),
										stepSize(1.0)		{ }
							~Parameter()					{ }

				ParameterType		 GetType() const				{ return type; }
				Void			 SetType(ParameterType nType)			{ type = nType; }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				const String		&GetArgument() const				{ return argument; }
				Void			 SetArgument(const String &nArgument)		{ argument = nArgument; }

				Bool			 GetEnabled() const				{ return enabled; }
				Void			 SetEnabled(Bool nEnabled)			{ enabled = nEnabled; }

				Float			 GetStepSize() const				{ return stepSize; }
				Void			 SetStepSize(Float nStepSize)			{ stepSize = nStepSize; }

				const String		&GetDefault() const				{ return defaultValue; }
				Void			 SetDefault(const String &nDefault)		{ defaultValue = nDefault; }

				const Array<Option *>	&GetOptions() const				{ return options; }
				Void			 AddOption(Option *nOption)			{ options.Add(nOption); }
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
				ComponentType		 type;

				ComponentMode		 mode;

				String			 name;
				String			 version;

				Bool			 threadSafe;
				Bool			 debug;

				Array<String>		 requireComponents;
				Array<String>		 replaceComponents;
				Array<String>		 conflictComponents;
				Array<String>		 precedeComponents;
				Array<String>		 succeedComponents;

				String			 external_command;
				Bool			 external_ignoreExitCode;

				String			 external_arguments;

				String			 external_informat;
				String			 external_outformat;

				Array<Parameter *>	 external_parameters;

				Array<FileFormat *>	 formats;
				Array<TagSpec *>	 tag_specs;

							 ComponentSpecs();
							~ComponentSpecs();

				Bool			 LoadFromDLL(const String &);
				Bool			 LoadFromXML(const String &);

				String			 GetExternalArgumentsString();

				const char		*(*func_GetComponentSpecs)();

				void			*(*func_Create)();
				bool			 (*func_Delete)(void *);

				void			*(*func_GetConfigurationLayer)(void *);

				bool			 (*func_GetErrorState)(const void *);
				const void		*(*func_GetErrorString)(const void *);

				const void		*(*func_GetConfiguration)(void *);
				bool			 (*func_SetConfiguration)(void *, const void *);

				bool			 (*func_CanOpenStream)(void *, const wchar_t *);
				bool			 (*func_CanVerifyTrack)(void *, const void *);

				int			 (*func_GetStreamInfo)(void *, const wchar_t *, void *);
				void			 (*func_GetFormatInfo)(void *, void *);

				bool			 (*func_SetAudioTrackInfo)(void *, const void *);
				void			 (*func_SetVendorString)(void *, const wchar_t *);

				int			 (*func_ParseBuffer)(void *, const void *, void *);
				int			 (*func_ParseStreamInfo)(void *, const wchar_t *, void *);

				int			 (*func_RenderBuffer)(void *, void *, const void *);
				int			 (*func_RenderStreamInfo)(void *, const wchar_t *, const void *);

				int			 (*func_UpdateStreamInfo)(void *, const wchar_t *, const void *);

				int			 (*func_GetPackageSize)(void *);

				int			 (*func_SetDriver)(void *, void *);

				__int64			 (*func_GetInBytes)(const void *);

				int			 (*func_CanWrite)(void *);

				int			 (*func_SetPause)(void *, bool);
				bool			 (*func_IsPlaying)(void *);

				char			*(*func_GetOutputFileExtension)(void *);
				int			 (*func_GetNumberOfPasses)(void *);

				bool			 (*func_IsLossless)(void *);

				bool			 (*func_Activate)(void *);
				bool			 (*func_Deactivate)(void *);

				bool			 (*func_Seek)(void *, __int64);
				bool			 (*func_NextPass)(void *);

				int			 (*func_ReadData)(void *, void *, int);
				int			 (*func_WriteData)(void *, void *, int);
				int			 (*func_TransformData)(void *, void *, int);
				int			 (*func_ProcessData)(void *, void *);

				int			 (*func_Flush)(void *, void *);

				bool			 (*func_Verify)(void *);

				void			*(*func_GetMainTabLayer)(void *);
				void			*(*func_GetStatusBarLayer)(void *);

				int			 (*func_GetNumberOfDevices)(void *);
				const void		*(*func_GetNthDeviceInfo)(void *, int);

				bool			 (*func_OpenNthDeviceTray)(void *, int);
				bool			 (*func_CloseNthDeviceTray)(void *, int);

				const void		*(*func_GetNthDeviceTrackList)(void *, int);
				const void		*(*func_GetNthDeviceMCDI)(void *, int);

				void			 (*func_SetTrackList)(void *, const void *);

				bool			 (*func_CanOpenFile)(void *, const wchar_t *);

				const void		*(*func_ReadPlaylist)(void *, const wchar_t *);
				int			 (*func_WritePlaylist)(void *, const wchar_t *);
		};
	};
};

#endif
