 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

namespace smooth
{
	template <class returnTYPE> class SlotRValue0 : public SlotRBase0<returnTYPE>
	{
		private:
			returnTYPE		 value;
		public:
						 SlotRValue0(const returnTYPE &iValue)	{ value = iValue; }
			SlotRBase0<returnTYPE>	*Copy() const				{ return new SlotRValue0<returnTYPE>(value); }

			returnTYPE Emit() const
			{
				return value;
			}

			Bool operator ==(const SlotRBase0<returnTYPE> &cInstance) const
			{
				if (value == ((SlotRValue0<returnTYPE> &) cInstance).value)	return True;
				else								return False;
			}
	};
};
