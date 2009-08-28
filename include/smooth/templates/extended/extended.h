 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_EXTENDED
#define H_OBJSMOOTH_EXTENDED

#include "../callbacks.h"

namespace smooth
{
	template <class t> class Extended
	{
		private:
			t			 var;

			t			&DefaultRead(t &value)		{ return value; }
			t			&DefaultWrite(t &value)		{ return value; }
		public:
						 Extended()			{ onRead.Connect(&Extended<t>::DefaultRead, this); onWrite.Connect(&Extended<t>::DefaultWrite, this); }
						 Extended(const t &iValue)	{ onRead.Connect(&Extended::DefaultRead, this); onWrite.Connect(&Extended::DefaultWrite, this); t value = iValue; var = onWrite.CallUnprotected(value); }

			t &operator		 =(t &nValue)			{ var = onWrite.CallUnprotected(nValue); return onRead.CallUnprotected(var); }

			t &operator		 ++()				{ var = onWrite.CallUnprotected(++var); return onRead.CallUnprotected(var); }
			t operator		 ++(int)			{ t oValue = onRead.CallUnprotected(var); var = onWrite.CallUnprotected(++var); return oValue; }

			t &operator		 --()				{ var = onWrite.CallUnprotected(--var); return onRead.CallUnprotected(var); }
			t operator		 --(int)			{ t oValue = onRead.CallUnprotected(var); var = onWrite.CallUnprotected(--var); return oValue; }

			Bool operator		 ==(t &cValue)			{ return onRead.CallUnprotected(var) == onWrite.CallUnprotected(cValue); }
			Bool operator		 !=(t &cValue)			{ return onRead.CallUnprotected(var) != onWrite.CallUnprotected(cValue); }

			Bool operator		 <(t &cValue)			{ return onRead.CallUnprotected(var) < onWrite.CallUnprotected(cValue); }
			Bool operator		 >(t &cValue)			{ return onRead.CallUnprotected(var) > onWrite.CallUnprotected(cValue); }
			Bool operator		 <=(t &cValue)			{ return onRead.CallUnprotected(var) <= onWrite.CallUnprotected(cValue); }
			Bool operator		 >=(t &cValue)			{ return onRead.CallUnprotected(var) >= onWrite.CallUnprotected(cValue); }

			t &operator		 +=(t &nValue)			{ var = onWrite.CallUnprotected(var + nValue); return onRead.CallUnprotected(var); }
			t &operator		 -=(t &nValue)			{ var = onWrite.CallUnprotected(var - nValue); return onRead.CallUnprotected(var); }
			t &operator		 *=(t &nValue)			{ var = onWrite.CallUnprotected(var * nValue); return onRead.CallUnprotected(var); }
			t &operator		 /=(t &nValue)			{ var = onWrite.CallUnprotected(var / nValue); return onRead.CallUnprotected(var); }
			t &operator		 %=(t &nValue)			{ var = onWrite.CallUnprotected(var % nValue); return onRead.CallUnprotected(var); }

			t &operator		 &=(t &nValue)			{ var = onWrite.CallUnprotected(var & nValue); return onRead.CallUnprotected(var); }
			t &operator		 ^=(t &nValue)			{ var = onWrite.CallUnprotected(var ^ nValue); return onRead.CallUnprotected(var); }
			t &operator		 |=(t &nValue)			{ var = onWrite.CallUnprotected(var | nValue); return onRead.CallUnprotected(var); }

			t &operator		 <<=(t &nValue)			{ var = onWrite.CallUnprotected(var << nValue); return onRead.CallUnprotected(var); }
			t &operator		 >>=(t &nValue)			{ var = onWrite.CallUnprotected(var >> nValue); return onRead.CallUnprotected(var); }

			operator		 t()				{ return onRead.CallUnprotected(var); }

			t &operator		 *()				{ return onRead.CallUnprotected(var); }
			t *operator		 ->()				{ t &value = onRead.CallUnprotected(var); return &value; }

		callbacks:
			Callback1<t &, t &>	 onRead;
			Callback1<t &, t &>	 onWrite;
	};
};

#endif
