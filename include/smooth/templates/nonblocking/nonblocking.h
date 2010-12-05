 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../../threads/thread.h"
#include "caller.h"

namespace smooth
{
	template <NONBLOCKING_ARGUMENT_LIST NONBLOCKING_CONDITIONAL_COMMA class dummyTYPE = Void> class NONBLOCKING_NONBLOCKING_CLASS_NAME : public NonBlocking
	{
		protected:
			NONBLOCKING_CALLER_CLASS_NAME<NONBLOCKING_ARGUMENT_TYPES NONBLOCKING_CONDITIONAL_COMMA dummyTYPE>	*caller;
		public:
			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> NONBLOCKING_NONBLOCKING_CLASS_NAME(slotReturnTYPE (classTYPE::*proc)(NONBLOCKING_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				caller = new NONBLOCKING_CALLER_CLASS_NAME<NONBLOCKING_ARGUMENT_TYPES NONBLOCKING_CONDITIONAL_COMMA dummyTYPE>(proc, inst);

				callers.Add(caller);
			}

			template <class slotReturnTYPE> NONBLOCKING_NONBLOCKING_CLASS_NAME(slotReturnTYPE (*proc)(NONBLOCKING_ARGUMENT_TYPES))
			{
				caller = new NONBLOCKING_CALLER_CLASS_NAME<NONBLOCKING_ARGUMENT_TYPES NONBLOCKING_CONDITIONAL_COMMA dummyTYPE>(proc);

				callers.Add(caller);
			}

			template <class slotReturnTYPE> NONBLOCKING_NONBLOCKING_CLASS_NAME(NONBLOCKING_SIGNAL_CLASS_NAME<slotReturnTYPE NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES> *sig)
			{
				caller = new NONBLOCKING_CALLER_CLASS_NAME<NONBLOCKING_ARGUMENT_TYPES NONBLOCKING_CONDITIONAL_COMMA dummyTYPE>(sig);

				callers.Add(caller);
			}

			virtual ~NONBLOCKING_NONBLOCKING_CLASS_NAME()
			{
			}

			Threads::Thread *Call(NONBLOCKING_ARGUMENT_PARAMETER_LIST)
			{
				return caller->Call(NONBLOCKING_ARGUMENT_PARAMETERS);
			}
	};
};
