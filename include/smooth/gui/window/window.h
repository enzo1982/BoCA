 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_WINDOW
#define H_OBJSMOOTH_WINDOW

namespace smooth
{
	namespace GUI
	{
		class Window;
		class WindowBackend;
		class Layer;
	};

#ifdef __APPLE__
	namespace System
	{
		class EventCocoa;
	};
#endif
};

#include "../widgets/widget.h"
#include "../../init.h"
#include "../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 WF_NORMAL		= 0;
		const Short	 WF_MODAL		= 1;
		const Short	 WF_TOPMOST		= 2;
		const Short	 WF_NORESIZE		= 4;
		const Short	 WF_NOTASKBUTTON	= 8;
		const Short	 WF_THINBORDER		= 16;

		const Short	 WF_NOTITLE		= 32768;

		const Short	 WO_SEPARATOR		= 1;
		const Short	 WO_NOSEPARATOR		= 2;

		const Short	 LD_DEFAULT		= 0;
		const Short	 LD_LEFTTORIGHT		= 1;
		const Short	 LD_RIGHTTOLEFT		= 2;

		class SMOOTHAPI Window : public Widget
		{
#ifdef __APPLE__
			friend class System::EventCocoa;
#endif
			enums:
				enum ProgressIndicatorState
				{
					ProgressIndicatorNone	= 0,
					ProgressIndicatorNormal,
					ProgressIndicatorPaused
				};

			private:
				Int				 order;
			protected:
				static Array<Window *, Void *>	 windows;

				WindowBackend			*backend;

				Bool				 maximized;
				Bool				 minimized;

				Bool				 created;
				volatile Bool			 destroyed;

				Bool				 visibilitySet;

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
				static const Short		 classID;

				static Short			 nOfActiveWindows;

								 Window(const String &, const Point &, const Size &, Void * = NIL);
				virtual				~Window();

				Int				 GetOrder() const			{ return order; }

				Int				 SetIcon(const Bitmap &);
				const Bitmap			&GetIcon() const			{ return icon; }

				Int				 SetIconDirect(Void *);

				virtual Rect			 GetVisibleArea() const;

				virtual Int			 SetMetrics(const Point &, const Size &);

				Int				 SetText(const String &);

				Layer				*GetMainLayer() const			{ return mainLayer; }

				Int				 SetStatusText(const String &);
				const String			&GetStatusText() const;

				Int				 SetDefaultStatusText(const String &);
				Int				 RestoreDefaultStatusText();

				Int				 SetProgressIndicator(ProgressIndicatorState, Float = -1.0);

				const Array<String>		&GetDroppedFiles() const;

				Int				 SetRightToLeft(Bool);
				virtual Bool			 IsRightToLeft() const;

				virtual Point			 GetRealPosition() const		{ return Point(0, 0); }

				Rect				 GetWindowRect() const;
				Rect				 GetClientRect() const;
				Rect				 GetRestoredWindowRect() const;

				Int				 GetFrameWidth() const			{ return frameWidth; }

				Void				 SetUpdateRect(const Rect &nUpdateRect)	{ updateRect = nUpdateRect; }
				const Rect			&GetUpdateRect() const			{ return updateRect; }

				Int				 SetMinimumSize(const Size &);
				Int				 SetMaximumSize(const Size &);

				virtual Int			 Show();
				virtual Int			 Hide();

				Int				 Maximize();
				Int				 Minimize();

				Int				 Restore();

				Int				 Raise();

				Int				 WaitUntilClosed();
				Int				 Close();

				Bool				 IsInUse() const			{ return (created && !destroyed); }

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
			accessors:
				/* Gets the offset by which the window
				 * differs from the specified size.
				 */
				const Size			&GetSizeModifier() const;

				Bool				 IsMaximized() const			{ return maximized; }
				Bool				 IsMinimized() const			{ return minimized; }

				Bool				 IsVisible() const			{ if (!IsRegistered()) return False; if (IsMinimized()) return False; return visible; }

				Bool				 IsVisibilitySet() const		{ return visibilitySet; }
				Bool				 IsBackgroundColorSet() const		{ return backgroundColorSet; }
			slots:
				virtual Void			 OnCreate();
				virtual Void			 OnDestroy();

				virtual Void			 OnMinimize();
				virtual Void			 OnMaximize();

				virtual Void			 OnRestore();
			signals:
				Signal0<Void>			 onCreate;

				Signal0<Void>			 onPaint;
				Signal3<Void, Int, Int, Int>	 onEvent;
			callbacks:
				Callback0<Bool>			 doClose;
		};
	};
};

#endif
