 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "../definitions.h"

#ifndef H_OBJSMOOTH_STRING
#define H_OBJSMOOTH_STRING

namespace smooth
{
	class String;
};

#include "../templates/array.h"
#include "../templates/buffer.h"
#include "../threads/mutex.h"

namespace smooth
{
	class SMOOTHAPI String
	{
		private:
			mutable Threads::Mutex		 mutex;

			Buffer<wchar_t>			 wString;

			static Threads::Mutex		 explodeMutex;

			static Array<char *, void *>	 allocatedBuffers;

			static const char		*GetDefaultEncoding();

			static Void			 DeleteTemporaryBuffers(Bool = False);
		public:
			static Int			 Initialize();
			static Int			 Free();

							 String(const int = NIL);
							 String(const char *);
							 String(const wchar_t *);
							 String(const String &);

							~String();

			Void				 Clean();

			static Bool			 IsANSI(const String &);
			static Bool			 IsUnicode(const String &);

			static const char		*GetInputFormat();
			static char			*SetInputFormat(const char *);

			static const char		*GetOutputFormat();
			static char			*SetOutputFormat(const char *);

			static const char		*GetInternalFormat();

			Int				 ImportFrom(const char *, const char *);
			char				*ConvertTo(const char *) const;

			Int				 ComputeCRC32() const;

			String				 EncodeBase64() const;
			String				 DecodeBase64() const;

			String				&Append(const char *);
			String				&Append(const wchar_t *);
			String				&Append(const String &);

			Int				 Find(const char *) const;
			Int				 Find(const wchar_t *) const;
			Int				 Find(const String &) const;

			Int				 FindLast(const char *) const;
			Int				 FindLast(const wchar_t *) const;
			Int				 FindLast(const String &) const;

			String				&Replace(const char *, const char *);
			String				&Replace(const wchar_t *, const wchar_t *);
			String				&Replace(const char *, const String &);
			String				&Replace(const wchar_t *, const String &);
			String				&Replace(const String &, const String &);

			String				&Copy(const char *);
			String				&Copy(const wchar_t *);
			String				&Copy(const String &);

			String				&CopyN(const char *, const Int);
			String				&CopyN(const wchar_t *, const Int);
			String				&CopyN(const String &, const Int);

			Bool				 Contains(const char *) const;
			Bool				 Contains(const wchar_t *) const;
			Bool				 Contains(const String &) const;

			Int				 Compare(const char *) const;
			Int				 Compare(const wchar_t *) const;
			Int				 Compare(const String &) const;

			Int				 CompareN(const char *, const Int) const;
			Int				 CompareN(const wchar_t *, const Int) const;
			Int				 CompareN(const String &, const Int) const;

			Bool				 StartsWith(const char *) const;
			Bool				 StartsWith(const wchar_t *) const;
			Bool				 StartsWith(const String &) const;

			Bool				 EndsWith(const char *) const;
			Bool				 EndsWith(const wchar_t *) const;
			Bool				 EndsWith(const String &) const;

			String				 SubString(Int, Int) const;

			String				 Head(Int) const;
			String				 Tail(Int) const;

			String				 Trim() const;

			String				&Fill(const Int);
			String				&FillN(const Int, const Int);

			Int				 Length() const;

			Int64				 ToInt() const;
			Float				 ToFloat() const;

			static String			 FromInt(const Int64);
			static String			 FromFloat(const Float);

			/* Case conversion routines for lower, upper and title case
			 * (implemented in generated string_case.cpp)
			 */
			String				 ToLower() const;
			String				 ToUpper() const;
			String				 ToTitle() const;

			const Array<String>		&Explode(const String &) const;
			static Int			 ExplodeFinish();

			static String			 Implode(const Array<String> &, const String & = String());

			wchar_t &operator		 [](const int);
			wchar_t operator		 [](const int) const;

			operator			 char *() const;
			operator			 wchar_t *() const;

			String &operator		 =(const int);
			String &operator		 =(const char *);
			String &operator		 =(const wchar_t *);
			String &operator		 =(const String &);

			Bool operator			 ==(const int) const;
			Bool operator			 ==(const char *) const;
			Bool operator			 ==(const wchar_t *) const;
			Bool operator			 ==(const String &) const;

			Bool operator			 !=(const int) const;
			Bool operator			 !=(const char *) const;
			Bool operator			 !=(const wchar_t *) const;
			Bool operator			 !=(const String &) const;
	};
};

#endif
