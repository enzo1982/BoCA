 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SETUP
#define H_OBJSMOOTH_SETUP

namespace smooth
{
	class Setup;
};

#include "../definitions.h"
#include "../graphics/color.h"

namespace smooth
{
	class SMOOTHAPI Setup
	{
		private:
						 Setup();
						 Setup(const Setup &);
		public:
						 Setup(const Setup *);

			static GUI::Color	 BackgroundColor;
			static GUI::Color	 ClientColor;
			static GUI::Color	 ClientTextColor;
			static GUI::Color	 LightGrayColor;
			static GUI::Color	 DividerLightColor;
			static GUI::Color	 DividerDarkColor;
			static GUI::Color	 TextColor;
			static GUI::Color	 InactiveTextColor;
			static GUI::Color	 HighlightColor;
			static GUI::Color	 HighlightTextColor;
			static GUI::Color	 GradientStartColor;
			static GUI::Color	 GradientEndColor;
			static GUI::Color	 GradientTextColor;
			static GUI::Color	 InactiveGradientStartColor;
			static GUI::Color	 InactiveGradientEndColor;
			static GUI::Color	 InactiveGradientTextColor;
			static GUI::Color	 TooltipColor;
			static GUI::Color	 TooltipTextColor;

			static Bool		 rightToLeft;
			static Bool		 useIconv;

			static Float		 FontSize;

			static Int		 HoverTime;
			static Int		 HoverWidth;
			static Int		 HoverHeight;
	};
};

#endif
