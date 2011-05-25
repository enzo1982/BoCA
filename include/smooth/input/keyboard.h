 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_KEYBOARD
#define H_OBJSMOOTH_KEYBOARD

namespace smooth
{
	namespace GUI
	{
		class Window;
	};

	namespace Input
	{
		class Keyboard;
	};
};

#include "../definitions.h"
#include "../graphics/forms/point.h"

namespace smooth
{
	namespace Input
	{
		class SMOOTHAPI Keyboard
		{
			public:
				/* Key constants.
				 */
				enum Key
				{
					KeyOther	= 0,

					KeyBack		= 0x08,
					KeyTab,

					KeyReturn	= 0x0D,

					KeyShift	= 0x10,
					KeyControl,
					KeyAlt,

					KeyEscape	= 0x1B,

					KeySpace	= 0x20,

					KeyPrior,
					KeyNext,
					KeyEnd,
					KeyHome,

					KeyLeft,
					KeyUp,
					KeyRight,
					KeyDown,

					KeyInsert	= 0x2D,
					KeyDelete,

					Key0		= 0x30,
					Key1,
					Key2,
					Key3,
					Key4,
					Key5,
					Key6,
					Key7,
					Key8,
					Key9,

					KeyA		= 0x41,
					KeyB,
					KeyC,
					KeyD,
					KeyE,
					KeyF,
					KeyG,
					KeyH,
					KeyI,
					KeyJ,
					KeyK,
					KeyL,
					KeyM,
					KeyN,
					KeyO,
					KeyP,
					KeyQ,
					KeyR,
					KeyS,
					KeyT,
					KeyU,
					KeyV,
					KeyW,
					KeyX,
					KeyY,
					KeyZ,

					KeyF1		= 0x70,
					KeyF2,
					KeyF3,
					KeyF4,
					KeyF5,
					KeyF6,
					KeyF7,
					KeyF8,
					KeyF9,
					KeyF10,
					KeyF11,
					KeyF12,
					KeyF13,
					KeyF14,
					KeyF15,
					KeyF16,
					KeyF17,
					KeyF18,
					KeyF19,
					KeyF20,
					KeyF21,
					KeyF22,
					KeyF23,
					KeyF24
				};

			private:
				static Array<Bool>	*keyState;

							 Keyboard();
							 Keyboard(const Keyboard &);
			public:
				/* Internal init and free functions.
				 */
				static Int		 InitKeyState();
				static Int		 FreeKeyState();

				/* Key status functions.
				 */
				static Bool		 GetKeyState(Key);

				static Void		 UpdateKeyState(Key, Bool);
				static Void		 ResetKeyState();
		};
	};
};

#endif
