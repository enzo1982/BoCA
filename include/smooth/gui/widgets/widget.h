 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_WIDGET
#define H_OBJSMOOTH_WIDGET

namespace smooth
{
	namespace GUI
	{
		class Widget;
		class Window;

		class Tooltip;
		class PopupMenu;
		class Layer;

		class Surface;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../../basic/object.h"
#include "../../graphics/font.h"
#include "../../templates/signals.h"
#include "../../templates/callbacks.h"
#include "../../graphics/forms/rect.h"

namespace smooth
{
	const Short	 SP_PAINT	= 0;
	const Short	 SP_UPDATE	= 1;
	const Short	 SP_MOUSEIN	= 2;
	const Short	 SP_MOUSEOUT	= 3;
	const Short	 SP_MOUSEDOWN	= 4;
	const Short	 SP_MOUSEUP	= 5;

	namespace GUI
	{
		class SMOOTHAPI Widget : public Object
		{
			private:
				Array<Widget *, Void *>		 widgets;

				Tooltip				*tooltip;
				System::Timer			*tipTimer;

				Point				 pos;
				Size				 size;

				Bool				 registered;
			protected:
				Bool				 visible;
				Bool				 active;
				Bool				 focussed;

				Bool				 alwaysActive;

				/* Makes a widget independent of its container; i.e. it can be
				 * painted and used even if it's located outside of the container.
				 */
				Bool				 independent;

				Short				 orientation;

				Bool				 tabstopCapable;
				Bool				 dropTarget;

				Bool				 mouseOver;

				Bool				 leftButtonDown;
				Bool				 rightButtonDown;

				Bool				 mouseDragging;

				Bool				 backgroundColorSet;
				Color				 backgroundColor;

				String				 text;
				String				 statusText;

				String				 tooltipText;
				Layer				*tooltipLayer;

				Font				 font;

				Size				 unscaledTextSize;
				Size				 scaledTextSize;

				Widget				*container;

				PopupMenu			*contextMenu;

				Void				 ComputeTextSize();

				Void				 SetFocusByKeyboard();

				Void				 ActivateTooltip();
				Void				 DeactivateTooltip();

				virtual Void			 EnqueueForDeletion();
			public:
				static const Short		 classID;

				Short				 subtype;

								 Widget(const Point &, const Size &);
				virtual				~Widget();

				virtual Int			 EnableLocking(Bool = True);

				Int				 GetNOfObjects() const				{ return widgets.Length(); }
				Widget				*GetNthObject(Int n) const			{ return widgets.GetNth(n); }

				virtual Int			 Add(Widget *);
				virtual Int			 Remove(Widget *);

				Window				*GetContainerWindow() const;
				virtual Surface			*GetDrawSurface() const;

				Void				 SetContainer(Widget *nContainer)		{ container = nContainer; }
				Widget				*GetContainer() const				{ return container; }

				Void				 SetRegisteredFlag(Bool nValue)			{ registered = nValue; }

				Widget				*GetPreviousTabstopWidget(Int) const;
				Widget				*GetNextTabstopWidget(Int) const;

				virtual Point			 GetRealPosition() const;
				virtual Size			 GetRealSize() const;

				virtual Int			 Show();
				virtual Int			 Hide();

				virtual Int			 Activate();
				virtual Int			 Deactivate();

				virtual Int			 Paint(Int);
				virtual Int			 Process(Int, Int, Int);

				virtual String			 ToString() const				{ return "a Widget"; }
			accessors:
				virtual Bool			 IsRegistered() const				{ return registered; }
				virtual Bool			 IsVisible() const				{ if (!registered) return False; if (!visible) return False; return container->IsVisible(); }
				virtual Bool			 IsActive() const				{ if (!registered) return active; if (!active) return False; if (alwaysActive) return True; return container->IsActive(); }

				Void				 SetVisibleDirect(Bool nValue)			{ visible = nValue; }
				Bool				 IsVisibleDirect() const			{ return visible; }

				Void				 SetAlwaysActive(Bool nValue)			{ alwaysActive = nValue; }
				Bool				 IsAlwaysActive() const				{ return alwaysActive; }

				Void				 SetIndependent(Bool nValue)			{ independent = nValue; }
				Bool				 IsIndependent() const				{ return independent; }

				Void				 SetTabstopCapable(Bool nValue)			{ tabstopCapable = nValue; }
				Bool				 IsTabstopCapable() const			{ return tabstopCapable; }

				Bool				 IsDropTarget() const;

				Bool				 IsMouseOver() const				{ return mouseOver; }

				Int				 SetFocus();
				Bool				 IsFocussed() const				{ return focussed; }

