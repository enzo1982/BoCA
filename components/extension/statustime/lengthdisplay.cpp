 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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
	bitmap = iBitmap;
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

				/* Draw bitmap.
				 */
				Size	 bitmapSize   = bitmap.GetSize();
				Size	 scaledSize   = bitmapSize / 2 * surface->GetSurfaceDPI() / 96.0;
				Int	 bitmapOffset = bitmap != NIL ? (frame.GetHeight() - scaledSize.cy) / 2.0 : 2;

				if (bitmap != NIL)
				{
					if (bitmapSize != scaledSize && bitmapScaled.GetSize() != scaledSize) bitmapScaled = bitmap.Scale(scaledSize);

					if (bitmapSize == scaledSize) surface->BlitFromBitmap(bitmap,	    Rect(Point(0, 0), scaledSize), Rect(frame.GetPosition() + Point(bitmapOffset, bitmapOffset), scaledSize));
					else			      surface->BlitFromBitmap(bitmapScaled, Rect(Point(0, 0), scaledSize), Rect(frame.GetPosition() + Point(bitmapOffset, bitmapOffset), scaledSize));
				}

				/* Draw text.
				 */
				Int	 textOffset   = Math::Round((frame.GetHeight() - scaledTextSize.cy) / 2.0);

				surface->SetText(text, frame + Point(scaledSize.cx + 2 * bitmapOffset - 1, textOffset - 1), font);
			}

			break;
	}

	return Success();
}

S::Int BoCA::LengthDisplay::SetText(const String &newText)
{
	Widget::SetText(newText);

	SetWidth(unscaledTextSize.cx + bitmap.GetSize().cx / 2 + 6);

	return Success();
}
