/*
 * libxspf - XSPF playlist handling library
 *
 * Copyright (C) 2006-2008, Sebastian Pipping / Xiph.Org Foundation
 * All rights reserved.
 *
 * Redistribution  and use in source and binary forms, with or without
 * modification,  are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions   of  source  code  must  retain  the   above
 *       copyright  notice, this list of conditions and the  following
 *       disclaimer.
 *
 *     * Redistributions  in  binary  form must  reproduce  the  above
 *       copyright  notice, this list of conditions and the  following
 *       disclaimer   in  the  documentation  and/or  other  materials
 *       provided with the distribution.
 *
 *     * Neither  the name of the Xiph.Org Foundation nor the names of
 *       its  contributors may be used to endorse or promote  products
 *       derived  from  this software without specific  prior  written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT  NOT
 * LIMITED  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND  FITNESS
 * FOR  A  PARTICULAR  PURPOSE ARE DISCLAIMED. IN NO EVENT  SHALL  THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL,    SPECIAL,   EXEMPLARY,   OR   CONSEQUENTIAL   DAMAGES
 * (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES;  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Sebastian Pipping, sping@xiph.org
 */

/**
 * @file XspfDefines.h
 */

#ifndef XSPF_DEFINES_H
#define XSPF_DEFINES_H


#include "XspfVersion.h"


// Namespace handling
#define XSPF_NS_HOME        _PT("http://xspf.org/ns/0/")  ///< XSPF namespace URI
#define XSPF_NS_HOME_LEN    21                            ///< XSPF namespace URI string length
#define XSPF_NS_SEP_CHAR    _PT(' ')                      ///< Namespace separator as character
#define XSPF_NS_SEP_STRING  _PT(" ")                      ///< Namespace separator as string
#define XML_NS_HOME          _PT("http://www.w3.org/XML/1998/namespace") ///< XML namespace URI
#define XML_NS_HOME_LEN      36                            ///< XML namespace URI string length


// Expat 1.95.8 or later
#include <expat.h>
#if (!defined(XML_MAJOR_VERSION) || !defined(XML_MINOR_VERSION) \
		|| !defined(XML_MICRO_VERSION) || (XML_MAJOR_VERSION < 1) \
		|| ((XML_MAJOR_VERSION == 1) && ((XML_MINOR_VERSION < 95) \
		|| ((XML_MINOR_VERSION == 95) && (XML_MICRO_VERSION < 8)))))
# error Expat 1.95.8 or later is required
#endif


/**
 * Maximum size in bytes of a file that
 * will be loaded into a buffer in whole
 */
#define XSPF_MAX_BLOCK_SIZE 100000


/// @cond DOXYGEN_IGNORE

// Deny overriding from outside
#undef XSPF_OS_WINDOWS
#undef XSPF_OS_UNIX


// Portability defines
#if (defined(__WIN32__) || defined(_WIN32) || defined(WIN32))

// Windows =========================
#include <windows.h>
#include <tchar.h>

#define XSPF_OS_WINDOWS 1
// =================================

# ifdef UNICODE
#  ifndef _UNICODE
#   error _UNICODE not defined
#  endif
# else
#  ifdef _UNICODE
#   error UNICODE not defined
#  endif
# endif
#else

// Unix, ANSI ======================
#define XSPF_OS_UNIX 1
// =================================

#endif


// OS-specific selection macro
#ifdef XSPF_OS_WINDOWS
# define XSPF_OS_SELECT(windows, unix) windows
#else
# ifdef XSPF_OS_UNIX
#  define XSPF_OS_SELECT(windows, unix) unix
# endif
#endif


#define PORT_ATOI      XSPF_OS_SELECT(_ttoi,      atoi)
#define PORT_FOPEN     XSPF_OS_SELECT(_tfopen,    fopen)
#define PORT_MAIN      XSPF_OS_SELECT(_tmain,     main)
#define PORT_PRINTF    XSPF_OS_SELECT(_tprintf,   printf)

#ifdef UNICODE
# define PORT_SNPRINTF  XSPF_OS_SELECT(_snwprintf,  snprintf)
#else
# define PORT_SNPRINTF  XSPF_OS_SELECT(_snprintf,  snprintf)
#endif

#define PORT_STRCMP    XSPF_OS_SELECT(_tcscmp,    strcmp)
#define PORT_STRCPY    XSPF_OS_SELECT(_tcscpy,    strcpy)
#define PORT_STRLEN    XSPF_OS_SELECT(_tcslen,    strlen)
#define PORT_STRNCMP   XSPF_OS_SELECT(_tcsncmp,   strncmp)
#define PORT_STRNCPY   XSPF_OS_SELECT(_tcsncpy,   strncpy)
#define PORT_STRNICMP  XSPF_OS_SELECT(_tcsnicmp,  strnicmp)
#define _PT(x)         XSPF_OS_SELECT(_T(x),      x)

/// @endcond


#endif // XSPF_DEFINES_H
