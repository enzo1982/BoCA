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
 * @file XspfTrackWriter.cpp
 * Implementation of XspfTrackWriter.
 */

#include "XspfTrackWriter.h"
#include <xspf/XspfTrack.h>
#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfExtension.h>
#include <xspf/XspfExtensionWriter.h>
#include <cassert>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfTrackWriter.
 */
class XspfTrackWriterPrivate {

	friend class XspfTrackWriter;

private:
	XspfTrack const * track; ///< Track to write
	int version; ///< XSPF version to produce

	/**
	 * Creates a new D object.
	 */
	XspfTrackWriterPrivate() : track(NULL) {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfTrackWriterPrivate() {

	}

};

/// @endcond


XspfTrackWriter::XspfTrackWriter()
		: XspfDataWriter(),
		d(new XspfTrackWriterPrivate()) {

}


XspfTrackWriter::XspfTrackWriter(XspfTrackWriter const & source)
		: XspfDataWriter(source),
		d(new XspfTrackWriterPrivate(*(source.d))) {

}


XspfTrackWriter & XspfTrackWriter::operator=(XspfTrackWriter const & source) {
	if (this != &source) {
		XspfDataWriter::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfTrackWriter::~XspfTrackWriter() {
	delete this->d;
}


void XspfTrackWriter::setTrack(XspfTrack const * track) {
	setData(track);
	this->d->track = track;
}


void
XspfTrackWriter::init(XspfXmlFormatter & output, int version,
		XML_Char const * baseUri) {
	this->getOutput() = &output;
	this->d->version = version;
	setBaseUri(baseUri);
}


void
XspfTrackWriter::write() {
	writeTrackOpen();
	writeLocations();
	writeIdentifiers();
	writeTitle();
	writeCreator();
	writeAnnotation();
	writeInfo();
	writeImage();
	writeAlbum();
	writeTrackNum();
	writeDuration();
	writeLinks();
	writeMetas();
	if (this->d->version > 0) {
		writeExtensions();
	}
	writeTrackClose();
}


void XspfTrackWriter::writeAlbum() {
	assert(this->d->track != NULL);
	XML_Char const * const album = this->d->track->getAlbum();
	if (album != NULL) {
		writePrimitive(_PT("album"), album);
	}
}


void XspfTrackWriter::writeDuration() {
	assert(this->d->track != NULL);
	int const duration = this->d->track->getDuration();
	if (duration != -1) {
		writePrimitive(_PT("duration"), duration);
	}
}


void XspfTrackWriter::writeTrackNum() {
	assert(this->d->track != NULL);
	int const trackNum = this->d->track->getTrackNum();
	if (trackNum != -1) {
		writePrimitive(_PT("trackNum"), trackNum);
	}
}


void XspfTrackWriter::writeLocations() {
	assert(this->d->track != NULL);
	int index = 0;
	XML_Char const * location;
	for (;;) {
		location = this->d->track->getLocation(index++);
		if (location == NULL) {
			return;
		}

		XML_Char * const relUri = makeRelativeUri(location);
		writePrimitive(_PT("location"), relUri);
		delete [] relUri;
	}
}


void XspfTrackWriter::writeIdentifiers() {
	assert(this->d->track != NULL);
	int index = 0;
	XML_Char const * identifier;
	for (;;) {
		identifier = this->d->track->getIdentifier(index++);
		if (identifier == NULL) {
			return;
		}

		XML_Char * const relUri = makeRelativeUri(identifier);
		writePrimitive(_PT("identifier"), relUri);
		delete [] relUri;
	}
}


void XspfTrackWriter::writeTrackClose() {
	this->getOutput()->writeHomeEnd(_PT("track"));
}


void XspfTrackWriter::writeTrackOpen() {
	XML_Char const * atts[1] = {NULL};
	this->getOutput()->writeHomeStart(_PT("track"), atts);
}


}
