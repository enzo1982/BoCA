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
 * @file XspfToolbox.cpp
 * Implementation of Xspf::Toolbox.
 */

#include <xspf/XspfToolbox.h>
#include <uriparser/Uri.h>
#include <string>

namespace Xspf {
namespace Toolbox {


/// @cond DOXYGEN_NON_API


bool
XspfStringCompare::operator()(XML_Char const * s1,
		XML_Char const * s2) const {
	// Same address implies same content
	// and saves the string comparison
	if (s1 == s2) {
		return false;
	} else {
		return ::PORT_STRCMP(s1, s2) < 0;
	}
}

/// @endcond


XML_Char *
newAndCopy(XML_Char const * source) {
	if (source == NULL) {
		return NULL;
	}
	XML_Char * dup = new XML_Char[static_cast<int>(PORT_STRLEN(source)) + 1];
	PORT_STRCPY(dup, source);
	return dup;
}


void
deleteNewAndCopy(XML_Char ** dest, XML_Char const * src) {
	if (dest == NULL) {
		return;
	}

	if (*dest != NULL) {
		delete [] *dest;
	}

	if (src == NULL) {
		*dest = NULL;
	} else {
		int const srcLen = static_cast<int>(::PORT_STRLEN(src));
		if (srcLen > 0) {
			*dest = new XML_Char[static_cast<int>(srcLen) + 1];
			PORT_STRCPY(*dest, src);
		} else {
			*dest = NULL;
		}
	}
}


void
deleteNewAndCopy(XML_Char const * & dest, bool & destOwnership,
		XML_Char const * source, bool sourceCopy) {
	// Delete old memory if owner
	if (destOwnership && (dest != NULL)) {
		delete [] dest;
	}

	if (source == NULL) {
		dest = NULL;
		destOwnership = false;
	} else {
		// Copy new memory if desired
		if (sourceCopy) {
			int const sourceLen = static_cast<int>(::PORT_STRLEN(source));
			if (sourceLen > 0) {
				XML_Char * const tempDest = new XML_Char[static_cast<int>(sourceLen) + 1];
				PORT_STRCPY(tempDest, source);
				dest = tempDest;
				destOwnership = true;
			} else {
				dest = NULL;
				destOwnership = false;
			}
		} else {
			dest = source;
			destOwnership = false;
		}
	}
}


void
copyIfOwned(XML_Char const * & dest, bool & ownDest,
		XML_Char const * source, bool ownSource) {
	if (source != NULL) {
		// Memory owned?
		if (ownSource) {
			// Deep copy, both own their own copy
			dest = newAndCopy(source);
		} else {
			// Shallow copy, both lend the same memory
			dest = source;
		}
		ownDest = ownSource;
	} else {
		dest = NULL;
	}
}


void
freeIfOwned(XML_Char const * & dest, bool ownDest) {
	if (ownDest && (dest != NULL)) {
		delete [] dest;
	}
	// dest = NULL;
}


#ifndef XSPF_DOXYGEN

#ifdef UNICODE
# define UriParserState UriParserStateW
# define UriUri UriUriW
# define uriFreeUriMembers uriFreeUriMembersW
# define uriParseUri uriParseUriW
# define uriAddBaseUri uriAddBaseUriW
# define uriToStringCharsRequired uriToStringCharsRequiredW
# define uriToString uriToStringW
# define uriRemoveBaseUri uriRemoveBaseUriW
#else // ifdef UNICODE
# define UriParserState UriParserStateA
# define UriUri UriUriA
# define uriFreeUriMembers uriFreeUriMembersA
# define uriParseUri uriParseUriA
# define uriAddBaseUri uriAddBaseUriA
# define uriToStringCharsRequired uriToStringCharsRequiredA
# define uriToString uriToStringA
# define uriRemoveBaseUri uriRemoveBaseUriA
#endif // ifdef UNICODE

#endif // ifndef XSPF_DOXYGEN


namespace {

/**
 * Returns a string representation of the given URI
 * allocated with new[]. You are resposible to delete
 * this string when it's not needed anymore.
 *
 * @param uri	URI to make string from
 * @return		Newly-allocated URI string
 */
XML_Char * makeUriString(UriUri const & uri) {
	XML_Char * uriString;
	int charsRequired;
	if (uriToStringCharsRequired(&uri, &charsRequired) != URI_SUCCESS) {
		return NULL;
	}
	charsRequired++;
	uriString = new XML_Char[charsRequired];
	if (uriToString(uriString, &uri, charsRequired, NULL) != URI_SUCCESS) {
		delete [] uriString;
		return NULL;
	}
	return uriString;
}


/**
 * Either resolves (<c>addOrRemoveBase == true</c>) or reduces
 * (<c>addOrRemoveBase == false</c>) a source URI against a base
 * URI. The resulting string has been allocated through new[]
 * and must be deleted manually after.
 *
 * @param sourceUri			Source URI to transform
 * @param baseUri			Base URI to work against
 * @param addOrRemoveBase	Mode switch, see above
 * @return 					<c>NULL</c> or a newly-allocated string
 */
XML_Char * allocTransformUri(XML_Char const * sourceUri,
		XML_Char const * baseUri, bool addOrRemoveBase) {
	UriParserState state;
	UriUri absoluteDest;
	UriUri relativeSource;
	UriUri absoluteBase;

	state.uri = &relativeSource;
	if (uriParseUri(&state, sourceUri) != URI_SUCCESS) {
		uriFreeUriMembers(&relativeSource);
		return NULL;
	}

	state.uri = &absoluteBase;
	if (uriParseUri(&state, baseUri) != URI_SUCCESS) {
		uriFreeUriMembers(&relativeSource);
		uriFreeUriMembers(&absoluteBase);
		return NULL;
	}

	int res;
	if (addOrRemoveBase) {
		res = uriAddBaseUri(&absoluteDest, &relativeSource, &absoluteBase);
	} else {
		res = uriRemoveBaseUri(&absoluteDest, &relativeSource, &absoluteBase, URI_FALSE);
	}

	if (res != URI_SUCCESS) {
		uriFreeUriMembers(&relativeSource);
		uriFreeUriMembers(&absoluteBase);
		return NULL;
	}
	
	XML_Char * const uriString = makeUriString(absoluteDest);
	uriFreeUriMembers(&relativeSource);
	uriFreeUriMembers(&absoluteBase);
	uriFreeUriMembers(&absoluteDest);
	return uriString;
}

} // anon namespace


XML_Char *
makeAbsoluteUri(XML_Char const * sourceUri, XML_Char const * baseUri) {
	bool const ADD_BASE = true;
	return allocTransformUri(sourceUri, baseUri, ADD_BASE);
}


XML_Char *
makeRelativeUri(XML_Char const * sourceUri, XML_Char const * baseUri) {
	bool const REMOVE_BASE = false;
	return allocTransformUri(sourceUri, baseUri, REMOVE_BASE);
}


bool
isUri(XML_Char const * text) {
#ifdef UNICODE
	UriParserStateW stateW;
	UriUriW uriW;
	stateW.uri = &uriW;
	int const res = uriParseUriW(&stateW, text);
	uriFreeUriMembersW(&uriW);
#else
	UriParserStateA stateA;
	UriUriA uriA;
	stateA.uri = &uriA;
	int const res = uriParseUriA(&stateA, text);
	uriFreeUriMembersA(&uriA);
#endif
	return (res == 0);
}


bool
isAbsoluteUri(XML_Char const * text) {
#ifdef UNICODE
	UriParserStateW stateW;
	UriUriW uriW;
	stateW.uri = &uriW;
	int const res = uriParseUriW(&stateW, text);
	bool const validAndAbsolute = (res == 0) && (uriW.scheme.first != NULL);
	uriFreeUriMembersW(&uriW);
#else
	UriParserStateA stateA;
	UriUriA uriA;
	stateA.uri = &uriA;
	int const res = uriParseUriA(&stateA, text);
	bool const validAndAbsolute = (res == 0) && (uriA.scheme.first != NULL);
	uriFreeUriMembersA(&uriA);
#endif
	return validAndAbsolute;
}


bool
extractInteger(XML_Char const * text, int inclusiveMinimum, int * output) {
	int & number = *output;
	number = ::PORT_ATOI(text);
	if ((number < inclusiveMinimum)
			|| ((number == 0) && ::PORT_STRCMP(text, _PT("0")))) {
		return false;
	} else {
		return true;
	}
}


bool
isWhiteSpace(XML_Char const * text,
		int numChars) {
	if ((text == NULL) || (numChars < 1)) {
		return true;
	}

	XML_Char const * walk = text;
	do {
		switch (*walk) {
		case _PT('\0'):
			return true;

		case _PT(' '):
		case _PT('\t'):
		case _PT('\x0a'):
		case _PT('\x0d'):
			walk++;
			break;

		default:
			return false;
		}
	} while (walk - text < numChars);

	return true;
}


void
cutOffWhiteSpace(XML_Char const * input,
		int inputNumChars, XML_Char const * & blackSpaceStart,
		int & blackSpaceNumChars) {
	if ((input == NULL) || (inputNumChars < 1)) {
		blackSpaceStart = NULL;
		blackSpaceNumChars = 0;
		return;
	}

	XML_Char const * walk = input;
	XML_Char const * firstBlackChar = NULL;
	XML_Char const * lastBlackChar = NULL;
	do {
		switch (*walk) {
		case _PT(' '):
		case _PT('\t'):
		case _PT('\x0a'):
		case _PT('\x0d'):
			break;

		default:
			if (firstBlackChar == NULL) {
				firstBlackChar = walk;
			}
			lastBlackChar = walk;
			break;
		}
		walk++;
	} while (walk - input < inputNumChars);

	if (firstBlackChar == NULL) {
		// No black space
		blackSpaceStart = walk; // Right after the string
		blackSpaceNumChars = 0;
	} else {
		// Black space found
		blackSpaceStart = firstBlackChar;
		blackSpaceNumChars = static_cast<int>(lastBlackChar - firstBlackChar) + 1;
	}
}


void
trimString(std::basic_string<XML_Char> & target) {
	XML_Char const * const data = target.data();
	int const len = static_cast<int>(target.length());
	XML_Char const * blackSpaceStart = NULL;
	int blackSpaceNumChars = 0;
	cutOffWhiteSpace(data, len, blackSpaceStart, blackSpaceNumChars);
	if (blackSpaceStart == NULL) {
		target.clear();
	} else {
		target = std::basic_string<XML_Char>(target,
				blackSpaceStart - data, blackSpaceNumChars);
	}
}





}
}
