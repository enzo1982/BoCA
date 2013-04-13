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
 * @file XspfWriter.cpp
 * Implementation of XspfWriter.
 */

#include <xspf/XspfWriter.h>
#include "XspfPropsWriter.h"
#include "XspfTrackWriter.h"
#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfProps.h>
#include <xspf/XspfToolbox.h>
#include <cstdio>
#include <string>

namespace Xspf {


/*static*/ bool const XspfWriter::EMBED_AS_XML_BASE = true;
/*static*/ bool const XspfWriter::NO_XML_BASE = false;


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfWriter.
 */
class XspfWriterPrivate {

	friend class XspfWriter;

	XspfXmlFormatter * formatter; ///< Output formatter in use
	XspfPropsWriter propsWriter; ///< Related playlist properties
	std::basic_ostringstream<XML_Char> * accum; ///< Output text accumulator
	bool trackListEmpty; ///< Tracklist empty flag
	bool headerWritten; ///< <c>playlist</c> and <c>trackList</c> tag opened flag
	bool footerWritten; ///< <c>trackList</c> and <c>trackList</c> tag closed flag
	int version; ///< XSPF version to use
	XML_Char * baseUri; ///< Base URI to reduce URIs against

	/**
	 * Creates a new D object.
	 */
	XspfWriterPrivate(XspfXmlFormatter & formatter,
			XML_Char const * baseUri)
			: formatter(&formatter),
			propsWriter(),
			accum(new std::basic_ostringstream<XML_Char>()),
			trackListEmpty(true),
			headerWritten(false),
			footerWritten(false),
			version(-1),
			baseUri(Toolbox::newAndCopy(baseUri)) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfWriterPrivate(XspfWriterPrivate const & source)
			: formatter(source.formatter),
			propsWriter(source.propsWriter),
			accum(new std::basic_ostringstream<XML_Char>()),
			trackListEmpty(source.trackListEmpty),
			headerWritten(source.headerWritten),
			footerWritten(source.footerWritten),
			version(source.version),
			baseUri(Toolbox::newAndCopy(source.baseUri)) {
		this->accum->str(source.accum->str());
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfWriterPrivate & operator=(XspfWriterPrivate const & source) {
		if (this != &source) {
			this->formatter = source.formatter;
			this->propsWriter = source.propsWriter;
			this->accum->str(source.accum->str());
			this->trackListEmpty = source.trackListEmpty;
			this->headerWritten = source.headerWritten;
			this->footerWritten = source.footerWritten;
			this->version = source.version;
			Toolbox::deleteNewAndCopy(&this->baseUri, source.baseUri);
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfWriterPrivate() {
		delete this->accum;
		delete [] this->baseUri;
	}

};

/// @endcond


XspfWriter::XspfWriter(XspfXmlFormatter & formatter,
		XML_Char const * baseUri, bool embedBase)
		: d(new XspfWriterPrivate(formatter, baseUri)) {
	// Init formatter
	formatter.setOutput(*(this->d->accum));
	this->d->propsWriter.init(*(this->d->formatter), baseUri, embedBase);
}


/*static*/ XspfWriter *
XspfWriter::makeWriter(XspfXmlFormatter & formatter,
		XML_Char const * baseUri, bool embedBase, int * errorCode) {
	// Check base URI
	if ((baseUri != NULL) && !Toolbox::isAbsoluteUri(baseUri)) {
		if (errorCode != NULL) {
			*errorCode = XSPF_WRITER_ERROR_BASE_URI_USELESS;
		}
		return NULL;
	}

	// Create
	if (errorCode != NULL) {
		*errorCode = XSPF_WRITER_SUCCESS;
	}
	return new XspfWriter(formatter, baseUri, embedBase);
}


XspfWriter::XspfWriter(XspfWriter const & source)
		: d(new XspfWriterPrivate(*(source.d))) {

}


XspfWriter & XspfWriter::operator=(XspfWriter const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfWriter::~XspfWriter() {
	delete this->d;
}


bool
XspfWriter::registerNamespace(XML_Char const * uri,
		XML_Char const * prefixSuggestion) {
	// Too late?
	if (this->d->headerWritten) {
		return false;
	}

	return this->d->propsWriter.registerNamespace(uri, prefixSuggestion);
}


bool
XspfWriter::addTrack(XspfTrack const * track) {
	if (track == NULL) {
		return false;
	}

	// Playlist already finalized?
	if (this->d->footerWritten) {
		return false;
	}

	XspfTrackWriter trackWriter;
	trackWriter.setTrack(track);

	// First track ever?
	if (!this->d->headerWritten) {
		this->d->propsWriter.setProps(NULL);
		this->d->version = 1;
		this->d->propsWriter.writeStartPlaylist();
		this->d->propsWriter.writeStartTracklist(false);
		this->d->headerWritten = true;
	}

	trackWriter.init(*(this->d->formatter), this->d->version,
			this->d->baseUri);
	trackWriter.write();
	this->d->trackListEmpty = false;

	return true;
}


bool
XspfWriter::addTrack(XspfTrack const & track) {
	return addTrack(&track);
}


bool
XspfWriter::setProps(XspfProps const * props) {
	if (this->d->headerWritten) {
		return false;
	}

	this->d->propsWriter.setProps(props);
	this->d->version = (props == NULL)
		? 1
		: props->getVersion();
	this->d->propsWriter.writeStartPlaylist();
	this->d->propsWriter.writeStartTracklist(false);
	this->d->headerWritten = true;
	return true;
}


bool
XspfWriter::setProps(XspfProps const & props) {
	return setProps(&props);
}


void
XspfWriter::onBeforeWrite() {
	// Header
	if (!this->d->headerWritten) {
		this->d->propsWriter.writeStartPlaylist();
		this->d->propsWriter.writeStartTracklist(true);
		this->d->headerWritten = true;
	}

	// Footer
	if (!this->d->footerWritten) {
		this->d->propsWriter.writeEndTracklist();
		this->d->propsWriter.writeEndPlaylist();
		this->d->footerWritten = true;
	}
}


int
XspfWriter::writeFile(XML_Char const * filename) {
	// Open file
	FILE * const file = ::PORT_FOPEN(filename, _PT("wb"));
	if (file == NULL) {
		// TODO
		return XSPF_WRITER_ERROR_OPENING;
	}

	onBeforeWrite();

	// Get final input
	std::basic_string<XML_Char> final = this->d->accum->str();
	XML_Char const * const rawFinal = final.c_str();
	int const rawFinalLen = static_cast<int>(::PORT_STRLEN(rawFinal));

	// UTF-8 conversion on Unicode Windows
#if (defined(UNICODE) && (defined(__WIN32__) || defined(WIN32)))
	char * rawFinalUtf8 = new char[rawFinalLen * 4];
	int const rawFinalUtf8Len = ::WideCharToMultiByte(CP_UTF8, 0,
			rawFinal, rawFinalLen, rawFinalUtf8, rawFinalLen * 4,
			NULL, NULL);
	::fwrite(rawFinalUtf8, 1, rawFinalUtf8Len, file);
	delete [] rawFinalUtf8;
#else
	::fwrite(rawFinal, sizeof(XML_Char), rawFinalLen, file);
#endif
	::fclose(file);

	return XSPF_WRITER_SUCCESS;
}


int
XspfWriter::writeMemory(char * & memory, int & numBytes) {
	onBeforeWrite();

	// Get final input
	std::basic_string<XML_Char> final = this->d->accum->str();
	XML_Char const * const rawFinal = final.c_str();
	int const rawFinalLen = static_cast<int>(::PORT_STRLEN(rawFinal));

	// UTF-8 conversion on Unicode Windows
#if (defined(UNICODE) && (defined(__WIN32__) || defined(WIN32)))
	memory = new char[rawFinalLen * 4];
	numBytes = ::WideCharToMultiByte(CP_UTF8, 0, rawFinal, rawFinalLen, memory, rawFinalLen * 4, NULL, NULL);
#else
	memory = new char[rawFinalLen + 1];
	memcpy(memory, rawFinal, rawFinalLen);
	memory[rawFinalLen] = '\0';
	numBytes = rawFinalLen;
#endif
	return XSPF_WRITER_SUCCESS;
}


void
XspfWriter::reset(XspfXmlFormatter & formatter,
		XML_Char const * baseUri, bool embedBase) {
	// Init formatter and content writer
	this->d->formatter = &formatter;
	formatter.setOutput(*this->d->accum);
	this->d->propsWriter.init(*this->d->formatter, baseUri, embedBase);

	this->d->trackListEmpty = true;
	this->d->headerWritten = false;
	this->d->footerWritten = false;

	// Clear buffer
	delete this->d->accum;
	this->d->accum = new std::basic_ostringstream<XML_Char>;
}


}
