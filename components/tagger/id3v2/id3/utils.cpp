// $Id$

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

/* This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/
 */

#include <ctype.h>

#if (defined(__GNUC__) && __GNUC__ == 2)
#	define NOCREATE ios::nocreate
#else
#	if defined(macintosh)  //not sure if this is still needed
#		define toascii(X) (X)  //not sure if this is still needed
#	endif                  //not sure if this is still needed

#	define NOCREATE ((std::ios_base::openmode)0)
#endif

#include <id3/utils.h>

/* Check if we have all unicodes
 */
#if defined HAVE_ICONV_H
#	if (defined(ID3_ICONV_FORMAT_UTF16BE) && defined(ID3_ICONV_FORMAT_UTF16LE) && defined(ID3_ICONV_FORMAT_UTF8) && defined(ID3_ICONV_FORMAT_ASCII))
#		include <iconv.h>
#		include <errno.h>
#	else
#		undef HAVE_ICONV_H
#	endif
#endif

using namespace dami;

/* Converts an ASCII string into a Unicode one
 */
dami::String mbstoucs(dami::String data)
{
	size_t		 size = data.size();
	dami::String	 unicode(size * 2, '\0');

	for (size_t i = 0; i < size; ++i)
	{
		unicode[i * 2 + 1] = toascii(data[i]);
	}

	return unicode;
}

/* Converts a Unicode string into ASCII
 */
dami::String ucstombs(dami::String data)
{
	size_t		 size = data.size() / 2;
	dami::String	 ascii(size, '\0');

	for (size_t i = 0; i < size; ++i)
	{
		ascii[i] = toascii(data[i * 2 + 1]);
	}

	return ascii;
}

dami::String oldconvert(dami::String data, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
	dami::String	 target;

	if	(ID3TE_IS_SINGLE_BYTE_ENC(sourceEnc) && ID3TE_IS_DOUBLE_BYTE_ENC(targetEnc)) target = mbstoucs(data);
	else if (ID3TE_IS_DOUBLE_BYTE_ENC(sourceEnc) && ID3TE_IS_SINGLE_BYTE_ENC(targetEnc)) target = ucstombs(data);

	return target;
}

size_t dami::renderNumber(uchar *buffer, uint32 val, size_t size)
{
	uint32	 num = val;

	for (size_t i = 0; i < size; i++)
	{
		buffer[size - i - 1] = (uchar) (num & MASK8);
		num >>= 8;
	}

	return size;
}

String dami::renderNumber(uint32 val, size_t size)
{
	String	 str(size, '\0');
	uint32	 num = val;

	for (size_t i = 0; i < size; i++)
	{
		str[size - i - 1] = (uchar) (num & MASK8);
		num >>= 8;
	}

	return str;
}


#if defined(HAVE_ICONV_H)

namespace
{
	String convert_i(iconv_t cd, String source)
	{
		String		 target;
		size_t		 source_size = source.size();
#if defined(ID3LIB_ICONV_OLDSTYLE)
		const char	*source_str = source.data();
#else
		char		*source_str = new char[source.size() + 1];

		source.copy(source_str, String::npos);
		source_str[source.length()] = 0;
#endif

#define ID3LIB_BUFSIZ 1024

		char	 buf[ID3LIB_BUFSIZ];
		char	*target_str = buf;
		size_t	 target_size = ID3LIB_BUFSIZ;

		do
		{
			errno = 0;

			size_t	 nconv = iconv(cd, &source_str, &source_size,
						   &target_str, &target_size);

			if (nconv == (size_t) -1 && errno != EINVAL && errno != E2BIG)
			{
				/* errno is probably EILSEQ here, which means either an invalid
				 * byte sequence or a valid but unconvertible byte sequence.
				 */
#if !defined(ID3LIB_ICONV_OLDSTYLE)
				delete [] source_str;
#endif

				return target;
			}

			target.append(buf, ID3LIB_BUFSIZ - target_size);
			target_str = buf;
			target_size = ID3LIB_BUFSIZ;
		}
		while (source_size > 0);

#if !defined(ID3LIB_ICONV_OLDSTYLE)
		delete [] source_str;
#endif

		return target;
	}

	const char *getFormat(ID3_TextEnc enc)
	{
		const char	*format = NULL;

		switch (enc)
		{
			case ID3TE_ISO8859_1:
				format = ID3_ICONV_FORMAT_ASCII;
				break;
			case ID3TE_UTF16:
				format = ID3_ICONV_FORMAT_UTF16LE;
				break;
			case ID3TE_UTF16BE:
				format = ID3_ICONV_FORMAT_UTF16BE;
				break;
			case ID3TE_UTF8:
				format = ID3_ICONV_FORMAT_UTF8;
				break;
			default:
				break;
		}

		return format;
	}
}
#endif

