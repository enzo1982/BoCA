 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_WINDOW_
#define _H_OBJSMOOTH_WINDOW_

namespace smooth
{
	namespace GUI
	{
		class Window;
		class WindowBackend;
		class Layer;
	};
};

#include "../widgets/widget.h"
#include "../../init.h"
#include "../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WF_MODAL		= 1;
		const Int	 WF_SYSTEMMODAL		= 2;
		const Int	 WF_TOPMOST		= 4;
		const Int	 WF_APPTOPMOST		= 8;
		const Int	 WF_NORESIZE		= 16;
		const Int	 WF_NOTASKBUTTON	= 32;
		const Int	 WF_THINBORDER		= 64;

		const Int	 WO_SEPARATOR		= 1;
		const Int	 WO_NOSEPARATOR		= 2;

		const Int	 LD_DEFAULT		= 0;
		const Int	 LD_LEFTTORIGHT		= 1;
		const Int	 LD_RIGHTTOLEFT		= 2;

		class SMOOTHAPI Window : public Widget
		{
			private:
				Int				 order;
			protected:
				static Array<Window *, Void *>	 windows;

				WindowBackend			*backend;

				Int				 stay;
				Bool				 maximized;
				Bool				 minimized;

				Bool				 created;
				Bool				 destroyed;

				Bitmap				 icon;

				Rect				 innerOffset;

				Int				 frameWidth;
				Rect				 updateRect;

				String				 defaultStatus;

				Int				 layoutDirection;

				Layer				*mainLayer;

				virtual Bool			 Create();

				Void				 CalculateOffsets();
			public:
				static const Int		 classID;

				static Int			 nOfActiveWindows;

				Bool				 initshow;

								 Window(const String &, const Point &, const Size &, Void * = NIL);
				virtual				~Window();

				Int				 GetOrder() const			 { return order; }

				Int				 SetIcon(const Bitmap &);
				const Bitmap			&GetIcon() const;

				virtual Rect			 GetVisibleArea() const			 { return Rect(Point(), GetSize()); }

				virtual Int			 SetMetrics(const Point &, const Size &);

				Int				 SetText(const String &);

				Layer				*GetMainLayer() const;

				Int				 SetStatusText(const String &);
				const String			&GetStatusText() const;

				Int				 SetDefaultStatusText(const String &);
				Int				 RestoreDefaultStatusText();

				Int				 SetRightToLeft(Bool);
				virtual Bool			 IsRightToLeft() const;

				virtual Point			 GetRealPosition() const;

				Rect				 GetWindowRect() const;
				Rect				 GetClientRect() const;
				Rect				 GetRestoredWindowRect() const;

				Int				 GetFrameWidth()			{ return frameWidth; }

				const Rect			&GetUpdateRect() const;
				Int				 SetUpdateRect(const Rect &);

				Int				 SetMinimumSize(const Size &);
				Int				 SetMaximumSize(const Size &);

				virtual Int			 Show();
				virtual Int			 Hide();

				Int				 Maximize();
				Int				 Minimize();

				Int				 Restore();

				Bool				 IsMaximized() const			{ return maximized; }
				Bool				 IsMinimized() const			{ return minimized; }

				Bool				 IsVisible() const			{ if (IsMinimized()) return False; return Widget::IsVisible(); }

				Int				 Stay();
				Int				 Close();

				Bool				 IsInUse() const;

				virtual Int			 Paint(Int);
				virtual Int			 Process(Int, Int, Int);

				Point				 GetMousePosition() const;

				Bool				 IsMouseOn(const Rect &) const;

				virtual Surface			*GetDrawSurface() const;
				Void				*GetSystemWindow() const;

				static Int			 GetNOfWindows()			{ return windows.Length(); }
				static Window			*GetNthWindow(Int n)			{ return windows.GetNth(n); }

				static Window			*GetWindow(Void *);

				virtual Int			 Add(Widget *);
				virtual Int			 Remove(Widget *);
			slots:
				virtual Void			 OnCreate();
				virtual Void			 OnDestroy();

				virtual Void			 OnMinimize();
				virtual Void			 OnMaximize();

				virtual Void			 OnRestore();
			signals:
				Signal0<Void>			 onCreate;

				Signal0<Void>			 onPaint;
				Signal0<Void>			 onPeek;
				Signal3<Void, Int, Int, Int>	 onEvent;
			callbacks:
				Callback0<Bool>			 doClose;
		};
	};
};

#endif
