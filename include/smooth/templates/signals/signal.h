 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
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
			Array<Void *>	 slotsN;
			Array<Void *>	 slots0;
		public:
			SIGNALS_SIGNAL_CLASS_NAME()
			{
			}

			SIGNALS_SIGNAL_CLASS_NAME(const SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				*this = oSignal;
			}

			virtual ~SIGNALS_SIGNAL_CLASS_NAME()
			{
				DisconnectAll();
			}

			SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &operator =(const SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &oSignal)
			{
				for (Int i = 0; i < oSignal.slotsN.Length(); i++)
				{
					slotsN.Add(((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) oSignal.slotsN.GetNth(i))->Copy());
				}

				for (Int j = 0; j < oSignal.slots0.Length(); j++)
				{
					slots0.Add(((SlotBase0<Void> *) oSignal.slots0.GetNth(j))->Copy());
				}

				parent = oSignal.parent;

				return *this;
			}

			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> Int Connect(slotReturnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				slotsN.Add(new SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst));

				return Success();
			}

			template <class slotReturnTYPE> Int Connect(slotReturnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				slotsN.Add(new SIGNALS_SLOT_GLOBAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc));

				return Success();
			}

			template <class slotReturnTYPE> Int Connect(SIGNALS_SIGNAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				slotsN.Add(new SIGNALS_SLOT_SIGNAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig));
		
				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> Int Connect(slotReturnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				slots0.Add(new SlotClass0<oClassTYPE, slotReturnTYPE>(proc, inst));

				return Success();
			}

			template <class slotReturnTYPE> Int Connect(slotReturnTYPE (*proc)())
			{
				slots0.Add(new SlotGlobal0<slotReturnTYPE>(proc));

				return Success();
			}

			template <class slotReturnTYPE> Int Connect(Signal0<slotReturnTYPE> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				slots0.Add(new SlotSignal0<slotReturnTYPE>(sig));

				return Success();
			}
#endif

			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> Int Disconnect(slotReturnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			template <class slotReturnTYPE> Int Disconnect(slotReturnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_GLOBAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			template <class slotReturnTYPE> Int Disconnect(SIGNALS_SIGNAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_SIGNAL_CLASS_NAME<slotReturnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE, class slotReturnTYPE> Int Disconnect(slotReturnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotBase0<Void> *) slots0.GetNth(i))) == SlotClass0<oClassTYPE, slotReturnTYPE>(proc, inst))
					{
						delete (SlotBase0<Void> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			template <class slotReturnTYPE> Int Disconnect(slotReturnTYPE (*proc)())
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotBase0<Void> *) slots0.GetNth(i))) == SlotGlobal0<slotReturnTYPE>(proc))
					{
						delete (SlotBase0<Void> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			template <class slotReturnTYPE> Int Disconnect(Signal0<slotReturnTYPE> *sig)
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotBase0<Void> *) slots0.GetNth(i))) == SlotSignal0<slotReturnTYPE>(sig))
					{
						delete (SlotBase0<Void> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}
#endif

			Int DisconnectAll()
			{
				for (Int i = 0; i < slotsN.Length(); i++)	delete (SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);
				for (Int j = 0; j < slots0.Length(); j++)	delete (SlotBase0<Void> *) slots0.GetNth(j);

				slotsN.RemoveAll();
				slots0.RemoveAll();

				return Success();
			}

			Void Emit(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				ProtectParent();

				for (Int i = 0; i < slotsN.Length(); i++)	((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				for (Int j = 0; j < slots0.Length(); j++)	((SlotBase0<Void> *) slots0.GetNth(j))->Emit();

				UnprotectParent();
			}

			Void EmitUnprotected(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				for (Int i = 0; i < slotsN.Length(); i++)	((SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				for (Int j = 0; j < slots0.Length(); j++)	((SlotBase0<Void> *) slots0.GetNth(j))->Emit();
			}

			Int GetNOfConnectedSlots() const
			{
				return slotsN.Length() + slots0.Length();
			}
	};
};
