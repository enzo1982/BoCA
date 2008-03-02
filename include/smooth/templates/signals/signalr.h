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
				for (int i = 0; i < oSignal.slotsN.Length(); i++)
				{
					slotsN.Add(((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) oSignal.slotsN.GetNth(i))->Copy());
				}

				for (int j = 0; j < oSignal.slots0.Length(); j++)
				{
					slots0.Add(((SlotRBase0<returnTYPE> *) oSignal.slots0.GetNth(j))->Copy());
				}

				parent = oSignal.parent;

				return *this;
			}

			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				slotsN.Add(new SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst));

				return Success();
			}

			Int Connect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				slotsN.Add(new SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc));

				return Success();
			}

			Int Connect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				slotsN.Add(new SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig));
		
				return Success();
			}

			Int Connect(const returnTYPE value)
			{
				slots0.Add(new SlotRValue0<returnTYPE>(value));

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Connect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				slots0.Add(new SlotRClass0<oClassTYPE, returnTYPE>(proc, inst));

				return Success();
			}

			Int Connect(returnTYPE (*proc)())
			{
				slots0.Add(new SlotRGlobal0<returnTYPE>(proc));

				return Success();
			}

			Int Connect(SignalR0<returnTYPE> *sig)
			{
				if ((Signal *) sig == (Signal *) this) return Error();

				slots0.Add(new SlotRSignal0<returnTYPE>(sig));

				return Success();
			}
#endif

			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(SIGNALS_ARGUMENT_TYPES), oClassTYPE *inst)
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_CLASS_CLASS_NAME<oClassTYPE, returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc, inst))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)(SIGNALS_ARGUMENT_TYPES))
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(proc))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			Int Disconnect(SIGNALS_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *sig)
			{
				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))) == SIGNALS_SLOT_SIGNAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(sig))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);

						slotsN.Remove(slotsN.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			Int Disconnect(const returnTYPE value)
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slots0.GetNth(i))) == SlotRValue0<returnTYPE>(value))
					{
						delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

#ifndef SIGNALS_SIGNAL_ZERO
			template <class classTYPE, class oClassTYPE> Int Disconnect(returnTYPE (classTYPE::*proc)(), oClassTYPE *inst)
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0.GetNth(i))) == SlotRClass0<oClassTYPE, returnTYPE>(proc, inst))
					{
						delete (SlotRBase0<returnTYPE> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			Int Disconnect(returnTYPE (*proc)())
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0.GetNth(i))) == SlotRGlobal0<returnTYPE>(proc))
					{
						delete (SlotRBase0<returnTYPE> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}

			Int Disconnect(SignalR0<returnTYPE> *sig)
			{
				for (Int i = 0; i < slots0.Length(); i++)
				{
					if ((*((SlotRBase0<returnTYPE> *) slots0.GetNth(i))) == SlotRSignal0<returnTYPE>(sig))
					{
						delete (SlotRBase0<returnTYPE> *) slots0.GetNth(i);

						slots0.Remove(slots0.GetNthIndex(i));

						break;
					}
				}

				return Success();
			}
#endif

			Int DisconnectAll()
			{
				for (Int i = 0; i < slotsN.Length(); i++)	delete (SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i);
				for (Int j = 0; j < slots0.Length(); j++)	delete (SlotRBase0<returnTYPE> *) slots0.GetNth(j);

				slotsN.RemoveAll();
				slots0.RemoveAll();

				return Success();
			}

			returnTYPE Emit(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				returnTYPE	 returnValue = (returnTYPE) NIL;

				ProtectParent();

				for (Int i = 0; i < slotsN.Length(); i++)	returnValue = ((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				for (Int j = 0; j < slots0.Length(); j++)	returnValue = ((SlotRBase0<returnTYPE> *) slots0.GetNth(j))->Emit();

				UnprotectParent();

				return returnValue;
			}

			returnTYPE EmitUnprotected(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				Bool	 last = False;

				for (Int i = 0; i < slotsN.Length(); i++)
				{
					if (i == slotsN.Length() - 1 && slots0.Length() == 0) last = True;

					if (last)	return ((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
					else		((SIGNALS_SLOT_BASE_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> *) slotsN.GetNth(i))->Emit(SIGNALS_ARGUMENT_PARAMETERS);
				}

				for (Int j = 0; j < slots0.Length(); j++)
				{
					if (j == slots0.Length() - 1) last = True;

					if (last)	return ((SlotRBase0<returnTYPE> *) slots0.GetNth(j))->Emit();
					else		((SlotRBase0<returnTYPE> *) slots0.GetNth(j))->Emit();
				}
			}

			Int GetNOfConnectedSlots() const
			{
				return slotsN.Length() + slots0.Length();
			}
	};
};
