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
	template <class returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_LIST> class SIGNALS_SLOT_GLOBAL_CLASS_NAME : public SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>
	{
		private:
			returnTYPE										 (*function)(SIGNALS_ARGUMENT_TYPES);
		public:
														 SIGNALS_SLOT_GLOBAL_CLASS_NAME(returnTYPE (*iFunction)(SIGNALS_ARGUMENT_TYPES))	{ function = iFunction; }
			SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>	*Copy() const										{ return new SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES>(function); }

			Void Emit(SIGNALS_ARGUMENT_PARAMETER_LIST) const
			{
				function(SIGNALS_ARGUMENT_PARAMETERS);
			}

			Bool operator ==(const SIGNALS_SLOT_BASE_CLASS_NAME<Void SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &cInstance) const
			{
				if (function == ((SIGNALS_SLOT_GLOBAL_CLASS_NAME<returnTYPE SIGNALS_CONDITIONAL_COMMA SIGNALS_ARGUMENT_TYPES> &) cInstance).function)	return True;
				else																	return False;
			}
	};
};
