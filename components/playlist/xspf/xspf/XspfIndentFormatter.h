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
 * @file XspfIndentFormatter.h
 * Interface of XspfIndentFormatter.
 */

#ifndef XSPF_INDENT_FORMATTER_H
#define XSPF_INDENT_FORMATTER_H


#include "XspfXmlFormatter.h"

namespace Xspf {


template <class T> class XspfStack;
class XspfIndentFormatterPrivate;


/**
 * Outputs XML with proper indentation and newlines.
 * The finishing newline is optional.
 */
class XspfIndentFormatter : public XspfXmlFormatter {

private:
	/// @cond DOXYGEN_NON_API
	XspfIndentFormatterPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new XspfIndentFormatter object with custom indent shift.
	 *
	 * @param shift		Non-positive indent shift (-2 will create two tabs less)
	 */
	XspfIndentFormatter(int shift = 0);

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfIndentFormatter(XspfIndentFormatter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfIndentFormatter & operator=(XspfIndentFormatter const & source);

	/**
	 * Destroys this XspfIndentFormatter object and deletes all
	 * memory associated with it.
	 */
	~XspfIndentFormatter();

private:
	void writeStart(XML_Char const * name, XML_Char const * const * atts);
	void writeEnd(XML_Char const * name);
	void writeBody(XML_Char const * text);
	void writeBody(int number);

};


}

#endif // XSPF_INDENT_FORMATTER_H
