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
 * @file XspfDataWriter.cpp
 * Implementation of XspfDataWriter.
 */

#include "XspfDataWriter.h"
#include <xspf/XspfData.h>
#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfExtension.h>
#include <xspf/XspfExtensionWriter.h>
#include <xspf/XspfToolbox.h>
#include <cassert>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfDataWriter.
 */
class XspfDataWriterPrivate {

	friend class XspfDataWriter;

	XspfData const * data; ///< Data object to write
	XspfXmlFormatter * output; ///< Formatter to use
	XML_Char * baseUri; ///< Base URI to reduce URIs against

	/**
	 * Creates a new D object.
	 */
	XspfDataWriterPrivate()
			: data(NULL), output(NULL), baseUri(NULL) {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfDataWriterPrivate() {
		delete [] baseUri;
	}
};

/// @endcond


XspfDataWriter::XspfDataWriter()
		: d(new XspfDataWriterPrivate()) {

}


XspfDataWriter::XspfDataWriter(XspfDataWriter const & source)
		: d(new XspfDataWriterPrivate(*(source.d))) {

}


XspfDataWriter & XspfDataWriter::operator=(XspfDataWriter const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfDataWriter::~XspfDataWriter() {
	delete this->d;
}


XspfXmlFormatter * &
XspfDataWriter::getOutput() const {
	return this->d->output;
}


XML_Char const *
XspfDataWriter::getBaseUri() const {
	return this->d->baseUri;
}


void
XspfDataWriter::setBaseUri(XML_Char const * baseUri) {
	Toolbox::deleteNewAndCopy(&this->d->baseUri, baseUri);
}


void
XspfDataWriter::setData(XspfData const * data) {
	this->d->data = data;
}


void
XspfDataWriter::writePrimitive(XML_Char const * name, XML_Char const * body) {
	XML_Char const * atts[1] = {NULL};
	this->d->output->writeHomeStart(name, atts);
	this->d->output->writeBody(body);
	this->d->output->writeHomeEnd(name);
}


void
XspfDataWriter::writePrimitive(XML_Char const * name, int body) {
	XML_Char const * atts[1] = {NULL};
	this->d->output->writeHomeStart(name, atts);
	this->d->output->writeBody(body);
	this->d->output->writeHomeEnd(name);
}


void
XspfDataWriter::writeImage() {
	assert(this->d->data != NULL);
	XML_Char const * const image = this->d->data->getImage();
	if (image != NULL) {
		XML_Char * const relUri = makeRelativeUri(image);
		writePrimitive(_PT("image"), relUri);
		delete [] relUri;
	}
}


void
XspfDataWriter::writeInfo() {
	assert(this->d->data != NULL);
	XML_Char const * const info = this->d->data->getInfo();
	if (info != NULL) {
		XML_Char * const relUri = makeRelativeUri(info);
		writePrimitive(_PT("info"), relUri);
		delete [] relUri;
	}
}


void
XspfDataWriter::writeAnnotation() {
	assert(this->d->data != NULL);
	XML_Char const * const annotation = this->d->data->getAnnotation();
	if (annotation != NULL) {
		writePrimitive(_PT("annotation"), annotation);
	}
}


void
XspfDataWriter::writeCreator() {
	assert(this->d->data != NULL);
	XML_Char const * const creator = this->d->data->getCreator();
	if (creator != NULL) {
		writePrimitive(_PT("creator"), creator);
	}
}


void
XspfDataWriter::writeTitle() {
	assert(this->d->data != NULL);
	XML_Char const * const title = this->d->data->getTitle();
	if (title != NULL) {
		writePrimitive(_PT("title"), title);
	}
}


void
XspfDataWriter::writeLinks() {
	assert(this->d->data != NULL);
	int index = 0;
	const std::pair<XML_Char const *, XML_Char const *> * entry;
	for (;;) {
		entry = this->d->data->getLink(index++);
		if (entry == NULL) {
			return;
		}

		XML_Char const * atts[3] = {_PT("rel"), entry->first, NULL};
		this->d->output->writeHomeStart(_PT("link"), atts);
		XML_Char * const relUri = makeRelativeUri(entry->second);
		this->d->output->writeBody(relUri);
		delete [] relUri;
		this->d->output->writeHomeEnd(_PT("link"));

		delete entry; // since the pair was created for us
	}
}


void
XspfDataWriter::writeMetas() {
	assert(this->d->data != NULL);
	int index = 0;
	const std::pair<XML_Char const *, XML_Char const *> * entry;
	for (;;) {
		entry = this->d->data->getMeta(index++);
		if (entry == NULL) {
			return;
		}

		XML_Char const * atts[3] = {_PT("rel"), entry->first, NULL};
		this->d->output->writeHomeStart(_PT("meta"), atts);
		this->d->output->writeBody(entry->second);
		this->d->output->writeHomeEnd(_PT("meta"));

		delete entry; // since the pair was created for us
	}
}


void
XspfDataWriter::writeExtensions() {
	assert(this->d->data != NULL);
	int index = 0;
	XspfExtension const * entry;
	for (;;) {
		entry = this->d->data->getExtension(index++);
		if (entry == NULL) {
			return;
		}

		XspfExtensionWriter * const writer
				= entry->newWriter(this->d->output,
				this->d->baseUri);
		if (writer != NULL) {
			writer->write();
			delete writer;
		}
	}
}


XML_Char *
XspfDataWriter::makeRelativeUri(XML_Char const * sourceUri) const {
	if (this->d->baseUri == NULL) {
		// Leave absolute
		return Toolbox::newAndCopy(sourceUri);
	} else {
		XML_Char * const relUri = Toolbox::makeRelativeUri(
				sourceUri, this->d->baseUri);
		// Fall back to original value for better backwards compatibility
		return (relUri != NULL) ? relUri : Toolbox::newAndCopy(sourceUri);
	}
}


} // namespace Xspf
