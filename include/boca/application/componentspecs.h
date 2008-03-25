 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
		class BOCA_DLL_EXPORT Format
		{
			private:
				String			 name;
				Array<String>		 extensions;
			public:
				const String		&GetName()				{ return name; }
				Void			 SetName(const String &nName)		{ name = nName; }

				const Array<String>	&GetExtensions()			{ return extensions; }
				Void			 AddExtension(const String &nExt)	{ extensions.Add(nExt); }
		};

		class BOCA_DLL_EXPORT ComponentSpecs
		{
			private:
				DynamicLoader	*library;

				String		 componentName;

				Bool		 ParseXMLSpec(const String &);
			public:
				String		 id;
				Int		 type;

				Int		 mode;

				String		 name;
				String		 version;

				String		 external_command;
				String		 external_arguments;

				String		 external_informat;
				String		 external_outformat;

				Array<Format *>	 formats;

						 ComponentSpecs();
						~ComponentSpecs();

				Bool		 LoadFromDLL(const String &);
				Bool		 LoadFromXML(const String &);

				const char	*(*func_GetComponentSpecs)();

				void		*(*func_Create)();
				bool		 (*func_Delete)(void *);

				void		*(*func_GetConfigurationLayer)(void *);
				void		 (*func_FreeConfigurationLayer)(void *);

				bool		 (*func_GetErrorState)(void *);
				const void	*(*func_GetErrorString)(void *);

				bool		 (*func_CanOpenStream)(void *, const wchar_t *);
				int		 (*func_GetStreamInfo)(void *, const wchar_t *, void *);
				void		 (*func_SetInputFormat)(void *, const void *);

				int		 (*func_GetPackageSize)(void *);

				int		 (*func_SetDriver)(void *, void *);

				__int64		 (*func_GetInBytes)(void *);

				int		 (*func_CanWrite)(void *);

				int		 (*func_SetPause)(void *, bool);
				bool		 (*func_IsPlaying)(void *);

				bool		 (*func_SetAudioTrackInfo)(void *, const void *);

				char		*(*func_GetOutputFileExtension)(void *);

				bool		 (*func_Activate)(void *);
				bool		 (*func_Deactivate)(void *);

				int		 (*func_ReadData)(void *, void *, int);
				int		 (*func_WriteData)(void *, void *, int);
		};
	};

	const Int	 COMPONENT_TYPE_UNKNOWN		= 0;
	const Int	 COMPONENT_TYPE_DECODER		= 1;
	const Int	 COMPONENT_TYPE_ENCODER		= 2;
	const Int	 COMPONENT_TYPE_OUTPUT		= 3;
	const Int	 COMPONENT_TYPE_DSP		= 4;
	const Int	 COMPONENT_TYPE_EXTENSION	= 5;

	const Int	 INTERNAL			= 0;
	const Int	 EXTERNAL_MODE_FILE		= 1;
	const Int	 EXTERNAL_MODE_STDIO		= 2;
};

#endif
