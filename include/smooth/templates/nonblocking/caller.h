 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../../system/system.h"

namespace smooth
{
	template <NONBLOCKING_ARGUMENT_LIST NONBLOCKING_CONDITIONAL_COMMA class dummyTYPE = Void> class NONBLOCKING_CALLER_CLASS_NAME : public Caller
	{
		protected:
			NONBLOCKING_SLOT_BASE_CLASS_NAME<Void NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES>	*slotN;
			Threads::Thread											*thread;

			NONBLOCKING_ARGUMENT_DECLARATIONS
		public:
			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> NONBLOCKING_CALLER_CLASS_NAME(slotReturnTYPE (classTYPE::*proc)(NONBLOCKING_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				slotN	= new NONBLOCKING_SLOT_CLASS_CLASS_NAME<oClassTYPE, slotReturnTYPE NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES>(proc, inst);
				thread	= NIL;
			}

			template <class slotReturnTYPE> NONBLOCKING_CALLER_CLASS_NAME(slotReturnTYPE (*proc)(NONBLOCKING_ARGUMENT_TYPES))
			{
				slotN	= new NONBLOCKING_SLOT_GLOBAL_CLASS_NAME<slotReturnTYPE NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES>(proc);
				thread	= NIL;
			}

			template <class slotReturnTYPE> NONBLOCKING_CALLER_CLASS_NAME(NONBLOCKING_SIGNAL_CLASS_NAME<slotReturnTYPE NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES> *sig)
			{
				slotN	= new NONBLOCKING_SLOT_SIGNAL_CLASS_NAME<slotReturnTYPE NONBLOCKING_CONDITIONAL_COMMA NONBLOCKING_ARGUMENT_TYPES>(sig);
				thread	= NIL;
			}

			virtual ~NONBLOCKING_CALLER_CLASS_NAME()
			{
				if (thread == NIL)
				{
					delete slotN;
				}
				else
				{
					while (IsActive()) System::System::Sleep(10);

					delete slotN;
					delete thread;
				}
			}

			Bool IsActive()
			{
				return (thread->GetStatus() != Threads::THREAD_STOPPED &&
					thread->GetStatus() != Threads::THREAD_STOPPED_SELF);
			}

			Threads::Thread *Call(NONBLOCKING_ARGUMENT_PARAMETER_LIST)
			{
				NONBLOCKING_ARGUMENT_ASSIGNMENTS

				thread = new Threads::Thread();

				thread->threadMain.Connect(&NONBLOCKING_CALLER_CLASS_NAME::Thread, this);

				thread->Start();

				return thread;
			}

			Int Thread(Threads::Thread *thread)
			{
				slotN->Emit(NONBLOCKING_ARGUMENT_PARAMETERS);

				return Success();
			}
	};
};
