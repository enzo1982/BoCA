 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BUFFER
#define H_OBJSMOOTH_BUFFER

#include "../../definitions.h"
#include "../../misc/memory.h"

namespace smooth
{
	template <class t> class Buffer
	{
		private:
			Memory		*memory;

			Int		 size;
			Int		 allocated;
		public:
			Buffer()
			{
				memory		= NIL;

				size		= 0;
				allocated	= 0;
			}

			Buffer(Int iSize)
			{
				memory		= new Memory(iSize * sizeof(t));

				size		= iSize;
				allocated	= iSize;
			}

			Buffer(const Buffer<t> &oBuffer)
			{
				*this = oBuffer;
			}

			~Buffer()
			{
				Free();
			}

			Buffer<t> &operator =(const Buffer<t> &oBuffer)
			{
				if (&oBuffer == this) return *this;

				memory		= oBuffer.memory;

				size		= oBuffer.size;
				allocated	= -1;

				return *this;
			}

			Int Size() const
			{
				return size;
			}

			Bool Resize(Int nSize)
			{
				/* Check if this is actually our memory.
				 */
				if (allocated == -1) return False;

				/* Let's see if we really need to reallocate.
				 */
				if (nSize > allocated)
				{
					if (memory != NIL) memory->Resize(nSize * sizeof(t));
					else		   memory = new Memory(nSize * sizeof(t));

					size		= nSize;
					allocated	= nSize;
				}
				else
				{
					size		= nSize;
				}

				return True;
			}

			Bool Zero()
			{
				if (memory == NIL) return True;

				memset((void *) *memory, 0, size * sizeof(t));

				return True;
			}

			Bool Free()
			{
				/* Check if this is actually our memory.
				 */
				if (allocated == -1) return False;

				/* Nothing to free?
				 */
				if (memory == NIL) return True;

				delete memory;

				memory		= NIL;

				size		= 0;
				allocated	= 0;

				return True;
			}

			inline t &operator	 [](const int n)	{ return ((t *) (void *) *memory)[n]; }
			inline t &operator	 [](const Int n)	{ return (*this)[(int) n]; }

			inline t operator	 [](const int n) const	{ return ((t *) (void *) *memory)[n]; }
			inline t operator	 [](const Int n) const	{ return (*this)[(int) n]; }

			inline operator		 t *() const		{ return (t *) (memory == NIL ? NIL : (void *) *memory); }
	};
};

#endif
