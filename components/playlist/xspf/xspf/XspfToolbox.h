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
 * @file XspfToolbox.h
 * Interface of Xspf::Toolbox.
 */

#ifndef XSPF_TOOLBOX_H
#define XSPF_TOOLBOX_H


#include "XspfDefines.h"
#include <string>
#include <cstring>

namespace Xspf {


class XspfDateTime;


/**
 * Provides common helper functions.
 */
namespace Toolbox {


/// @cond DOXYGEN_NON_API

/**
 * Compares the content of two strings.
 * Used with STL containers.
 */
struct XspfStringCompare {

	/**
	 * Returns true if string <code>s1</code> is lower than
	 * <code>s2</code>, false otherwise.
	 *
	 * @return	<code>s1</code> &lt; <code>s2</code>
	 */
	bool operator()(XML_Char const * s1, XML_Char const * s2) const;

};

/// @endcond


/**
 * Duplicates the string (using new() not malloc())
 * and returns the duplicate. If <code>source</code>
 * is NULL the return value also is NULL.
 *
 * @param source  Source text
 * @return        Duplacated text or NULL
 */
XML_Char * newAndCopy(XML_Char const * source);

/**
 * Replaces the string in <c>*dest</c> by a duplicate of the string in <c>src</c>
 * (using new() not malloc()). The old string is deleted.
 *
 * @param dest	Destination text
 * @param src	Source text
 */
void deleteNewAndCopy(XML_Char ** dest, XML_Char const * src);

/**
 * Replaces the string in <c>dest</c> by a duplicate of the string in <c>src</c>
 * (using new() not malloc()). The old string is deleted. If <c>destOwnership</c>
 * is false the old string is not deleted. If <c>sourceCopy</c> is false only
 * <c>source</c>'s pointer is copied, not the string.
 *
 * @param dest			Destination text
 * @param destOwnership	Destination ownership flag
 * @param source		Source text
 * @param sourceCopy	Source copy flag
 */
void deleteNewAndCopy(XML_Char const * & dest, bool & destOwnership,
	XML_Char const * source, bool sourceCopy);

/**
 * Sets a pointer to <c>NULL</c> and returns the
 * original value. This helper is used for <i>stealing</i>
 * memory.
 *
 * @param dest	Destination
 * @return		Old value
 */
template <class T>
T const * getSetNull(T const * & dest) {
	T const * backup = dest;
	dest = NULL;
	return backup;
}


/**
 * Copies a string's content if owned or just it's address if not.
 *
 * @param dest       Reference of destination string
 * @param ownDest    Reference of destination owner flag
 * @param source     Source string
 * @param ownSource  Source owner flag
 */
void copyIfOwned(XML_Char const * & dest, bool & ownDest, XML_Char const * source, bool ownSource);


/**
 * Deletes the text behind <code>dest</code> if owned
 * and non-NULL.
 * NOTE: <code>dest</code> is not set to NULL after
 *
 * @param dest     Reference of string to delete
 * @param ownDest  Owner flag, false will prevent deletion
 */
void freeIfOwned(XML_Char const * & dest, bool ownDest);


/**
 * Resolves a URI reference against a given base URI. If the given URI already
 * is absolute the result will be that URI again. You can think of this function
 * as "make my URI absolute if it isn't already".
 * Please note you are responbsible to run delete [] on the return value.
 *
 * @param sourceUri		Relative URI to resolve
 * @param baseUri		Absolute base URI to resolve against
 * @return				NULL or an absolute URI allocated with new[].
 * @since 1.0.0rc1
 */
XML_Char * makeAbsoluteUri(XML_Char const * sourceUri, XML_Char const * baseUri);


/**
 * Reduces a given absolute URI to a relative URI reference if possible.
 * If there is no commonality between <c>sourceUri</c> and <c>baseUri</c>
 * the resulting URI will still be absolute. You can think of this function
 * as "make my URI relative if possible".
 * Please note you are responbsible to run delete [] on the return value.
 *
 * @param sourceUri		Absolute URI to reduce
 * @param baseUri		Absolute base URI to reduce against
 * @return				NULL or an URI allocated with new[].
 * @since 1.0.0rc1
 */
XML_Char * makeRelativeUri(XML_Char const * sourceUri, XML_Char const * baseUri);


/**
 * Checks wether <c>text</c> is a valid URI.
 *
 * @param text	Text
 * @return		Valid URI flag
 * @since 1.0.0rc1
 */
bool isUri(XML_Char const * text);


/**
 * Checks wether <c>text</c> is a valid absolute URI.
 *
 * @param text	Text
 * @return		Valid absolute URI flag
 * @since 1.0.0rc1
 */
bool isAbsoluteUri(XML_Char const * text);


/**
 * Extracts an integer from <c>text</c>.
 *
 * @param text				Text
 * @param inclusiveMinimum	Inclusive minimum
 * @param output			Integer storage destination
 * @return					Valid integer less or equal <c>inclusiveMinimum</c> flag
 * @since 1.0.0rc1
 */
bool extractInteger(XML_Char const * text, int inclusiveMinimum, int * output);


/**
 * Checks a string for being all whitespace.
 * Whitespace is: ' ', '\\r', '\\n', '\\t' as defined here:
 * http://www.w3.org/TR/xmlschema-2/#rf-whiteSpace
 *
 * @param text      Text to check
 * @param numChars  Length of <code>text</code> in characters
 * @return          All-whitespace flag
 * @since 1.0.0rc1
 */
bool isWhiteSpace(XML_Char const * text, int numChars);


/**
 * Cuts off whitespace at head and tail. The source string
 * is not modified so this more is a detection function.
 *
 * @param input               Source text
 * @param inputNumChars       Length of source in characters
 * @param blackSpaceStart     Pointer to first non-white character
 * @param blackSpaceNumChars  Length of characters until whitespace tail
 * @since 1.0.0rc1
 */
void cutOffWhiteSpace(XML_Char const * input, int inputNumChars,
		XML_Char const * & blackSpaceStart, int & blackSpaceNumChars);


/**
 * Cuts off whitespace at head and tail.
 *
 * @param target    String object to modify
 * @since 1.0.0rc1
 */
void trimString(std::basic_string<XML_Char> & target);


} // namespace Toolbox
} // namespace Xspf

#endif // XSPF_TOOLBOX_H
