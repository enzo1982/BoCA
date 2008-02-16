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
	template <class returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_LIST> class SIGNALS_CALLBACK_CLASS_NAME : public Signal
	{
		protected:
			SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>	*slotN;
			SlotRBase0<returnTYPE>										*slot0;
		public:
			SIGNALS_CALLBACK_CLASS_NAME()
			{
				slotN = NIL;
				slot0 = NIL;
			}

			SIGNALS_CALLBACK_CLASS_NAME(const SIGNALS_CALLBACK_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				*this = oSignal;
			}

			virtual ~SIGNALS_CALLBACK_CLASS_NAME()
			{
				DisconnectAll();
			}

			SIGNALS_CALLBACK_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &operator =(const SIGNALS_CALLBACK_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				slotN = oSignal.slotN->Copy();
				slot0 = oSignal.slot0->Copy();

				parent = oSignal.parent;

				return *this;
			}

			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				DisconnectAll();

				slotN = new SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst);

				return Success();
			}

			Int Connect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				DisconnectAll();

				slotN = new SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc);

				return Success();
			}

			Int Connect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				DisconnectAll();

				slotN = new SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig);
		
				return Success();
			}

			Int Connect(const returnTYPE value)
			{
				DisconnectAll();

				slot0 = new SlotRValue0<returnTYPE>(value);

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				DisconnectAll();

				slot0 = new SlotRClass0<oClassTYPE, returnTYPE>(proc, inst);

				return Success();
			}

			Int Connect(returnTYPE (*proc)())
			{
				DisconnectAll();

				slot0 = new SlotRGlobal0<returnTYPE>(proc);

				return Success();
			}

			Int Connect(SignalR0<returnTYPE> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				DisconnectAll();

				slot0 = new SlotRSignal0<returnTYPE>(sig);

				return Success();
			}
#endif

			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				if ((*slotN) == SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst)) DisconnectAll();

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				if ((*slotN) == SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc)) DisconnectAll();

				return Success();
			}

			Int Disconnect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((*slotN) == SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig)) DisconnectAll();

				return Success();
			}

			Int Disconnect(const returnTYPE value)
			{
				if ((*slot0) == SlotRValue0<returnTYPE>(value)) DisconnectAll();

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				if ((*slot0) == SlotRClass0<oClassTYPE, returnTYPE>(proc, inst)) DisconnectAll();

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)())
			{
				if ((*slot0) == SlotRGlobal0<returnTYPE>(proc)) DisconnectAll();

				return Success();
			}

			Int Disconnect(SignalR0<returnTYPE> *sig)
			{
				if ((*slot0) == SlotRSignal0<returnTYPE>(sig)) DisconnectAll();

				return Success();
			}
#endif

			Int DisconnectAll()
			{
				if (slotN != NIL) { delete slotN; slotN = NIL; }
				if (slot0 != NIL) { delete slot0; slot0 = NIL; }

				return Success();
			}

			returnTYPE Call(SIGNALS_ARGUMENT_PARAMETER_LIST)
			{
				returnTYPE	 returnValue = NIL;

				ProtectParent();

				if (slotN != NIL)	returnValue = slotN->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				if (slot0 != NIL)	returnValue = slot0->Emit();

				UnprotectParent();

				return returnValue;
			}

			returnTYPE CallUnprotected(SIGNALS_ARGUMENT_PARAMETER_LIST)
			{
				if (slotN != NIL)	return slotN->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				if (slot0 != NIL)	return slot0->Emit();

				return NIL;
			}

			Int GetNOfConnectedSlots()
			{
				if (slotN != NIL || slot0 != NIL)	return 1;
				else					return 0;
			}
	};
};
