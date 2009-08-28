 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_WINDOWBACKEND
#define H_OBJSMOOTH_WINDOWBACKEND

namespace smooth
{
	namespace GUI
	{
		class WindowBackend;
	};
};

#include "../../../misc/string.h"
#include "../../../graphics/forms/point.h"
#include "../../../graphics/forms/size.h"
#include "../../../templates/signals.h"
#include "../../../templates/callbacks.h"
#include "../../../graphics/surface.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WINDOW_NONE = 0;

		class WindowBackend
		{
			private:
				static WindowBackend		*(*backend_creator)();
			protected:
				Int				 type;

				Surface				*nullSurface;
				Surface				*drawSurface;

				Rect				 updateRect;
			public:
				static Int			 SetBackend(WindowBackend *(*)());

				static WindowBackend		*CreateBackendInstance();

								 WindowBackend(Void * = NIL);
				virtual				~WindowBackend();

				Int				 GetWindowType();

				virtual Void			*GetSystemWindow() const;

				virtual Surface			*GetDrawSurface() const;

				virtual Int			 Open(const String &, const Point &, const Size &, Int);
				virtual Int			 Close();

				virtual Int			 RequestClose();

				virtual Int			 SetTitle(const String &);
				virtual Int			 SetIcon(const Bitmap &);

				virtual Int			 SetMinimumSize(const Size &);
				virtual Int			 SetMaximumSize(const Size &);

				virtual Int			 Show();
				virtual Int			 Hide();

				virtual Rect			 GetRestoredWindowRect() const;

				virtual Int			 SetMetrics(const Point &, const Size &);

				virtual Int			 Minimize();

				virtual Int			 Maximize();
				virtual Int			 Restore();
			accessors:
				const Rect			&GetUpdateRect();
			signals:
				Callback0<Bool>			 doClose;

				Callback3<Int, Int, Int, Int>	 onEvent;

				Signal0<Void>			 onMinimize;
				Signal0<Void>			 onMaximize;

				Signal0<Void>			 onRestore;

				Signal0<Void>			 onCreate;
				Signal0<Void>			 onDestroy;
		};
	};
};

#endif
