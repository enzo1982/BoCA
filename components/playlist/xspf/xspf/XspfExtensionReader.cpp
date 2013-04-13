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
 * @file XspfExtensionReader.cpp
 * Implementation of XspfExtensionReader.
 */

#include <xspf/XspfExtensionReader.h>
#include <xspf/XspfReader.h>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfExtensionReader.
 */
class XspfExtensionReaderPrivate {

	friend class XspfExtensionReader;

	XspfReader * reader; ///< XspfReader to interact with

	/**
	 * Creates a new D object.
	 *
	 * @param reader	XspfReader to interact with
	 */
	XspfExtensionReaderPrivate(XspfReader * reader)
			: reader(reader) {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfExtensionReaderPrivate() {

	}

};

/// @endcond


XspfExtensionReader::XspfExtensionReader(XspfReader * reader)
		: d(new XspfExtensionReaderPrivate(reader)) {

}


XspfExtensionReader::XspfExtensionReader(XspfExtensionReader const & source)
		: d(new XspfExtensionReaderPrivate(*(source.d))) {

}


XspfExtensionReader & XspfExtensionReader::operator=(XspfExtensionReader const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfExtensionReader::~XspfExtensionReader() {
	delete this->d;
}


bool XspfExtensionReader::handleError(int code, XML_Char const * text) {
	return this->d->reader->handleError(code, text);
}


bool
XspfExtensionReader::handleError(int code, XML_Char const * format,
		XML_Char const * param) {
	return this->d->reader->handleError(code, format, param);
}


XspfStack<unsigned int> &
XspfExtensionReader::getElementStack() const {
	return this->d->reader->getElementStack();
}


XspfStack<std::basic_string<XML_Char> > &
XspfExtensionReader::getBaseUriStack() const {
	return this->d->reader->getBaseUriStack();
}


bool
XspfExtensionReader::handleXmlBaseAttribute(XML_Char const * xmlBase) {
	return this->d->reader->handleXmlBaseAttribute(xmlBase);
}


XspfExtensionReader *
XspfExtensionReader::createBrother() const {
	return createBrother(this->d->reader);
}


void
XspfExtensionReader::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
