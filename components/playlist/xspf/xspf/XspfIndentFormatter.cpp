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
 * @file XspfIndentFormatter.cpp
 * Implementation of XspfIndentFormatter.
 */

#include <xspf/XspfIndentFormatter.h>
#include <xspf/XspfStack.h>

namespace Xspf {


/**
 * Specifies the type of basic XML unit.
 */
enum XspfElemPos {
	XSPF_ELEM_DUMMY, ///< Stack returns 0 if empty

	XSPF_ELEM_START, ///< Opening tag
	XSPF_ELEM_BODY ///< Tag body
};


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfIndentFormatter.
 */
class XspfIndentFormatterPrivate {

	friend class XspfIndentFormatter;

	int level; ///< Element tree depth
	XspfStack<unsigned int> stack; ///< Tag position stack
	int shift; ///< Indent shift

	/**
	 * Creates a new D object.
	 *
	 * @param shift          Non-positive indent shift (-2 will create two tabs less)
	 */
	XspfIndentFormatterPrivate(int shift)
			: level(0), stack(), shift(shift) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfIndentFormatterPrivate(const XspfIndentFormatterPrivate &
			source)
			: level(source.level),
			stack(source.stack),
			shift(source.shift) {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfIndentFormatterPrivate() {
		this->stack.clear();
	}

};


/// @endcond

XspfIndentFormatter::XspfIndentFormatter(int shift)
		: XspfXmlFormatter(),
		d(new XspfIndentFormatterPrivate(shift)) {
	// Fix broken indent shift
	if (this->d->shift > 0) {
		this->d->shift = 0;
	}
}


XspfIndentFormatter::XspfIndentFormatter(XspfIndentFormatter const & source)
		: XspfXmlFormatter(source),
		d(new XspfIndentFormatterPrivate(*(source.d))) {

}


XspfIndentFormatter & XspfIndentFormatter::operator=(XspfIndentFormatter const & source) {
	if (this != &source) {
		XspfXmlFormatter::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfIndentFormatter::~XspfIndentFormatter() {
	delete this->d;
}


void XspfIndentFormatter::writeStart(XML_Char const * name,
		XML_Char const * const * atts) {
	// XML Header
	this->writeXmlDeclaration();
	*this->getOutput() << _PT("\n");

	// Indent
	for (int i = -(this->d->shift); i < this->d->level; i++) {
		*this->getOutput() << _PT('\t');
	}

	// Element
	*this->getOutput() << _PT('<') << name;
	while (atts[0] != NULL) {
		*this->getOutput() << _PT(' ') << atts[0] << _PT("=\"") << atts[1] << _PT("\"");
		atts += 2;
	}
	*this->getOutput() << _PT(">");

	this->d->level++;
	this->d->stack.push(XSPF_ELEM_START);
}


void XspfIndentFormatter::writeEnd(XML_Char const * name) {
	this->d->level--;
	if (this->d->stack.top() != XSPF_ELEM_BODY) {
		*this->getOutput() << _PT('\n');
		for (int i = -(this->d->shift); i < this->d->level; i++) {
			*this->getOutput() << _PT('\t');
		}
	} else {
		// Pop body
		this->d->stack.pop();
	}
	// Pop start
	this->d->stack.pop();

	*this->getOutput() << _PT("</") << name << _PT('>');

	if (this->d->level == 0) {
		*this->getOutput() << _PT("\n");
	}
}


void XspfIndentFormatter::writeBody(XML_Char const * text) {
	writeCharacterData(text);
	this->d->stack.push(XSPF_ELEM_BODY);
}


void XspfIndentFormatter::writeBody(int number) {
	*this->getOutput() << number;
	this->d->stack.push(XSPF_ELEM_BODY);
}


}
