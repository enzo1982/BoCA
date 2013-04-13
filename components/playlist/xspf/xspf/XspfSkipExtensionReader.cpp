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
 * @file XspfSkipExtensionReader.cpp
 * Implementation of XspfSkipExtensionReader.
 */

#include "XspfSkipExtensionReader.h"
#include <xspf/XspfReader.h>
#include <xspf/XspfStack.h>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfSkipExtensionReader.
 */
class XspfSkipExtensionReaderPrivate {

	friend class XspfSkipExtensionReader;

	/**
	 * Creates a new D object.
	 */
	XspfSkipExtensionReaderPrivate() {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfSkipExtensionReaderPrivate() {

	}

};

/// @endcond


XspfSkipExtensionReader::XspfSkipExtensionReader(XspfReader * reader)
		: XspfExtensionReader(reader),
		d(new XspfSkipExtensionReaderPrivate()) {

}


XspfSkipExtensionReader::XspfSkipExtensionReader(XspfSkipExtensionReader const & source)
		: XspfExtensionReader(source),
		d(new XspfSkipExtensionReaderPrivate(*(source.d))) {

}


XspfSkipExtensionReader & XspfSkipExtensionReader::operator=(XspfSkipExtensionReader const & source) {
	if (this != &source) {
		XspfExtensionReader::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfSkipExtensionReader::~XspfSkipExtensionReader() {
	delete this->d;
}


bool XspfSkipExtensionReader::handleExtensionStart(
		XML_Char const * /*fullName*/, XML_Char const ** /*atts*/) {
			switch (this->getElementStack().size() + 1) {
	case 2:
		// We only get called on this level for playlist.extension
		// Name and attributes have already been checked.
		this->getElementStack().push(TAG_PLAYLIST_EXTENSION);
		return true;

	case 4:
		if (this->getElementStack().top() == TAG_PLAYLIST_TRACKLIST_TRACK) {
			// Name and attributes have already been checked.
			this->getElementStack().push(TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION);
			return true;
		}
		break;

	}

	this->getElementStack().push(TAG_UNKNOWN);
	return true;
}


bool XspfSkipExtensionReader::handleExtensionEnd(
		XML_Char const * /*fullName*/) {
	this->getElementStack().pop();
	return true;
}


bool XspfSkipExtensionReader::handleExtensionCharacters(
		XML_Char const * /*s*/, int /*len*/) {
	return true;
}


XspfExtension * XspfSkipExtensionReader::wrap() {
	return NULL;
}


XspfExtensionReader * XspfSkipExtensionReader::createBrother(
		XspfReader * reader) const {
	return new XspfSkipExtensionReader(reader);
}


}
