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
 * @file XspfChunkCallback.cpp
 * Implementation of XspfChunkCallback.
 */

#include <xspf/XspfChunkCallback.h>


namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfChunkCallback.
 */
class XspfChunkCallbackPrivate {

	friend class XspfChunkCallback;

	// No membes yet

	/**
	 * Creates a new XspfChunkCallbackPrivate object.
	 */
	XspfChunkCallbackPrivate() {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfChunkCallbackPrivate(XspfChunkCallbackPrivate const & /*source*/) {

	}

};

/// @endcond


XspfChunkCallback::XspfChunkCallback()
		: d(new XspfChunkCallbackPrivate()) {

}


XspfChunkCallback::XspfChunkCallback(XspfChunkCallback const & source)
		: d(new XspfChunkCallbackPrivate(*(source.d))) {

}


XspfChunkCallback & XspfChunkCallback::operator=(XspfChunkCallback const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfChunkCallback::~XspfChunkCallback() {
	delete this->d;
}


void XspfChunkCallback::notifyStop() {

}


void
XspfChunkCallback::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
