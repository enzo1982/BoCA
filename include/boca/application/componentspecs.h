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

#ifndef H_BOCA_COMPONENTSPECS
#define H_BOCA_COMPONENTSPECS

#include "../common/config.h"

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

	struct ParameterRequirement
	{
		String	 option;
		String	 arguments;
		Bool	 useStderr;
	};

	struct ParameterDependency
	{
		String	 setting;
		Bool	 state;
		String	 value;
	};

	namespace AS
	{
		class BOCA_DLL_EXPORT InputSpec
		{
			private:
				Bool			 fp;
				Bool			 sign;

				String			 bits;
				String			 channels;
				String			 rate;
			public:
							 InputSpec() : fp(False),
								       sign(True),
								       bits("8-32"),
								       channels("1-255"),
								       rate("1-768000")			{ }
							~InputSpec()					{ }

				Bool			 GetFloat() const				{ return fp; }
				Void			 SetFloat(Bool nFp)				{ fp = nFp; }

				Bool			 GetSigned() const				{ return sign; }
				Void			 SetSigned(Bool nSign)				{ sign = nSign; }

				const String		&GetBits() const				{ return bits; }
				Void			 SetBits(const String &nBits)			{ bits = nBits; }

				const String		&GetChannels() const				{ return channels; }
				Void			 SetChannels(const String &nChannels)		{ channels = nChannels; }

				const String		&GetRate() const				{ return rate; }
				Void			 SetRate(const String &nRate)			{ rate = nRate; }
		};

		class BOCA_DLL_EXPORT TagFormat
		{
			private:
				String			 name;
				String			 tagger;
				TagMode			 mode;
			public:
							 TagFormat() : mode(TAG_MODE_NONE)		{ }
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
							 FileFormat() : lossless(False)			{ }
							~FileFormat()					{ }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				const Bool		&IsLossless() const				{ return lossless; }
				Void			 SetLossless(Bool nLossless)			{ lossless = nLossless; }

				const Array<String>	&GetExtensions() const				{ return extensions; }
				Void			 AddExtension(const String &nExt)		{ extensions.Add(nExt); }

				const Array<String>	&GetCompanionExtensions() const;
				Void			 AddCompanionExtension(const String &);

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

				Bool			 allowsPrependZero;
				Bool			 prependZeroDefault;
			public:
							 TagSpec() : defaultEnabled(True),
								     supportsFreeEncoding(True),
								     supportsCoverArt(False),
								     coverArtDefault(True),
								     allowsPrependZero(False),
								     prependZeroDefault(True)		{ }
							~TagSpec()					{ }

				const String		&GetName() const				{ return name; }
				Void			 SetName(const String &nName)			{ name = nName; }

				Bool			 IsDefault() const				{ return defaultEnabled; }
				Void			 SetDefault(Bool nDefault)			{ defaultEnabled = nDefault; }

				Bool			 IsCoverArtSupported() const			{ return supportsCoverArt; }
				Void			 SetCoverArtSupported(Bool nCoverArt)		{ supportsCoverArt = nCoverArt; }

				Bool			 IsCoverArtDefault() const			{ return coverArtDefault; }
				Void			 SetCoverArtDefault(Bool nCoverArt)		{ coverArtDefault = nCoverArt; }

				Bool			 IsPrependZeroAllowed() const			{ return allowsPrependZero; }
				Void			 SetPrependZeroAllowed(Bool nPrependZero)	{ allowsPrependZero = nPrependZero; }

				Bool			 IsPrependZeroDefault() const			{ return prependZeroDefault; }
				Void			 SetPrependZeroDefault(Bool nPrependZero)	{ prependZeroDefault = nPrependZero; }

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
							 Option() : type(OPTION_TYPE_OPTION)		{ }
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
				ParameterType				 type;
				String					 name;
				String					 argument;
				Bool					 enabled;
				Float					 stepSize;
				String					 defaultValue;
				Array<Option *>				 options;
			public:
									 Parameter() : type(PARAMETER_TYPE_SWITCH),
										       enabled(False),
										       stepSize(1.0)			{ }
									~Parameter()					{ }

				ParameterType				 GetType() const				{ return type; }
				Void					 SetType(ParameterType nType)			{ type = nType; }

				const String				&GetName() const				{ return name; }
				Void					 SetName(const String &nName)			{ name = nName; }

				const String				&GetArgument() const				{ return argument; }
				Void					 SetArgument(const String &nArgument)		{ argument = nArgument; }

				Bool					 GetEnabled() const				{ return enabled; }
				Void					 SetEnabled(Bool nEnabled)			{ enabled = nEnabled; }

				Float					 GetStepSize() const				{ return stepSize; }
				Void					 SetStepSize(Float nStepSize)			{ stepSize = nStepSize; }

				const String				&GetDefault() const				{ return defaultValue; }
				Void					 SetDefault(const String &nDefault)		{ defaultValue = nDefault; }

				const Array<Option *>			&GetOptions() const				{ return options; }
				Void					 AddOption(Option *nOption)			{ options.Add(nOption); }

				const Array<ParameterDependency>	&GetDependencies() const;
				Void					 AddDependency(const ParameterDependency &);

				Bool					 GetHidden() const;
				Void					 SetHidden(Bool nHidden);

				const Array<ParameterRequirement>	&GetRequirements() const;
				Void					 AddRequirement(const ParameterRequirement &);
		};

		class BOCA_DLL_EXPORT ComponentSpecs
		{
			private:
				DynamicLoader		*library;

				String			 componentName;

				Bool			 ParseXMLSpec(const String &);

				Bool			 ParseParameters(XML::Node *);
				Bool			 ParseParameterRequirements(Parameter *, XML::Node *);
				Bool			 ParseParameterDependencies(Parameter *, XML::Node *);
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

				String			 external_md5_arguments;
				Bool			 external_md5_stderr;
				String			 external_md5_require;
				String			 external_md5_prefix;

				Array<InputSpec *>	 inputs;

				Array<Parameter *>	 parameters;

				Array<FileFormat *>	 formats;
				Array<TagSpec *>	 tags;

							 ComponentSpecs();
							~ComponentSpecs();

				Bool			 LoadFromDLL(const String &);
				Bool			 LoadFromXML(const String &);

				String			 GetExternalArgumentsString(const Config *);

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
				const void		*(*func_GetFormatInfo)(void *);

				bool			 (*func_SetAudioTrackInfo)(void *, const void *);
				void			 (*func_SetVendorString)(void *, const wchar_t *);

				int			 (*func_ParseBuffer)(void *, const void *, void *);
				int			 (*func_ParseStreamInfo)(void *, const wchar_t *, void *);

				int			 (*func_RenderBuffer)(void *, void *, const void *);
				int			 (*func_RenderStreamInfo)(void *, const wchar_t *, const void *);

				int			 (*func_UpdateStreamInfo)(void *, const wchar_t *, const void *);

				void			 (*func_SetDriver)(void *, void *);

				__int64			 (*func_GetInBytes)(const void *);

				bool			 (*func_Finish)(void *);

				int			 (*func_CanWrite)(void *);

				int			 (*func_SetPause)(void *, bool);
				bool			 (*func_IsPlaying)(void *);

				bool			 (*func_SetOutputFormat)(void *, int);
				char			*(*func_GetOutputFileExtension)(void *);

				int			 (*func_GetNumberOfPasses)(void *);

				bool			 (*func_IsThreadSafe)(void *);

				bool			 (*func_IsLossless)(void *);

				bool			 (*func_Activate)(void *);
				bool			 (*func_Deactivate)(void *);

				bool			 (*func_Seek)(void *, __int64);
				bool			 (*func_NextPass)(void *);

				int			 (*func_ReadData)(void *, void *);
				int			 (*func_WriteData)(void *, void *);
				int			 (*func_TransformData)(void *, void *);
				int			 (*func_ProcessData)(void *, void *);

				int			 (*func_Flush)(void *, void *);

				bool			 (*func_Verify)(void *);

				void			*(*func_GetMainTabLayer)(void *);
				void			*(*func_GetStatusBarLayer)(void *);

				int			 (*func_GetNumberOfDevices)(void *);
				const void		*(*func_GetNthDeviceInfo)(void *, int);

				bool			 (*func_IsNthDeviceTrayOpen)(void *, int);

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
