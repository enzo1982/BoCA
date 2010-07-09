 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "lengthdisplay.h"

BoCA::LengthDisplay::LengthDisplay(const Bitmap &iBitmap) : Widget(Point(0, 0), Size(42, 14))
{
	bitmap	= iBitmap;
}

BoCA::LengthDisplay::~LengthDisplay()
{
}

Int BoCA::LengthDisplay::Paint(Int message)
{
	if (!IsRegistered())	return Error();
	if (!IsVisible())	return Success();

	switch (message)
	{
		case SP_SHOW:
		case SP_PAINT:
			{
				Surface	*surface = container->GetDrawSurface();

				surface->Frame(Rect(GetRealPosition(), GetSize()), FRAME_DOWN);

				if (bitmap != NIL)
				{
					surface->BlitFromBitmap(bitmap, Rect(Point(0, 0), bitmap.GetSize()), Rect(GetRealPosition() + Point(2, 2), bitmap.GetSize()));
				}

				surface->SetText(text, Rect(GetRealPosition() + Point(3 + bitmap.GetSize().cx, 0), GetSize()), font);
			}

			break;
	}

	return Success();
}

S::Int BoCA::LengthDisplay::SetText(const String &newText)
{
	Widget::SetText(newText);

	SetWidth(textSize.cx + bitmap.GetSize().cx + 6);

	return Success();
}
