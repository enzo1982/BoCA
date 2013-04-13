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
 * @file XspfExtension.cpp
 * Implementation of XspfExtension.
 */

#include <xspf/XspfExtension.h>
#include <xspf/XspfToolbox.h>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfExtension.
 */
class XspfExtensionPrivate {

	friend class XspfExtension;

	XML_Char const * applicationUri; ///< Application URI

	/**
	 * Creates a new D object.
	 */
	XspfExtensionPrivate(XML_Char const * applicationUri)
			: applicationUri(Toolbox::newAndCopy(applicationUri)) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionPrivate(XspfExtensionPrivate const & source)
			: applicationUri(Toolbox::newAndCopy(source.applicationUri)) {

	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionPrivate & operator=(XspfExtensionPrivate const & source) {
		if (this != &source) {
			delete [] this->applicationUri;
			this->applicationUri = Toolbox::newAndCopy(source.applicationUri);
		}
		return *this;
	}


	/**
	 * Destroys this D object.
	 */
	~XspfExtensionPrivate() {
		delete [] this->applicationUri;
	}

};

/// @endcond


XspfExtension::XspfExtension(XML_Char const * applicationUri)
		: d(new XspfExtensionPrivate(applicationUri)) {

}


XspfExtension::XspfExtension(XspfExtension const & source)
		: d(new XspfExtensionPrivate(*(source.d))) {

}


XspfExtension &
XspfExtension::operator=(XspfExtension const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfExtension::~XspfExtension() {
	delete this->d;
}


XML_Char const *
XspfExtension::getApplicationUri() const {
	return this->d->applicationUri;
}


void
XspfExtension::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