String dami::convert(String data, ID3_TextEnc sourceEnc, ID3_TextEnc targetEnc)
{
	String	 target;

	if ((sourceEnc != targetEnc) && (data.size() > 0))
	{
#if !defined HAVE_ICONV_H
		target = oldconvert(data, sourceEnc, targetEnc);
#else
		const char	*targetFormat = getFormat(targetEnc);
		const char	*sourceFormat = getFormat(sourceEnc);

		iconv_t		 cd = iconv_open(targetFormat, sourceFormat);

		if (cd != (iconv_t) -1)
		{
			target = convert_i(cd, data);

			if (target.size() == 0)
			{
				/* Try without iconv.
				 */
				target = oldconvert(data, sourceEnc, targetEnc);
			}
		}
		else
		{
			target = oldconvert(data, sourceEnc, targetEnc);
		}

		iconv_close (cd);
#endif
	}

	return target;
}

size_t dami::ucslen(const unicode_t *unicode)
{
	if (unicode == NULL) return 0;

	for (size_t size = 0; true; size++)
	{
		if (unicode[size] == NULL_UNICODE) return size;
	}

	return 0;
}

namespace
{
	bool exists(String name)
	{
		ifstream	 file(name.c_str(), NOCREATE);

		return file.is_open() != 0;
	}
};

ID3_Err dami::createFile(String name, fstream &file)
{
	if (file.is_open()) file.close();

	file.open(name.c_str(), ios::in | ios::out | ios::binary | ios::trunc);

	if (!file) return ID3E_ReadOnly;

	return ID3E_NoError;
}

size_t dami::getFileSize(fstream &file)
{
	size_t	 size = 0;

	if (file.is_open())
	{
		streamoff	 curpos = file.tellg();

		file.seekg(0, ios::end);

		size = file.tellg();

		file.seekg(curpos);
	}

	return size;
}

size_t dami::getFileSize(ifstream &file)
{
	size_t	 size = 0;

	if (file.is_open())
	{
		streamoff	 curpos = file.tellg();

		file.seekg(0, ios::end);

		size = file.tellg();

		file.seekg(curpos);
	}

	return size;
}

size_t dami::getFileSize(ofstream &file)
{
	size_t	 size = 0;

	if (file.is_open())
	{
		streamoff	 curpos = file.tellp();

		file.seekp(0, ios::end);

		size = file.tellp();

		file.seekp(curpos);
	}

	return size;
}

ID3_Err dami::openWritableFile(String name, fstream &file)
{
	if (!exists(name)) return ID3E_NoFile;

	if (file.is_open()) file.close();

	file.open(name.c_str(), ios::in | ios::out | ios::binary | NOCREATE);

	if (!file) return ID3E_ReadOnly;

	return ID3E_NoError;
}

ID3_Err dami::openWritableFile(String name, ofstream &file)
{
	if (!exists(name)) return ID3E_NoFile;

	if (file.is_open()) file.close();

	file.open(name.c_str(), ios::in | ios::out | ios::binary | NOCREATE);

	if (!file) return ID3E_ReadOnly;

	return ID3E_NoError;
}

ID3_Err dami::openReadableFile(String name, fstream &file)
{
	if (file.is_open()) file.close();

	file.open(name.c_str(), ios::in | ios::binary | NOCREATE);

	if (!file) return ID3E_NoFile;

	return ID3E_NoError;
}

ID3_Err dami::openReadableFile(String name, ifstream &file)
{
	if (file.is_open()) file.close();

	file.open(name.c_str(), ios::in | ios::binary | NOCREATE);

	if (!file) return ID3E_NoFile;

	return ID3E_NoError;
}

String dami::toString(uint32 val)
{
	if (val == 0) return "0";

	String	 text;

	while (val > 0)
	{
		String	 tmp;
		char	 ch = (val % 10) + '0';

		tmp += ch;
		text = tmp + text;
		val /= 10;
	}

	return text;
}

WString dami::toWString(const unicode_t buf[], size_t len)
{
	WString	 str;

	str.reserve(len);

	for (size_t i = 0; i < len; ++i)
	{
		str += static_cast<WString::value_type>(buf[i]);
	}

	return str;
}
