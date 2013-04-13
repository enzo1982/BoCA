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
 * @file XspfReaderCallback.cpp
 * Implementation of XspfReaderCallback.
 */

#include <xspf/XspfReaderCallback.h>
#include <xspf/XspfTrack.h>
#include <xspf/XspfProps.h>


namespace Xspf {


XspfReaderCallback::XspfReaderCallback() {

}


XspfReaderCallback::~XspfReaderCallback() {

}


void
XspfReaderCallback::addTrack(XspfTrack * track) {
	delete track;
}


void XspfReaderCallback::setProps(XspfProps * props) {
	delete props;
}


void
XspfReaderCallback::notifyFatalError(int /*line*/, int /*column*/,
		int /*errorCode*/, XML_Char const * /*description*/) {

}


bool
XspfReaderCallback::handleError(int /*line*/, int /*column*/,
		int /*errorCode*/, XML_Char const * /*description*/) {
	return true; // Continue parsing
}


bool
XspfReaderCallback::handleWarning(int /*line*/, int /*column*/,
		int /*warningCode*/, XML_Char const * /*description*/) {
	return true; // Continue parsing
}


void
XspfReaderCallback::notifySuccess() {

}


void
XspfReaderCallback::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
