 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONT
#define H_OBJSMOOTH_FONT

namespace smooth
{
	namespace GUI
	{
		class Font;
		class FontBackend;
	};
};

#include "../definitions.h"
#include "color.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Font
		{
			private:
				FontBackend	*backend;
			protected:
				Short		 fontSize;
				String		 fontName;
				Color		 fontColor;
				Short		 fontWeight;
				Short		 fontStyle;
			constants:
				static String	 Default;
				static Short	 DefaultSize;

				static Short	 Thin;
				static Short	 ExtraLight;
				static Short	 Light;
				static Short	 Normal;
				static Short	 Medium;
				static Short	 SemiBold;
				static Short	 Bold;
				static Short	 ExtraBold;
				static Short	 Black;

				static Short	 Italic;
				static Short	 Underline;
				static Short	 StrikeOut;
			public:
						 Font(const String & = Font::Default, Short = Font::DefaultSize, Short = Font::Normal, Short = Font::Normal, const Color & = Color());
						 Font(const Font &);
						~Font();

				Font &operator	 =(const Font &);
				Bool operator	 ==(const Font &) const;
				Bool operator	 !=(const Font &) const;

				Int		 SetName(const String &);
				Int		 SetSize(Short);
				Int		 SetColor(const Color &);
				Int		 SetWeight(Short);
				Int		 SetStyle(Short);

				const String	&GetName() const;
				Short		 GetSize() const;
				const Color	&GetColor() const;
				Short		 GetWeight() const;
				Short		 GetStyle() const;

				Int		 GetUnscaledTextSizeX(const String &) const;
				Int		 GetUnscaledTextSizeY(const String & = "*") const;

				Int		 GetScaledTextSizeX(const String &) const;
				Int		 GetScaledTextSizeY(const String & = "*") const;
		};
	};
};

#endif
