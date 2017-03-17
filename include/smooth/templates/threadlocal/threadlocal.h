 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADLOCAL
#define H_OBJSMOOTH_THREADLOCAL

#include "../../definitions.h"

#ifdef __WIN32__
#	ifdef __GNUC__
#		define multithread(t) __thread t
#	else
#		define multithread(t) __declspec(thread) t
#	endif
#else
#	if !defined __APPLE__ && !defined __OpenBSD__ && !defined __HAIKU__
#		define multithread(t) __thread t
#	else
#		include <pthread.h>

		namespace smooth
		{
			template <class t> class ThreadLocal
			{
				private:
					pthread_key_t			 key;
				public:
									 ThreadLocal()			{ pthread_key_create(&key, NIL); }
									 ThreadLocal(const t &value)	{ pthread_key_create(&key, NIL); *this = value; }

									~ThreadLocal()			{ pthread_key_delete(key); }

					ThreadLocal<t> &operator	 =(const t &value)		{ pthread_setspecific(key, (const void *) value); return *this; }

					ThreadLocal<t>  operator	 +(const t &value) const	{ return t(*this) + value; }
					ThreadLocal<t>  operator	 -(const t &value) const	{ return t(*this) - value; }
					ThreadLocal<t>  operator	 *(const t &value) const	{ return t(*this) * value; }
					ThreadLocal<t>  operator	 /(const t &value) const	{ return t(*this) / value; }

					ThreadLocal<t>  operator	 <<(const t &value) const	{ return t(*this) << value; }
					ThreadLocal<t>  operator	 >>(const t &value) const	{ return t(*this) >> value; }

					ThreadLocal<t> &operator	 +=(const t &value)		{ return *this = t(*this) + value; }
					ThreadLocal<t> &operator	 -=(const t &value)		{ return *this = t(*this) - value; }
					ThreadLocal<t> &operator	 *=(const t &value)		{ return *this = t(*this) * value; }
					ThreadLocal<t> &operator	 /=(const t &value)		{ return *this = t(*this) / value; }

					ThreadLocal<t> &operator	 <<=(int value)			{ return *this = t(*this) << value; }
					ThreadLocal<t> &operator	 >>=(int value)			{ return *this = t(*this) >> value; }

					inline operator			 t() const			{ return (t) pthread_getspecific(key); }

					inline t operator		 ->()				{ return t(*this); }
					inline const t operator		 ->() const			{ return t(*this); }

					inline Bool operator		 ==(const t &value) const	{ return t(*this) == value; }
					inline Bool operator		 !=(const t &value) const	{ return t(*this) != value; }
			};
		};

#		define multithread(t) ThreadLocal<t>
#	endif
#endif

#endif