				virtual Int			 SetText(const String &);
				virtual const String		&GetText() const				{ return text; }

				virtual Int			 SetTooltipText(const String &);
				virtual const String		&GetTooltipText() const				{ return tooltipText; }

				virtual Int			 SetTooltipLayer(Layer *);
				virtual Layer			*GetTooltipLayer() const			{ return tooltipLayer; }

				virtual Int			 SetStatusText(const String &nStatusText)	{ statusText = nStatusText; return Success(); }
				virtual const String		&GetStatusText() const				{ return statusText; }

				virtual Int			 SetFont(const Font &);
				virtual const Font		&GetFont() const				{ return font; }

				Int				 GetUnscaledTextWidth() const			{ return unscaledTextSize.cx; }
				Int				 GetScaledTextWidth() const			{ return scaledTextSize.cx; }

				Int				 GetUnscaledTextHeight() const			{ return unscaledTextSize.cy; }
				Int				 GetScaledTextHeight() const			{ return scaledTextSize.cy; }

				const Size			&GetUnscaledTextSize() const			{ return unscaledTextSize; }
				const Size			&GetScaledTextSize() const			{ return scaledTextSize; }

				virtual Int			 SetOrientation(Int);
				virtual Int			 GetOrientation() const				{ return orientation; }

				virtual Rect			 GetVisibleArea() const;

				Int				 SetBackgroundColor(const Color &);
				const Color			&GetBackgroundColor() const;

				virtual Bool			 IsBackgroundColorSet() const;

				Int				 SetX(Int nX)					{ return SetMetrics(Point(nX, pos.y), size); }
				Int				 GetX() const					{ return pos.x; }

				Int				 SetY(Int nY)					{ return SetMetrics(Point(pos.x, nY), size); }
				Int				 GetY() const					{ return pos.y; }

				Int				 SetWidth(Int nWidth)				{ return SetMetrics(pos, Size(nWidth, size.cy)); }
				Int				 GetWidth() const				{ return size.cx; }

				Int				 SetHeight(Int nHeight)				{ return SetMetrics(pos, Size(size.cx, nHeight)); }
				Int				 GetHeight() const				{ return size.cy; }

				Int				 SetPosition(const Point &nPos)			{ return SetMetrics(nPos, size); }
				const Point			&GetPosition() const				{ return pos; }

				Int				 SetSize(const Size &nSize)			{ return SetMetrics(pos, nSize); }
				const Size			&GetSize() const				{ return size; }

				virtual Int			 SetMetrics(const Point &, const Size &);

				virtual Bool			 IsAffected(const Rect &) const;
				virtual Bool			 IsRightToLeft() const;
			callbacks:
				Callback1<Bool, const Point &>	 hitTest;
				Callback0<PopupMenu *>		 getContextMenu;
			signals:
				Signal0<Void>			 onShow;
				Signal0<Void>			 onHide;

				Signal0<Void>			 onActivate;
				Signal0<Void>			 onDeactivate;

				Signal1<Void, const Point &>	 onChangePosition;
				Signal1<Void, const Size &>	 onChangeSize;

				Signal0<Void>			 onMouseOver;
				Signal0<Void>			 onMouseOut;

				Signal1<Void, const Point &>	 onLeftButtonDown;
				Signal1<Void, const Point &>	 onLeftButtonUp;
				Signal1<Void, const Point &>	 onLeftButtonClick;
				Signal1<Void, const Point &>	 onLeftButtonDoubleClick;

				Signal1<Void, const Point &>	 onRightButtonDown;
				Signal1<Void, const Point &>	 onRightButtonUp;
				Signal1<Void, const Point &>	 onRightButtonClick;
				Signal1<Void, const Point &>	 onRightButtonDoubleClick;

				Signal1<Void, const Point &>	 onMouseDragStart;
				Signal1<Void, const Point &>	 onMouseDrag;
				Signal1<Void, const Point &>	 onMouseDragEnd;

				Signal1<Void, Int>		 onMouseWheel;

				Signal0<Void>			 onAction;

				Signal0<Void>			 onOpenContextMenu;
				Signal0<Void>			 onCloseContextMenu;

				Signal0<Void>			 onGetFocus;
				Signal0<Void>			 onGetFocusByKeyboard;
				Signal0<Void>			 onLoseFocus;

				Signal0<Void>			 onClickInFocus;

				Signal1<Void, Widget *>		 onRegister;
				Signal1<Void, Widget *>		 onUnregister;

				Signal0<Void>			 onEnqueueForDeletion;
			slots:
				Bool				 DefaultHitTest(const Point &);

				Void				 OpenContextMenu();
				Void				 CloseContextMenu();
		};
	};
};

#endif
