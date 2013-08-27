 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
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

#include "../xml/document.h"

namespace smooth
{
	namespace I18n
	{
		class SMOOTHAPI Translator
		{
			private:
				String				 appPrefix;

				Int				 GetSupportedLanguages();
			protected:
				Array<Language *, Void *>	 languages;

				Language			*activeLanguage;

				Int				 AddLanguage(Language *);

				Int				 LoadDescription(XML::Document *, Language *);
				Int				 LoadData(XML::Document *, Language *);
			public:
				static Translator		*defaultTranslator;

								 Translator(const String &);
				virtual				~Translator();

				Int				 SetInternalLanguageInfo(const String &, const String &, const String &, Bool);
				Int				 SelectUserDefaultLanguage();

				virtual Int			 ActivateLanguage(const String &);
				const String			&TranslateString(const String &, const String & = NIL);

				Bool				 SetContext(const String &);
				const String			&GetContext() const;
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
