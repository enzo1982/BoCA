 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
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
	template <class returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_LIST> class SIGNALS_SIGNAL_CLASS_NAME : public Signal
	{
		protected:
			Array<Void *>	*slotsN;
			Array<Void *>	*slots0;

			Void RemoveNthN(Int n)
			{
				if (slotsN == NIL)	   return;
				if (slotsN->Length() <= n) return;

				delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN->GetNth(n);

				slotsN->RemoveNth(n);

				if (slotsN->Length() == 0)
				{
					delete slotsN;

					slotsN = NIL;
				}
			}

			Void RemoveNth0(Int n)
			{
				if (slots0 == NIL)	   return;
				if (slots0->Length() <= n) return;

				delete (SlotRBase0<returnTYPE> *) slots0->GetNth(n);

				slots0->RemoveNth(n);

				if (slots0->Length() == 0)
				{
					delete slots0;

					slots0 = NIL;
				}
			}
		public:
			SIGNALS_SIGNAL_CLASS_NAME()
			{
				slotsN = NIL;
				slots0 = NIL;
			}

			SIGNALS_SIGNAL_CLASS_NAME(const SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				slotsN = NIL;
				slots0 = NIL;

				*this = oSignal;
			}

			virtual ~SIGNALS_SIGNAL_CLASS_NAME()
			{
				DisconnectAll();
			}

			SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &operator =(const SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				if (&oSignal == this) return *this;

				DisconnectAll();

				if (oSignal.slotsN != NIL)
				{
					slotsN = new Array<Void *>();

					for (Int i = 0; i < oSignal.slotsN->Length(); i++)
					{
						slotsN->Add(((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) oSignal.slotsN->GetNth(i))->Copy());
					}
				}

				if (oSignal.slots0 != NIL)
				{
					slots0 = new Array<Void *>();

					for (Int j = 0; j < oSignal.slots0->Length(); j++)
					{
						slots0->Add(((SlotRBase0<returnTYPE> *) oSignal.slots0->GetNth(j))->Copy());
					}
				}

				parent = oSignal.parent;

				return *this;
			}

			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				if (slotsN == NIL) slotsN = new Array<Void *>();

				slotsN->Add(new SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst));

				return Success();
			}

			Int Connect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				if (slotsN == NIL) slotsN = new Array<Void *>();

				slotsN->Add(new SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc));

				return Success();
			}

			Int Connect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				if (slotsN == NIL) slotsN = new Array<Void *>();

				slotsN->Add(new SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig));

				return Success();
			}

			Int Connect(const returnTYPE value)
			{
				if (slots0 == NIL) slots0 = new Array<Void *>();

				slots0->Add(new SlotRValue0<returnTYPE>(value));

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				if (slots0 == NIL) slots0 = new Array<Void *>();

				slots0->Add(new SlotRClass0<oClassTYPE, returnTYPE>(proc, inst));

				return Success();
			}

			Int Connect(returnTYPE (*proc)())
			{
				if (slots0 == NIL) slots0 = new Array<Void *>();

				slots0->Add(new SlotRGlobal0<returnTYPE>(proc));

				return Success();
			}

			Int Connect(SignalR0<returnTYPE> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				if (slots0 == NIL) slots0 = new Array<Void *>();

				slots0->Add(new SlotRSignal0<returnTYPE>(sig));

				return Success();
			}
#endif

			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				if (slotsN == NIL) return Error();

				for (Int i = slotsN->Length() - 1; i >= 0; i--)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN->GetNth(i))) == SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst))
					{
						RemoveNthN(i);

						break;
					}
				}

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				if (slotsN == NIL) return Error();

				for (Int i = slotsN->Length() - 1; i >= 0; i--)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN->GetNth(i))) == SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc))
					{
						RemoveNthN(i);

						break;
					}
				}

				return Success();
			}

			Int Disconnect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if (slotsN == NIL) return Error();

				for (Int i = slotsN->Length() - 1; i >= 0; i--)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN->GetNth(i))) == SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig))
					{
						RemoveNthN(i);

						break;
					}
				}

				return Success();
			}

			Int Disconnect(const returnTYPE value)
			{
				if (slots0 == NIL) return Error();

				for (Int i = slots0->Length() - 1; i >= 0; i--)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slots0->GetNth(i))) == SlotRValue0<returnTYPE>(value))
					{
						RemoveNth0(i);

						break;
					}
				}

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				if (slots0 == NIL) return Error();

				for (Int i = slots0->Length() - 1; i >= 0; i--)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0->GetNth(i))) == SlotRClass0<oClassTYPE, returnTYPE>(proc, inst))
					{
						RemoveNth0(i);

						break;
					}
				}

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)())
			{
				if (slots0 == NIL) return Error();

				for (Int i = slots0->Length() - 1; i >= 0; i--)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0->GetNth(i))) == SlotRGlobal0<returnTYPE>(proc))
					{
						RemoveNth0(i);

						break;
					}
				}

				return Success();
			}

			Int Disconnect(SignalR0<returnTYPE> *sig)
			{
				if (slots0 == NIL) return Error();

				for (Int i = slots0->Length() - 1; i >= 0; i--)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0->GetNth(i))) == SlotRSignal0<returnTYPE>(sig))
					{
						RemoveNth0(i);

						break;
					}
				}

				return Success();
			}
#endif

			Int DisconnectAll()
			{
				while (slotsN != NIL) RemoveNthN(slotsN->Length() - 1);
				while (slots0 != NIL) RemoveNth0(slots0->Length() - 1);

				return Success();
			}

			returnTYPE Emit(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				returnTYPE	 returnValue = (returnTYPE) NIL;

				if (slotsN == NIL && slots0 == NIL) return returnValue;

				ProtectParent();

				returnValue = EmitUnprotected(SIGNALS_ARGUMENT_PARAMETERS);

				UnprotectParent();

				return returnValue;
			}

			returnTYPE EmitUnprotected(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				returnTYPE	 returnValue = (returnTYPE) NIL;

				for (Int i = 0; slotsN != NIL && i < slotsN->Length(); i++) returnValue = ((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN->GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				for (Int j = 0; slots0 != NIL && j < slots0->Length(); j++) returnValue = ((SlotRBase0<returnTYPE> *) slots0->GetNth(j))->Emit();

				return returnValue;
			}

			Int GetNOfConnectedSlots() const
			{
				if (slotsN == NIL && slots0 == NIL) return 0;

				if (slotsN == NIL) return slots0->Length();
				if (slots0 == NIL) return slotsN->Length();

				return slotsN->Length() + slots0->Length();
			}
	};
};
