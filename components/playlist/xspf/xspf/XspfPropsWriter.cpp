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
 * @file XspfPropsWriter.cpp
 * Implementation of XspfPropsWriter.
 */

#include "XspfPropsWriter.h"
#include <xspf/XspfProps.h>
#include <xspf/XspfDateTime.h>
#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfExtension.h>
#include <xspf/XspfExtensionWriter.h>
#include <xspf/XspfToolbox.h>
#include <list>
#include <cstdio>
#include <cstdlib>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfPropsWriter.
 */
class XspfPropsWriterPrivate {

	friend class XspfPropsWriter;

	XspfProps props; ///< Playlist properties to write
	bool trackListEmpty; ///< Tracklist empty flag, no initialization needed
	std::list<std::pair<XML_Char const *, XML_Char *> > initNamespaces; ///< Namespaces to register when writing
	bool embedBase; ///< Embed Base URI as xml:base flag, no initialization needed

	/**
	 * Creates a new D object.
	 */
	XspfPropsWriterPrivate() { }

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsWriterPrivate(XspfPropsWriterPrivate const & source)
			: props(source.props),
			trackListEmpty(source.trackListEmpty),
			embedBase(source.embedBase) {
		copyNamespaceInits(this->initNamespaces, source.initNamespaces);
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsWriterPrivate & operator=(XspfPropsWriterPrivate const & source) {
		if (this != &source) {
			this->props = source.props;
			this->trackListEmpty = source.trackListEmpty;
			this->embedBase = source.embedBase;
			freeNamespaceInits(this->initNamespaces);
			copyNamespaceInits(this->initNamespaces, source.initNamespaces);
		}
		return *this;
	}

	static void freeNamespaceInits(std::list<std::pair<
			XML_Char const *, XML_Char *> >	& container) {
		std::list<std::pair<XML_Char const *, XML_Char *> >::iterator
			iter = container.begin();
		while (iter != container.end()) {
			std::pair<XML_Char const *, XML_Char *> & entry = *iter;
			delete [] entry.second;
			iter++;
		}
		container.clear();
	}

	static void copyNamespaceInits(
			std::list<std::pair<XML_Char const *, XML_Char *> >
				& dest,
			const std::list<std::pair<XML_Char const *, XML_Char *> >
				& source) {
		std::list<std::pair<XML_Char const *, XML_Char *> >::const_iterator
			iter = source.begin();
		while (iter != source.end()) {
			const std::pair<XML_Char const *, XML_Char *> & entry = *iter;

			XML_Char const * const uri = entry.first;
			XML_Char * const prefixSuggestion = Toolbox::newAndCopy(entry.second);
			dest.push_back(
					std::pair<XML_Char const *, XML_Char *>(
					uri, prefixSuggestion));

			iter++;
		}
	}

	/**
	 * Destroys this D object.
	 */
	~XspfPropsWriterPrivate() {
		freeNamespaceInits(this->initNamespaces);
	}

};

/// @endcond


XspfPropsWriter::XspfPropsWriter()
		: XspfDataWriter(),
		d(new XspfPropsWriterPrivate()) {

}


XspfPropsWriter::XspfPropsWriter(XspfPropsWriter const & source)
		: XspfDataWriter(source),
		d(new XspfPropsWriterPrivate(*(source.d))) {

}


XspfPropsWriter & XspfPropsWriter::operator=(XspfPropsWriter const & source) {
	if (this != &source) {
		XspfDataWriter::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfPropsWriter::~XspfPropsWriter() {
	delete this->d;
}


void
XspfPropsWriter::setProps(XspfProps const * props) {
	this->d->props = (props == NULL)
		? XspfProps()
		: *props;
	setData(&this->d->props);
}


void XspfPropsWriter::writeAttribution() {
	int index = 0;
	std::pair<bool, XML_Char const *> * entry = this->d->props.getAttribution(index++);
	if (entry == NULL) {
		return;
	}

	XML_Char const * atts[1] = {NULL};
	this->getOutput()->writeHomeStart(_PT("attribution"), atts);
	do {
		XML_Char * const relUri = makeRelativeUri(entry->second);
		writePrimitive(entry->first ? _PT("location")
				: _PT("identifier"), relUri);
		delete [] relUri;
		delete entry; // since the pair was created for us
		entry = this->d->props.getAttribution(index++);
	} while (entry != NULL);
	this->getOutput()->writeHomeEnd(_PT("attribution"));
}


void XspfPropsWriter::writeDate() {
	XspfDateTime const * const dateTime = this->d->props.getDate();
	if (dateTime != NULL) {
		int const charCount = 10 + 1 + 8 + 6 + 1;
		XML_Char buffer[charCount];
		::PORT_SNPRINTF(buffer, charCount, _PT("%04i-%02i-%02iT%02i:%02i:%02i%s%02i:%02i"),
				dateTime->getYear(), dateTime->getMonth(), dateTime->getDay(), dateTime->getHour(),
				dateTime->getMinutes(), dateTime->getSeconds(),
				(dateTime->getDistHours() < 0) ? _PT("-") : _PT("+"),
				std::abs(dateTime->getDistHours()), std::abs(dateTime->getDistMinutes()));
		writePrimitive(_PT("date"), buffer);
	}
}


void XspfPropsWriter::writeIdentifier() {
	XML_Char const * const identifier = this->d->props.getIdentifier();
	if (identifier != NULL) {
		XML_Char * const relUri = makeRelativeUri(identifier);
		writePrimitive(_PT("identifier"), relUri);
		delete [] relUri;
	}
}


void XspfPropsWriter::writeLicense() {
	XML_Char const * const license = this->d->props.getLicense();
	if (license != NULL) {
		XML_Char * const relUri = makeRelativeUri(license);
		writePrimitive(_PT("license"), relUri);
		delete [] relUri;
	}
}


void XspfPropsWriter::writeLocation() {
	XML_Char const * const location = this->d->props.getLocation();
	if (location != NULL) {
		XML_Char * const relUri = makeRelativeUri(location);
		writePrimitive(_PT("location"), relUri);
		delete [] relUri;
	}
}


void XspfPropsWriter::writePlaylistClose() {
	this->getOutput()->writeHomeEnd(_PT("playlist"));
}


void XspfPropsWriter::writePlaylistOpen() {
	// Make namespace registration list
	int const count = static_cast<int>(this->d->initNamespaces.size());
	std::list<std::pair<XML_Char const *, XML_Char *> >::iterator iter
			= this->d->initNamespaces.begin();
	XML_Char const ** nsRegs = new const XML_Char *[2 * (1 + count) + 1];
	nsRegs[0] = XspfXmlFormatter::namespaceKey;
	nsRegs[1] = _PT("");
	int walk = 2;
	while (iter != this->d->initNamespaces.end()) {
		std::pair<XML_Char const *, XML_Char *> & entry = *iter;
		nsRegs[walk] = entry.first;
		nsRegs[walk + 1] = entry.second;
		walk += 2;
		iter++;
	}
	nsRegs[walk] = NULL;

	// Make version
	int const charCount = 16;
	XML_Char versionText[charCount];
	::PORT_SNPRINTF(versionText, charCount, _PT("%i"),
			this->d->props.getVersion());
	XML_Char const * atts[2 + 2 + 1] = {_PT("version"), versionText,
			NULL, NULL, NULL}; // Space for xml:base

	// Make xml:base
	XML_Char const * const baseUri = getBaseUri();
	if (this->d->embedBase && (baseUri != NULL)) {
		atts[0 + 2] = _PT("xml:base");
		atts[1 + 2] = baseUri;
	}

	// Write tag
	this->getOutput()->writeStart(XspfXmlFormatter::namespaceKey,
			_PT("playlist"), atts, nsRegs);

	// Delete reg list
	XspfPropsWriterPrivate::freeNamespaceInits(this->d->initNamespaces);
	delete [] nsRegs;
}


void XspfPropsWriter::writeTrackListClose() {
	if (this->d->trackListEmpty) {
		// Whole tag was written already
		return;
	}
	this->getOutput()->writeHomeEnd(_PT("trackList"));
}


void XspfPropsWriter::writeTrackListOpen() {
	XML_Char const * atts[1] = {NULL};
	if (this->d->trackListEmpty) {
		if (this->d->props.getVersion() > 0) {
			// Empty trackList allowed
			this->getOutput()->writeHomeStart(_PT("trackList"), atts);
			this->getOutput()->writeHomeEnd(_PT("trackList"));
		} else {
			// Empty trackList forbidden
			this->getOutput()->writeHomeStart(_PT("trackList"), atts);
			this->getOutput()->writeHomeStart(_PT("track"), atts);
			this->getOutput()->writeHomeEnd(_PT("track"));
			this->getOutput()->writeHomeEnd(_PT("trackList"));
		}
	} else {
		this->getOutput()->writeHomeStart(_PT("trackList"), atts);
	}
}


void
XspfPropsWriter::init(XspfXmlFormatter & output,
		XML_Char const * baseUri, bool embedBase) {
	this->getOutput() = &output;
	setBaseUri(baseUri);
	this->d->embedBase = embedBase;
}


void
XspfPropsWriter::writeStartPlaylist() {
	writePlaylistOpen();
	writeTitle();
	writeCreator();
	writeAnnotation();
	writeInfo();
	writeLocation();
	writeIdentifier();
	writeImage();
	writeDate();
	writeLicense();
	writeAttribution();
	writeLinks();
	writeMetas();
	if (this->d->props.getVersion() > 0) {
		writeExtensions();
	}
}


void
XspfPropsWriter::writeStartTracklist(bool trackListEmpty) {
	this->d->trackListEmpty = trackListEmpty;
	writeTrackListOpen();
}


void
XspfPropsWriter::writeEndTracklist() {
	writeTrackListClose();
}


void
XspfPropsWriter::writeEndPlaylist() {
	writePlaylistClose();
}


bool
XspfPropsWriter::registerNamespace(XML_Char const * uri,
		XML_Char const * prefixSuggestion) {
	// TODO too late?
	this->d->initNamespaces.push_back(std::pair<XML_Char const *,
			XML_Char *>(uri, Toolbox::newAndCopy(prefixSuggestion)));
	return true;
}


}
