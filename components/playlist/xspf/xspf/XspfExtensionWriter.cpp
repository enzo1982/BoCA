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
 * @file XspfExtensionWriter.cpp
 * Implementation of XspfExtensionWriter.
 */

#include <xspf/XspfExtensionWriter.h>
#include <xspf/XspfExtension.h>
#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfToolbox.h>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfExtensionWriter.
 */
class XspfExtensionWriterPrivate {

	friend class XspfExtensionWriter;

	XspfExtension const * extension; ///< Extension to write
	XspfXmlFormatter * output; ///< Output formatter to write to
	XML_Char * baseUri; ///< Base URI to reduce URIs against

	/**
	 * Creates a new D object.
	 */
	XspfExtensionWriterPrivate(XspfExtension const * extension,
			XspfXmlFormatter * output, XML_Char const * baseUri)
			: extension(extension), output(output),
			baseUri(Toolbox::newAndCopy(baseUri)) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionWriterPrivate(
			XspfExtensionWriterPrivate const & source)
			: extension(source.extension), output(source.output),
			baseUri(Toolbox::newAndCopy(source.baseUri)) {

	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionWriterPrivate & operator=(
			XspfExtensionWriterPrivate const & source) {
		this->extension = source.extension;
		this->output = source.output;
		Toolbox::deleteNewAndCopy(&this->baseUri, source.baseUri);
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfExtensionWriterPrivate() {
		delete [] this->baseUri;
	}

};

/// @endcond


XspfExtensionWriter::XspfExtensionWriter(
		XspfExtension const * extension, XspfXmlFormatter * output,
		XML_Char const * baseUri)
		: d(new XspfExtensionWriterPrivate(extension, output, baseUri)) {

}


XspfExtensionWriter::XspfExtensionWriter(
		XspfExtensionWriter const & source)
		: d(new XspfExtensionWriterPrivate(*(source.d))) {

}


XspfExtensionWriter & XspfExtensionWriter::operator=(XspfExtensionWriter const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfExtensionWriter::~XspfExtensionWriter() {
	delete this->d;
}


void XspfExtensionWriter::writeExtensionStart(
		XML_Char const * const * nsRegs) {
	// Open <extension>, register namespaces
	XML_Char const * atts[3] = {_PT("application"),
	this->d->extension->getApplicationUri(), NULL};
	this->d->output->writeHomeStart(_PT("extension"), atts, nsRegs);
}


void XspfExtensionWriter::writeExtensionStop() {
	// Close <extension>
	this->d->output->writeHomeEnd(_PT("extension"));
}


XML_Char const * const * XspfExtensionWriter::getNamespaceRegs() {
	// Note "static"!
	static XML_Char const * nsRegs[1] = { NULL };
	return nsRegs;
}


void XspfExtensionWriter::write() {
	writeExtensionStart(getNamespaceRegs());
	writeExtensionBody();
	writeExtensionStop();
}


XspfXmlFormatter * &
XspfExtensionWriter::getOutput() {
	return this->d->output;
}


const XspfExtension *
XspfExtensionWriter::getExtension() {
	return this->d->extension;
}


XML_Char const *
XspfExtensionWriter::getBaseUri() const {
	return this->d->baseUri;
}


void
XspfExtensionWriter::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
