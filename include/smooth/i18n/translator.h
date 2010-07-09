 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_I18N_TRANSLATOR
#define H_OBJSMOOTH_I18N_TRANSLATOR

namespace smooth
{
	namespace I18n
	{
		class Language;
	};
};

#include "string.h"
#include "../xml/document.h"

namespace smooth
{
	namespace I18n
	{
		class SMOOTHAPI Translator
		{
			private:
				String				 appPrefix;

				Language			*activeLanguage;

				Int				 GetSupportedLanguages();
			protected:
				Array<Language *, Void *>	 languages;

				Int				 LoadDoc(XML::Document *, Language *);
			public:
								 Translator(const String &);
				virtual				~Translator();

				Int				 SetInternalLanguageInfo(const String &, const String &, const String &, Bool);
				Int				 SelectUserDefaultLanguage();
				
				Int				 ActivateLanguage(const String &);
				const String			&TranslateString(const String &);

				static Translator		*defaultTranslator;
			accessors:
				Int				 GetNOfLanguages() const;

				const String			&GetNthLanguageName(Int) const;
				const String			&GetNthLanguageID(Int) const;
				const String			&GetNthLanguageAuthor(Int) const;
				const String			&GetNthLanguageEncoding(Int) const;
				const String			&GetNthLanguageURL(Int) const;
				Bool				 IsNthLanguageRightToLeft(Int) const;

				const String			&GetActiveLanguageName() const;
				const String			&GetActiveLanguageID() const;
				const String			&GetActiveLanguageAuthor() const;
				const String			&GetActiveLanguageEncoding() const;
				const String			&GetActiveLanguageURL() const;
				Bool				 IsActiveLanguageRightToLeft() const;
		};
	};
};

#endif
