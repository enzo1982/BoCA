 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BUFFER_
#define _H_OBJSMOOTH_BUFFER_

#include "../../definitions.h"
#include "../../misc/memory.h"

namespace smooth
{
	template <class t> class Buffer
	{
		private:
			Memory		*memory_manager;
			t		*memory;

			Int		 size;
			Int		 allocated;
		public:
			Buffer()
			{
				memory_manager	= NIL;
				memory		= NIL;

				size		= 0;
				allocated	= 0;
			}

			Buffer(Int iSize)
			{
				memory_manager	= new Memory(iSize * sizeof(t));
				memory		= (t *) (void *) *memory_manager;

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
				memory_manager	= oBuffer.memory_manager;
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
					if (memory != NIL) memory_manager->Resize(nSize * sizeof(t));
					else		   memory_manager = new Memory(nSize * sizeof(t));

					memory		= (t *) (void *) *memory_manager;

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

				memset(memory, 0, size * sizeof(t));

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

				delete memory_manager;

				memory_manager	= NIL;
				memory		= NIL;

				size		= 0;
				allocated	= 0;

				return True;
			}

			inline t &operator	 [](const int n)	{ return memory[n]; }
			inline t &operator	 [](const Int n)	{ return (*this)[(int) n]; }

			inline t operator	 [](const int n) const	{ return memory[n]; }
			inline t operator	 [](const Int n) const	{ return (*this)[(int) n]; }

			inline operator		 t *() const		{ return memory; }
	};
};

#endif
