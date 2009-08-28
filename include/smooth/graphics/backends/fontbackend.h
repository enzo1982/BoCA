 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONTBACKEND
#define H_OBJSMOOTH_FONTBACKEND

namespace smooth
{
	namespace GUI
	{
		class FontBackend;
	};
};

#include "../../definitions.h"
#include "../forms/size.h"
#include "../color.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 FONT_NONE = 0;

		class FontBackend
		{
			private:
				static FontBackend	*(*backend_creator)(const String &, Int, Int, Int, const Color &);
			protected:
				Int			 type;

				String			 fontName;
				Int			 fontSize;
				Color			 fontColor;
				Int			 fontWeight;
				Int			 fontStyle;
			public:
				static Int		 SetBackend(FontBackend *(*)(const String &, Int, Int, Int, const Color &));

				static FontBackend	*CreateBackendInstance(const String &, Int, Int, Int, const Color &);

							 FontBackend(const String &, Int, Int, Int, const Color &);
				virtual			~FontBackend();

				Int			 GetFontType() const;

				Int			 SetName(const String &newFontName)	{ fontName = newFontName; return Success(); }
				Int			 SetSize(Int newFontSize)		{ fontSize = newFontSize; return Success(); }
				Int			 SetColor(const Color &newFontColor)	{ fontColor = newFontColor; return Success(); }
				Int			 SetWeight(Int newFontWeight)		{ fontWeight = newFontWeight; return Success(); }
				Int			 SetStyle(Int newFontStyle)		{ fontStyle = newFontStyle; return Success(); }

				Int			 GetTextSizeX(const String &) const;
				Int			 GetTextSizeY(const String &) const;

				virtual Size		 GetTextSize(const String &) const;
		};
	};
};

#endif
