 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_COLORDLG
#define H_OBJSMOOTH_COLORDLG

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class ColorSelection;
		};

		class Window;
		class Button;
		class EditBox;
		class Text;
		class Slider;
		class Titlebar;
		class Divider;
	};
};

#include "dialog.h"

namespace smooth
{
	namespace GUI
	{
		namespace Dialogs
		{
			class SMOOTHAPI ColorSelection : public Dialog
			{
				private:
					Color		 color;

					Int		 yoffset;
					Int		 huexoffset;
					Int		 ncxoffset;
					Int		 ocxoffset;
					Int		 crsizex;
					Int		 crsizey;

					Bool		 huecapt;
					Bool		 vscapt;

					Bool		 forcehupdate;
					Bool		 forcevsupdate;
					Bool		 preventhupdate;
					Bool		 preventvsupdate;
					Bool		 updatetext;
					Bool		 updatehextext;

					Int		 lasthue;
					Int		 lastsat;
					Int		 lastval;

					Int		 acthue;
					Int		 actsat;
					Int		 actval;
					Int		 actred;
					Int		 actgreen;
					Int		 actblue;

					Int		 colortable[256][256];

					String		 hexval;

					Window		*dlgwnd;
					Titlebar	*titlebar;
					Divider		*divbar;
					Button		*okbtn;
					Button		*cancelbtn;
					Slider		*hueslider;
					Slider		*satslider;
					Slider		*valslider;
					Slider		*redslider;
					Slider		*greenslider;
					Slider		*blueslider;
					Text		*huetext;
					Text		*sattext;
					Text		*valtext;
					Text		*redtext;
					Text		*greentext;
					Text		*bluetext;
					Text		*hextext;
					EditBox		*hueedit;
					EditBox		*satedit;
					EditBox		*valedit;
					EditBox		*rededit;
					EditBox		*greenedit;
					EditBox		*blueedit;
					EditBox		*hexedit;

					Void		 ColorDlgUpdateHexValue();
					Void		 ColorDlgHexValueChanged();

					Void		 ColorDlgPaintProc();
					Void		 ColorDlgMessageProc(Int, Int, Int);
					Void		 ColorDlgUpdatePickers();
					Bool		 ColorDlgKillProc();
					Void		 ColorDlgOK();
					Void		 ColorDlgCancel();
					Void		 ColorDlgHueSlider();
					Void		 ColorDlgSatSlider();
					Void		 ColorDlgValSlider();
					Void		 ColorDlgRedSlider();
					Void		 ColorDlgGreenSlider();
					Void		 ColorDlgBlueSlider();
					Void		 ColorDlgHueEdit();
					Void		 ColorDlgSatEdit();
					Void		 ColorDlgValEdit();
					Void		 ColorDlgRedEdit();
					Void		 ColorDlgGreenEdit();
					Void		 ColorDlgBlueEdit();
					Void		 ColorDlgHexEdit();
				public:
							 ColorSelection();
					virtual		~ColorSelection();

					const Error	&ShowDialog();
				accessors:
					Int		 SetColor(const Color &);
					const Color	&GetColor() const;
			};
		};
	};
};

#endif
