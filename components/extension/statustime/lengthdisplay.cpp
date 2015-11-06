 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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
	if (!IsRegistered()) return Error();
	if (!IsVisible())    return Success();

	switch (message)
	{
		case SP_PAINT:
			{
				Surface	*surface = container->GetDrawSurface();
				Rect	 frame	 = Rect(GetRealPosition(), GetRealSize());

				surface->Box(frame, GetBackgroundColor(), Rect::Filled);
				surface->Frame(frame, FRAME_DOWN);

				if (bitmap != NIL)
				{
					surface->BlitFromBitmap(bitmap, Rect(Point(0, 0), bitmap.GetSize()), Rect(frame.GetPosition() + Point(2, 2) * surface->GetSurfaceDPI() / 96.0, bitmap.GetSize() * surface->GetSurfaceDPI() / 96.0));
				}

				surface->SetText(text, frame + Point(5 + bitmap.GetSize().cx, 2) * surface->GetSurfaceDPI() / 96.0 - Point(2, 2), font);
			}

			break;
	}

	return Success();
}

S::Int BoCA::LengthDisplay::SetText(const String &newText)
{
	Widget::SetText(newText);

	SetWidth(unscaledTextSize.cx + bitmap.GetSize().cx + 6);

	return Success();
}
