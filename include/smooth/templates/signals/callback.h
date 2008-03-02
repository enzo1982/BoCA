 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "signal_base.h"

namespace smooth
{
	template <class returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_LIST> class SIGNALS_CALLBACK_CLASS_NAME : public SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>
	{
		protected:
			/* FIXME: This is never used, but somehow smooth
			 *	  does not work on Linux without it.
			 */
			SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>	*dummy;
		public:
			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(proc, inst);
			}

			Int Connect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(proc);
			}

			Int Connect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(sig);
			}

			Int Connect(const returnTYPE value)
			{
				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(value);
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(proc, inst);
			}

			Int Connect(returnTYPE (*proc)())
			{
				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(proc);
			}

			Int Connect(SignalR0<returnTYPE> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::DisconnectAll();

				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Connect(sig);
			}
#endif

			returnTYPE Call(SIGNALS_ARGUMENT_PARAMETER_LIST)
			{
				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::Emit(SIGNALS_ARGUMENT_PARAMETERS);
			}

			returnTYPE CallUnprotected(SIGNALS_ARGUMENT_PARAMETER_LIST)
			{
				return SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>::EmitUnprotected(SIGNALS_ARGUMENT_PARAMETERS);
			}
	};
};
