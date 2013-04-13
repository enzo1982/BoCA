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
 * @file XspfReader.cpp
 * Implementation of XspfReader.
 */

#include <xspf/XspfReader.h>
#include <xspf/XspfProps.h>
#include <xspf/XspfDateTime.h>
#include <xspf/XspfTrack.h>
#include <xspf/XspfToolbox.h>
#include <xspf/XspfStack.h>
#include "XspfSkipExtensionReader.h"
#include <xspf/XspfExtensionReaderFactory.h>
#include <xspf/XspfChunkCallback.h>
#include "XspfStrictReaderCallback.h"
#include <uriparser/Uri.h>
#include <cassert>
#include <cstdio>
#include <algorithm>
#include <cstdlib> // atoi
#include <cstring> // size_t
#include <cstddef> // int

#if (URI_VER_MINOR < 7) || ((URI_VER_MINOR == 7) && (URI_VER_RELEASE < 2))
# error uriparser 0.7.2 or later is required
#endif


namespace {

int const XSPF_FALLBACK_VERSION = 1;

struct EntityInfo {
	int valueLen;
	int lookupSum;
	int lookupDepth;

	EntityInfo(int valueLen, int lookupSum, int lookupDepth)
			: valueLen(valueLen), lookupSum(lookupSum),
			lookupDepth(lookupDepth) { }
};

typedef std::basic_string<XML_Char> StringType;
typedef std::map<StringType, EntityInfo> MapType;
typedef std::pair<StringType, EntityInfo> PairType;

int const MAX_LENGTH_PER_ENTITY_VALUE        = 100000;
int const MAX_LOOKUP_SUM_PER_ENTITY_VALUE    = 10000;
int const MAX_LOOKUP_DEPTH_PER_ENTITY_VALUE  = 5;

XML_Char *
makeString(XML_Char const * first, XML_Char const * afterLast) {
	size_t const len = afterLast - first;
	XML_Char * dup = new XML_Char[len + 1];
	::PORT_STRNCPY(dup, first, len);
	dup[len] = '\0';
	return dup;
}

XML_Char *
nextEntityRefMalloc(XML_Char const * start,
		XML_Char const * & atAmpersand,
		XML_Char const * & afterSemiColon) {
	XML_Char const * walker = start;
	while (true) {
		switch (walker[0]) {
		case '\0':
			// No complete entity found
			atAmpersand = start;
			afterSemiColon = walker;
			return NULL;

		case '&':
			// Entity start found
			atAmpersand = walker;
			break;

		case ';':
			// Entity stop found
			if (atAmpersand != NULL) {
				afterSemiColon = walker + 1;
				return makeString(atAmpersand + 1, walker);
			}
			break;
		}
		walker++;
	}
}

} // anon namespace

namespace Xspf {



/// @cond DOXYGEN_NON_API

/**
 * D object for XspfReader.
 */
class XspfReaderPrivate {

	friend class XspfReader;
	friend class XspfExtensionReader;

private:
	XspfStack<unsigned int> elementStack; ///< Element stack
	XspfStack<std::basic_string<XML_Char> > baseUriStack; ///< Element stack
	XspfProps * props; ///< Playlist properties slot
	XspfTrack * track; ///< Track slot
	int version; ///< XSPF version

	XML_Parser parser; ///< Expat parser handle
	XspfReaderCallback * callback; ///< Reader callback
	bool ownCallback;
	std::basic_string<XML_Char> accum; ///< Element content accumulator
	std::basic_string<XML_Char> lastRelValue; ///< Value of the previous "rel" attribute

	XspfExtensionReader * extensionReader; ///< Current extension reader
	XspfExtensionReaderFactory * extensionReaderFactory; ///< Extension reader factory

	int errorCode; ///< Error code

	bool insideExtension; ///< Flag wether the parser is inside an extension element
	bool skip; ///< Flag indicating skip-reading to ignore certain kinds of errors
	unsigned int skipStopLevel; ///< Level on which skipping should stop

	bool firstPlaylistAnnotation; ///< First annotation in playlist flag
	bool firstPlaylistAttribution; ///< First attributation in playlist flag
	bool firstPlaylistCreator; ///< First creator in playlist flag
	bool firstPlaylistDate; ///< First date in playlist flag
	bool firstPlaylistIdentifier; ///< First identifier in playlist flag
	bool firstPlaylistImage; ///< First image in playlist flag
	bool firstPlaylistInfo; ///< First info in playlist flag
	bool firstPlaylistLicense; ///< First license in playlist flag
	bool firstPlaylistLocation; ///< First location in playlist flag
	bool firstPlaylistTitle; ///< First title in playlist flag
	bool firstPlaylistTrackList; ///< First trackList in playlist flag

	bool firstTrackTitle; ///< First title in track flag
	bool firstTrackCreator; ///< First creator in track flag
	bool firstTrackAnnotation; ///< First annotation in track flag
	bool firstTrackInfo; ///< First info in track flag
	bool firstTrackImage; ///< First image in track flag
	bool firstTrackAlbum; ///< First album in track flag
	bool firstTrackTrackNum; ///< First trackNum in track flag
	bool firstTrackDuration; ///< First duration in track flag

	bool firstTrack; ///< First track flag

	MapType entityNameToValueLen; // TODO

	int maxLengthPerEntity; // TODO
	int maxTotalLookupsPerEntity; // TODO
	int maxLookupDepthPerEntity; // TODO
	bool limitLengthPerEntityValue; // TODO
	bool limitLookupSumPerEntityValue; // TODO
	bool limitLookupDepthPerEntityValue; // TODO

	/**
	 * Creates a new D object.
	 *
	 * @param handlerFactory  Factory used to create handlers
	 */
	XspfReaderPrivate(XspfExtensionReaderFactory * handlerFactory)
			: elementStack(),
			props(NULL),
			track(NULL),
			version(-1),
			callback(NULL),
			ownCallback(false),
			extensionReader(NULL),
			extensionReaderFactory(handlerFactory),
			errorCode(XSPF_READER_SUCCESS),
			insideExtension(false),
			skip(false),
			skipStopLevel(0),
			firstPlaylistAnnotation(true),
			firstPlaylistAttribution(true),
			firstPlaylistCreator(true),
			firstPlaylistDate(true),
			firstPlaylistIdentifier(true),
			firstPlaylistImage(true),
			firstPlaylistInfo(true),
			firstPlaylistLicense(true),
			firstPlaylistLocation(true),
			firstPlaylistTitle(true),
			firstPlaylistTrackList(true),
			firstTrackTitle(true),
			firstTrackCreator(true),
			firstTrackAnnotation(true),
			firstTrackInfo(true),
			firstTrackImage(true),
			firstTrackAlbum(true),
			firstTrackTrackNum(true),
			firstTrackDuration(true),
			firstTrack(true),
			entityNameToValueLen(),
			maxLengthPerEntity(MAX_LENGTH_PER_ENTITY_VALUE),
			maxTotalLookupsPerEntity(MAX_LOOKUP_SUM_PER_ENTITY_VALUE),
			maxLookupDepthPerEntity(MAX_LOOKUP_DEPTH_PER_ENTITY_VALUE),
			limitLengthPerEntityValue(false),
			limitLookupSumPerEntityValue(false),
			limitLookupDepthPerEntityValue(false) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfReaderPrivate(XspfReaderPrivate const & source)
			: elementStack(source.elementStack),
			props((source.props != NULL)
				? new XspfProps(*(source.props))
				: NULL),
			track((source.track != NULL)
				? new XspfTrack(*(source.track))
				: NULL),
			version(source.version),
			callback(source.ownCallback
				? new XspfStrictReaderCallback
				: source.callback),
			ownCallback(source.ownCallback),
			extensionReader((source.extensionReader != NULL)
				? source.extensionReader->createBrother()
				: NULL),
			extensionReaderFactory(source.extensionReaderFactory),
			errorCode(source.errorCode),
			insideExtension(source.insideExtension),
			skip(source.skip),
			skipStopLevel(source.skipStopLevel),
			firstPlaylistAnnotation(source.firstPlaylistAnnotation),
			firstPlaylistAttribution(source.firstPlaylistAttribution),
			firstPlaylistCreator(source.firstPlaylistCreator),
			firstPlaylistDate(source.firstPlaylistDate),
			firstPlaylistIdentifier(source.firstPlaylistIdentifier),
			firstPlaylistImage(source.firstPlaylistImage),
			firstPlaylistInfo(source.firstPlaylistInfo),
			firstPlaylistLicense(source.firstPlaylistLicense),
			firstPlaylistLocation(source.firstPlaylistLocation),
			firstPlaylistTitle(source.firstPlaylistTitle),
			firstPlaylistTrackList(source.firstPlaylistTrackList),
			firstTrackTitle(source.firstTrackTitle),
			firstTrackCreator(source.firstTrackCreator),
			firstTrackAnnotation(source.firstTrackAnnotation),
			firstTrackInfo(source.firstTrackInfo),
			firstTrackImage(source.firstTrackImage),
			firstTrackAlbum(source.firstTrackAlbum),
			firstTrackTrackNum(source.firstTrackTrackNum),
			firstTrackDuration(source.firstTrackDuration),
			firstTrack(source.firstTrack),
			entityNameToValueLen(source.entityNameToValueLen),
			maxLengthPerEntity(source.maxLengthPerEntity),
			maxTotalLookupsPerEntity(source.maxTotalLookupsPerEntity),
			maxLookupDepthPerEntity(source.maxLookupDepthPerEntity),
			limitLengthPerEntityValue(source.limitLengthPerEntityValue),
			limitLookupSumPerEntityValue(source.limitLookupSumPerEntityValue),
			limitLookupDepthPerEntityValue(source.limitLookupDepthPerEntityValue) {

	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfReaderPrivate & operator=(XspfReaderPrivate const & source) {
		if (this != &source) {
			// stack
			this->elementStack = source.elementStack;

			// props
			if (this->props != NULL) {
				delete this->props;
			}
			this->props = (source.props != NULL)
				? new XspfProps(*(source.props))
				: NULL;

			// props
			if (this->track != NULL) {
				delete this->track;
			}
			this->track = (source.track != NULL)
				? new XspfTrack(*(source.track))
				: NULL;

			this->version = source.version;
			this->callback = source.ownCallback
					? new XspfStrictReaderCallback
					: source.callback;
			this->ownCallback = source.ownCallback;

			// extension reader
			if (this->extensionReader != NULL) {
				delete this->track;
			}
			this->extensionReader = (source.extensionReader != NULL)
				? source.extensionReader->createBrother()
				: NULL;

			this->extensionReaderFactory = source.extensionReaderFactory;
			this->errorCode = source.errorCode;
			this->insideExtension = source.insideExtension;
			this->skip = source.skip;
			this->skipStopLevel = source.skipStopLevel;
			this->firstPlaylistAnnotation = source.firstPlaylistAnnotation;
			this->firstPlaylistAttribution = source.firstPlaylistAttribution;
			this->firstPlaylistCreator = source.firstPlaylistCreator;
			this->firstPlaylistDate = source.firstPlaylistDate;
			this->firstPlaylistIdentifier = source.firstPlaylistIdentifier;
			this->firstPlaylistImage = source.firstPlaylistImage;
			this->firstPlaylistInfo = source.firstPlaylistInfo;
			this->firstPlaylistLicense = source.firstPlaylistLicense;
			this->firstPlaylistLocation = source.firstPlaylistLocation;
			this->firstPlaylistTitle = source.firstPlaylistTitle;
			this->firstPlaylistTrackList = source.firstPlaylistTrackList;
			this->firstTrackTitle = source.firstTrackTitle;
			this->firstTrackCreator = source.firstTrackCreator;
			this->firstTrackAnnotation = source.firstTrackAnnotation;
			this->firstTrackInfo = source.firstTrackInfo;
			this->firstTrackImage = source.firstTrackImage;
			this->firstTrackAlbum = source.firstTrackAlbum;
			this->firstTrackTrackNum = source.firstTrackTrackNum;
			this->firstTrackDuration = source.firstTrackDuration;
			this->firstTrack = source.firstTrack;
			this->entityNameToValueLen = source.entityNameToValueLen;
			this->maxLengthPerEntity = source.maxLengthPerEntity;
			this->maxTotalLookupsPerEntity = source.maxTotalLookupsPerEntity;
			this->maxLookupDepthPerEntity = source.maxLookupDepthPerEntity;
			this->limitLengthPerEntityValue = source.limitLengthPerEntityValue;
			this->limitLookupSumPerEntityValue = source.limitLookupSumPerEntityValue;
			this->limitLookupDepthPerEntityValue = source.limitLookupDepthPerEntityValue;
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfReaderPrivate() {
		if (this->props != NULL) {
			delete this->props;
		}
		if (this->track != NULL) {
			delete this->track;
		}
		if (this->extensionReader != NULL) {
			delete this->extensionReader;
		}
		if (this->ownCallback) {
			delete this->callback;
		}
	}

};

/// @endcond


XspfReader::XspfReader(XspfExtensionReaderFactory * handlerFactory)
		: d(new XspfReaderPrivate(handlerFactory)) { }


void XspfReader::makeReusable() {
	// Reset everything but the error state
	this->d->elementStack.clear();
	this->d->baseUriStack.clear();
	if (this->d->props != NULL) {
		delete this->d->props;
		this->d->props = NULL;
	}
	if (this->d->track != NULL) {
		delete this->d->track;
		this->d->track = NULL;
	}
	if (this->d->ownCallback) {
		delete this->d->callback;
		this->d->ownCallback = false;
	}
	this->d->callback = NULL;
	this->d->accum.clear();
	this->d->lastRelValue.clear();

	this->d->firstPlaylistAnnotation = true;
	this->d->firstPlaylistAttribution = true;
	this->d->firstPlaylistCreator = true;
	this->d->firstPlaylistDate = true;
	this->d->firstPlaylistIdentifier = true;
	this->d->firstPlaylistImage = true;
	this->d->firstPlaylistInfo = true;
	this->d->firstPlaylistLicense = true;
	this->d->firstPlaylistLocation = true;
	this->d->firstPlaylistTitle = true;
	this->d->firstPlaylistTrackList = true;

	this->d->firstTrackTitle = true;
	this->d->firstTrackCreator = true;
	this->d->firstTrackAnnotation = true;
	this->d->firstTrackInfo = true;
	this->d->firstTrackImage = true;
	this->d->firstTrackAlbum = true;
	this->d->firstTrackTrackNum = true;
	this->d->firstTrackDuration = true;

	this->d->firstTrack = true;

	this->d->insideExtension = false;
	this->d->skip = false;
	this->d->skipStopLevel = 0;
	this->d->version = -1;

	if (this->d->extensionReader != NULL) {
		delete this->d->extensionReader;
		this->d->extensionReader = NULL;
	}

	this->d->entityNameToValueLen.clear();
}


XspfReader::XspfReader(XspfReader const & source)
		: d(new XspfReaderPrivate(*(source.d))) { }


XspfReader & XspfReader::operator=(XspfReader const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfReader::~XspfReader() {
	delete this->d;
}


bool
XspfReader::onBeforeParse(XspfReaderCallback * callback,
		XML_Char const * baseUri) {
	this->d->ownCallback = (callback == NULL);
	this->d->callback = (callback == NULL)
			? new XspfStrictReaderCallback
			: callback;

	// Init base URI stack
	if (!Toolbox::isAbsoluteUri(baseUri)) {
		handleFatalError(XSPF_READER_ERROR_BASE_URI_USELESS,
				_PT("Base URI is not a valid absolute URI."));
		return false;
	}
	std::basic_string<XML_Char> baseUriString(baseUri);
	this->d->baseUriStack.push(baseUriString);

	clearError();

	// Create parser
	this->d->parser = ::XML_ParserCreateNS(NULL, XSPF_NS_SEP_CHAR);

	// Put class pointer into user data
	::XML_SetUserData(this->d->parser, this);

	// Register handlers
	::XML_SetElementHandler(this->d->parser, masterStart, masterEnd);
	::XML_SetCharacterDataHandler(this->d->parser, masterCharacters);
	::XML_SetEntityDeclHandler(this->d->parser, masterEntityDeclaration);
	return true;
}


void
XspfReader::onAfterParse() {
	::XML_ParserFree(this->d->parser);
	makeReusable();
}


/*static*/ bool
XspfReader::isXmlBase(XML_Char const * attributeKey) {
	return (!::PORT_STRNCMP(attributeKey, XML_NS_HOME,
			XML_NS_HOME_LEN) && !::PORT_STRCMP(attributeKey
			+ XML_NS_HOME_LEN + 1, _PT("base")));
}


bool
XspfReader::handleXmlBaseAttribute(XML_Char const * xmlBase) {
	// Check URI
	if (!Toolbox::isUri(xmlBase)) {
		if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_INVALID,
				XSPF_READER_TEXT_ZERO_WRONG_ATTRIBUTE_TYPE(
				_PT("xml:base"), _PT("URI")))) {
			return false;
		}
	}

	// Make absolute and push on URI stack
	XML_Char const * const resolveAgainst
				= this->d->baseUriStack.top().c_str();
	XML_Char * const resolvedUri = Toolbox::makeAbsoluteUri(
			xmlBase, resolveAgainst);
	this->d->baseUriStack.push(std::basic_string<XML_Char>(
			const_cast<XML_Char const *>(resolvedUri)));
	delete [] resolvedUri;
	return true;
}


XML_Char *
XspfReader::makeAbsoluteUri(XML_Char const * sourceUri) const {
	XML_Char const * const baseUri = this->d->baseUriStack.top().c_str();
	XML_Char * const res = Toolbox::makeAbsoluteUri(sourceUri, baseUri);
	return res;
}


void XspfReader::setExpatError() {
	XML_Error const expatCode = ::XML_GetErrorCode(this->d->parser);
	handleFatalError(XSPF_READER_ERROR_EXPAT + static_cast<int>(expatCode),
			XSPF_READER_TEXT_ONE_EXPAT_ERROR,
			::XML_ErrorString(expatCode));
}


int XspfReader::parseFile(XML_Char const * filename,
		XspfReaderCallback * callback, XML_Char const * baseUri) {
	// Init
	if (!onBeforeParse(callback, baseUri)) {
		return this->d->errorCode;
	}

	// Check filename
	if (filename == NULL) {
		handleFatalError(XSPF_READER_ERROR_NO_INPUT,
				XSPF_READER_TEXT_ZERO_FILENAME_NULL);
		return this->d->errorCode;
	}

	// Open file
	FILE * file = ::PORT_FOPEN(filename, _PT("r"));
	if (file == NULL) {
		handleFatalError(XSPF_READER_ERROR_NO_INPUT, XSPF_READER_TEXT_ONE_FILE_READING_ERROR, filename);
		return this->d->errorCode;
	}

	// Get filesize
	::fseek(file, 0, SEEK_END);
	long const filesize = ::ftell(file);
	::fseek(file, 0, SEEK_SET);

	// Read and parse file
	void * buffer;
	if (filesize > XSPF_MAX_BLOCK_SIZE) {
		// In several blocks
		long sizeLeft = filesize;
		while (sizeLeft > 0) {
			long const blockSize = std::min<long>(sizeLeft, XSPF_MAX_BLOCK_SIZE);
			buffer = ::XML_GetBuffer(this->d->parser, blockSize);
			::fread(buffer, 1, blockSize, file);
			sizeLeft -= blockSize;
			if (::XML_ParseBuffer(this->d->parser, blockSize, sizeLeft == 0)
					== XML_STATUS_ERROR) {
				if (this->d->errorCode == XSPF_READER_SUCCESS) {
					setExpatError();
				}
				break;
			}
		}
		::fclose(file);
	} else {
		// One single go
		buffer = ::XML_GetBuffer(this->d->parser, filesize);
		::fread(buffer, 1, filesize, file);
		::fclose(file);

		if (::XML_ParseBuffer(this->d->parser, filesize, 1)
				== XML_STATUS_ERROR) {
			if (this->d->errorCode == XSPF_READER_SUCCESS) {
				setExpatError();
			}
		}
	}

	notifySuccess();

	// Cleanup
	onAfterParse();

	return this->d->errorCode;
}


int XspfReader::parseMemory(char const * memory, int numBytes,
		XspfReaderCallback * callback, XML_Char const * baseUri) {
	// Init
	if (!onBeforeParse(callback, baseUri)) {
		return this->d->errorCode;
	}

	// Parse
	if (::XML_Parse(this->d->parser, memory, numBytes, 1)
			== XML_STATUS_ERROR) {
		if (this->d->errorCode == XSPF_READER_SUCCESS) {
			setExpatError();
		}
	}

	notifySuccess();

	// Cleanup
	onAfterParse();

	return this->d->errorCode;
}


int XspfReader::parseChunks(XspfChunkCallback * inputCallback,
		XspfReaderCallback * dataCallback, XML_Char const * baseUri) {
	// Init
	if (!onBeforeParse(dataCallback, baseUri)) {
		return this->d->errorCode;
	}

	// Parse chunks in a loop
	for (;;) {
		// Ask callback for buffer size
		int const bufferByteSize = inputCallback->getMinimumBufferByteSize();
		void * buffer = NULL; // init not needed

		// Create and fill buffer
		int bytesToParse = 0;
		if (bufferByteSize > 0) {
			buffer = ::XML_GetBuffer(this->d->parser, bufferByteSize);
			bytesToParse = inputCallback->fillBuffer(buffer);
		}

		// Parse chunk
		if (::XML_ParseBuffer(this->d->parser, bytesToParse,
				bytesToParse == 0) == XML_STATUS_ERROR) {
			// Error
			if (this->d->errorCode == XSPF_READER_SUCCESS) {
				setExpatError();
			}
			break;
		} else {
			// Fine, continue?
			if (bytesToParse == 0) {
				break;
			}
		}
	}
	inputCallback->notifyStop();
	notifySuccess(); // .. on dataCallback

	// Cleanup
	onAfterParse();

	return this->d->errorCode;
}


bool
XspfReader::checkAndSkipNamespace(XML_Char const * fullName,
		XML_Char const * & localName) {
	if (::PORT_STRNCMP(fullName, XSPF_NS_HOME, XSPF_NS_HOME_LEN)) {
		if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
				XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
				fullName)) {
			return false;
		}

		// Catch <xspf:tag> as <tag> instead
		localName = fullName;
		while ((localName[0] != _PT('\0'))
				&& (localName[0] != XSPF_NS_SEP_CHAR)) {
			localName++;
		}
		if (localName[0] == _PT('\0')) {
			// No namespace -> reset
			localName = fullName;
		} else {
			// Namespace -> jump to local name
			localName++;
		}
	} else {
		localName = fullName + XSPF_NS_HOME_LEN + 1;
	}
	return true;
}


bool
XspfReader::handleStartOne(XML_Char const * fullName, XML_Char const ** atts) {
	// Check and skip namespace
	XML_Char const * localName;
	if (!checkAndSkipNamespace(fullName, localName)) {
		return false;
	}

	// Check root name
	if (::PORT_STRCMP(localName, _PT("playlist"))) {
		if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
				XSPF_READER_TEXT_ONE_WRONG_ROOT_NAME,
				fullName)) {
			return false;
		}
	}

	this->d->props = new XspfProps();
	if (!handlePlaylistAttribs(atts)) {
		return false;
	}

	this->d->elementStack.push(TAG_PLAYLIST);
	this->d->props->setVersion(this->d->version);
	return true;
}


bool
XspfReader::handleStartTwo(XML_Char const * fullName,
		XML_Char const ** atts) {
	// Check and skip namespace
	XML_Char const * localName;
	if (!checkAndSkipNamespace(fullName, localName)) {
		return false;
	}

	switch (localName[0]) {
	case _PT('a'):
		switch (localName[1]) {
		case _PT('n'):
			if (::PORT_STRCMP(localName + 2, _PT("notation"))) {
				break;
			}

			if (this->d->firstPlaylistAnnotation) {
				this->d->firstPlaylistAnnotation = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("annotation")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_ANNOTATION);
				return true;
			}
			break;

		case _PT('t'):
			if (::PORT_STRCMP(localName + 2, _PT("tribution"))) {
				break;
			}

			if (this->d->firstPlaylistAttribution) {
				this->d->firstPlaylistAttribution = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("attribution")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_ATTRIBUTION);
				return true;
			}
			break;

		}
		break;

	case _PT('c'):
		if (::PORT_STRCMP(localName + 1, _PT("reator"))) {
			break;
		}

		if (this->d->firstPlaylistCreator) {
			this->d->firstPlaylistCreator = false;
		} else {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
					XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
					XSPF_NS_HOME, _PT("creator")))) {
				return false;
			}
		}

		if (!handleNoAttribsExceptXmlBase(atts)) {
			return false;
		} else {
			this->d->elementStack.push(TAG_PLAYLIST_CREATOR);
			return true;
		}
		break;

	case _PT('d'):
		if (::PORT_STRCMP(localName + 1, _PT("ate"))) {
			break;
		}

		if (this->d->firstPlaylistDate) {
			this->d->firstPlaylistDate = false;
		} else {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
					XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
					XSPF_NS_HOME, _PT("date")))) {
				return false;
			}
		}

		if (!handleNoAttribsExceptXmlBase(atts)) {
			return false;
		} else {
			this->d->elementStack.push(TAG_PLAYLIST_DATE);
			return true;
		}
		break;

	case _PT('e'):
		if (::PORT_STRCMP(localName + 1, _PT("xtension"))) {
			break;
		}

		// Tag only allowed in v1
		if (this->d->version == 0) {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN_VERSION_ZERO,
					fullName)) {
				return false;
			}
		}

		XML_Char const * applicationUri;
		if (handleExtensionAttribs(atts, applicationUri)) {
			if (applicationUri == NULL) {
				this->d->elementStack.push(TAG_PLAYLIST_EXTENSION);
				skipFromHere();
				return true;
			} else {
				this->d->insideExtension = true;

				// Create suitable handler
				if (this->d->extensionReaderFactory != NULL) {
					this->d->extensionReader = this->d->extensionReaderFactory
							->newPlaylistExtensionReader(applicationUri, this);
				}
				if (this->d->extensionReader == NULL) {
					this->d->extensionReader = new XspfSkipExtensionReader(this);
				}

				return this->d->extensionReader->handleExtensionStart(fullName, atts);
			}
		} else {
			return false;
		}
		break;

	case _PT('i'):
		switch (localName[1]) {
		case _PT('d'):
			if (::PORT_STRCMP(localName + 2, _PT("entifier"))) {
				break;
			}

			if (this->d->firstPlaylistIdentifier) {
				this->d->firstPlaylistIdentifier = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("identifier")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_IDENTIFIER);
				return true;
			}
			break;

		case _PT('m'):
			if (::PORT_STRCMP(localName + 2, _PT("age"))) {
				break;
			}

			if (this->d->firstPlaylistImage) {
				this->d->firstPlaylistImage = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("image")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_IMAGE);
				return true;
			}
			break;

		case _PT('n'):
			if (::PORT_STRCMP(localName + 2, _PT("fo"))) {
				break;
			}

			if (this->d->firstPlaylistInfo) {
				this->d->firstPlaylistInfo = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("info")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_INFO);
				return true;
			}
			break;

		}
		break;

	case _PT('l'):
		switch (localName[1]) {
		case _PT('i'):
			switch (localName[2]) {
			case _PT('c'):
				if (::PORT_STRCMP(localName + 3, _PT("ense"))) {
					break;
				}

				if (this->d->firstPlaylistLicense) {
					this->d->firstPlaylistLicense = false;
				} else {
					if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
							XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
							XSPF_NS_HOME, _PT("license")))) {
						return false;
					}
				}

				if (!handleNoAttribsExceptXmlBase(atts)) {
					return false;
				} else {
					this->d->elementStack.push(TAG_PLAYLIST_LICENSE);
					return true;
				}
				break;

			case _PT('n'):
				if (::PORT_STRCMP(localName + 3, _PT("k"))) {
					break;
				}

				{
					XML_Char const * rel = NULL; // No init needed
					if (handleMetaLinkAttribs(atts, rel)) {
						this->d->elementStack.push(TAG_PLAYLIST_LINK);
						if (rel != NULL) {
							this->d->lastRelValue.assign(atts[1]);
						} else {
							skipFromHere();
						}
						return true;
					} else {
						return false;
					}
				}
				break;

			}
			break;

		case _PT('o'):
			if (::PORT_STRCMP(localName + 2, _PT("cation"))) {
				break;
			}

			if (this->d->firstPlaylistLocation) {
				this->d->firstPlaylistLocation = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("location")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_LOCATION);
				return true;
			}
			break;

		}
		break;

	case _PT('m'):
		if (::PORT_STRCMP(localName + 1, _PT("eta"))) {
			break;
		}

		{
			XML_Char const * rel = NULL; // No init needed
			if (handleMetaLinkAttribs(atts, rel)) {
				this->d->elementStack.push(TAG_PLAYLIST_META);
				if (rel != NULL) {
					this->d->lastRelValue.assign(atts[1]);
				} else {
					skipFromHere();
				}
				return true;
			} else {
				return false;
			}
		}
		break;

	case _PT('t'):
		switch (localName[1]) {
		case _PT('i'):
			if (::PORT_STRCMP(localName + 2, _PT("tle"))) {
				break;
			}

			if (this->d->firstPlaylistTitle) {
				this->d->firstPlaylistTitle = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("title")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TITLE);
				return true;
			}
			break;

		case _PT('r'):
			if (::PORT_STRCMP(localName + 2, _PT("ackList"))) {
				break;
			}

			if (this->d->firstPlaylistTrackList) {
				this->d->firstPlaylistTrackList = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("trackList")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST);
				return true;
			}
			break;

		}
		break;

	}

	if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
			XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
			fullName)) {
		return false;
	}

	this->d->elementStack.push(TAG_UNKNOWN);
	skipFromHere();
	return true;
}


bool XspfReader::handleStartThree(XML_Char const * fullName, XML_Char const ** atts) {
	// Check and skip namespace
	XML_Char const * localName;
	if (!checkAndSkipNamespace(fullName, localName)) {
		return false;
	}

	switch (this->d->elementStack.top()) {
	case TAG_PLAYLIST_ATTRIBUTION:
		switch (localName[0]) {
		case _PT('i'):
			if (::PORT_STRCMP(localName + 1, _PT("dentifier"))) {
				break;
			}
			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			}
			this->d->elementStack.push(TAG_PLAYLIST_ATTRIBUTION_IDENTIFIER);
			return true;

		case _PT('l'):
			if (::PORT_STRCMP(localName + 1, _PT("ocation"))) {
				break;
			}
			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			}
			this->d->elementStack.push(TAG_PLAYLIST_ATTRIBUTION_LOCATION);
			return true;

		}
		break;

	case TAG_PLAYLIST_TRACKLIST:
		if (::PORT_STRCMP(localName, _PT("track"))) {
			break;
		}
		if (!handleNoAttribsExceptXmlBase(atts)) {
			return false;
		}
		this->d->firstTrack = false;
		this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK);
		this->d->track = new XspfTrack();
		return true;

	}

	if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
			XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
			fullName)) {
		return false;
	}

	this->d->elementStack.push(TAG_UNKNOWN);
	skipFromHere();
	return true;
}


bool XspfReader::handleStartFour(XML_Char const * fullName, XML_Char const ** atts) {
	if (this->d->elementStack.top() != TAG_PLAYLIST_TRACKLIST_TRACK) {
		return false;
	}

	// Check and skip namespace
	XML_Char const * localName;
	if (!checkAndSkipNamespace(fullName, localName)) {
		return false;
	}

	switch (localName[0]) {
	case _PT('a'):
		switch (localName[1]) {
		case _PT('l'):
			if (::PORT_STRCMP(localName + 2, _PT("bum"))) {
				break;
			}

			if (this->d->firstTrackAlbum) {
				this->d->firstTrackAlbum = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("album")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_ALBUM);
				return true;
			}
			break;

		case _PT('n'):
			if (::PORT_STRCMP(localName + 2, _PT("notation"))) {
				break;
			}

			if (this->d->firstTrackAnnotation) {
				this->d->firstTrackAnnotation = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("annotation")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_ANNOTATION);
				return true;
			}
			break;

		case _PT('r'):
			if (::PORT_STRCMP(localName + 2, _PT("tist"))) {
				break;
			}

			// Note: Element //playlist/trackList/track/artist
			// is not valid XSPF. This is a loose fallback.
			if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
					fullName)) {
				return false;
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_CREATOR);
				return true;
			}
			break;

		}
		break;

	case _PT('c'):
		if (::PORT_STRCMP(localName + 1, _PT("reator"))) {
			break;
		}

		if (this->d->firstTrackCreator) {
			this->d->firstTrackCreator = false;
		} else {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
					XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
					XSPF_NS_HOME, _PT("creator")))) {
				return false;
			}
		}

		if (!handleNoAttribsExceptXmlBase(atts)) {
			return false;
		} else {
			this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_CREATOR);
			return true;
		}
		break;

	case _PT('d'):
		if (::PORT_STRCMP(localName + 1, _PT("uration"))) {
			break;
		}

		if (this->d->firstTrackDuration) {
			this->d->firstTrackDuration = false;
		} else {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
					XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
					XSPF_NS_HOME, _PT("duration")))) {
				return false;
			}
		}

		if (!handleNoAttribsExceptXmlBase(atts)) {
			return false;
		} else {
			this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_DURATION);
			return true;
		}
		break;

	case _PT('e'):
		if (::PORT_STRCMP(localName + 1, _PT("xtension"))) {
			break;
		}

		// Tag only allowed in v1
		if (this->d->version == 0) {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN_VERSION_ZERO,
					fullName)) {
				return false;
			}
		}

		XML_Char const * applicationUri;
		if (handleExtensionAttribs(atts, applicationUri)) {
			if (applicationUri == NULL) {
				this->d->elementStack.push(
						TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION);
				skipFromHere();
				return true;
			} else {
				this->d->insideExtension = true;

				// Create suitable handler
				if (this->d->extensionReaderFactory != NULL) {
					this->d->extensionReader = this->d->extensionReaderFactory
							->newTrackExtensionReader(applicationUri, this);
				}
				if (this->d->extensionReader == NULL) {
					this->d->extensionReader = new XspfSkipExtensionReader(this);
				}

				return this->d->extensionReader->handleExtensionStart(fullName, atts);
			}
		} else {
			return false;
		}
		break;

	case _PT('i'):
		switch (localName[1]) {
		case _PT('d'):
			if (::PORT_STRCMP(localName + 2, _PT("entifier"))) {
				break;
			}
			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			}
			this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_IDENTIFIER);
			return true;

		case _PT('m'):
			if (::PORT_STRCMP(localName + 2, _PT("age"))) {
				break;
			}

			if (this->d->firstTrackImage) {
				this->d->firstTrackImage = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("image")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_IMAGE);
				return true;
			}
			break;

		case _PT('n'):
			if (::PORT_STRCMP(localName + 2, _PT("fo"))) {
				break;
			}

			if (this->d->firstTrackInfo) {
				this->d->firstTrackInfo = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("info")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_INFO);
				return true;
			}
			break;

		}
		break;

	case _PT('l'):
		switch (localName[1]) {
		case _PT('i'):
			if (::PORT_STRCMP(localName + 2, _PT("nk"))) {
				break;
			}

			{
				XML_Char const * rel = NULL; // No init needed
				if (handleMetaLinkAttribs(atts, rel)) {
					this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_LINK);
					if (rel != NULL) {
						this->d->lastRelValue.assign(atts[1]);
					} else {
						skipFromHere();
					}
					return true;
				} else {
					return false;
				}
			}
			break;

		case _PT('o'):
			if (::PORT_STRCMP(localName + 2, _PT("cation"))) {
				break;
			}
			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			}
			this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_LOCATION);
			return true;

		}
		break;

	case _PT('m'):
		if (::PORT_STRCMP(localName + 1, _PT("eta"))) {
			break;
		}

		{
			XML_Char const * rel = NULL; // No init needed
			if (handleMetaLinkAttribs(atts, rel)) {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_META);
				if (rel != NULL) {
					this->d->lastRelValue.assign(atts[1]);
				} else {
					skipFromHere();
				}
				return true;
			} else {
				return false;
			}
		}
		break;

	case _PT('t'):
		switch (localName[1]) {
		case _PT('i'):
			if (::PORT_STRCMP(localName + 2, _PT("tle"))) {
				break;
			}

			if (this->d->firstTrackTitle) {
				this->d->firstTrackTitle = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("title")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_TITLE);
				return true;
			}
			break;

		case _PT('r'):
			if (::PORT_STRCMP(localName + 2, _PT("ackNum"))) {
				break;
			}

			if (this->d->firstTrackTrackNum) {
				this->d->firstTrackTrackNum = false;
			} else {
				if (!handleError(XSPF_READER_ERROR_ELEMENT_TOOMANY,
						XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(
						XSPF_NS_HOME, _PT("trackNum")))) {
					return false;
				}
			}

			if (!handleNoAttribsExceptXmlBase(atts)) {
				return false;
			} else {
				this->d->elementStack.push(TAG_PLAYLIST_TRACKLIST_TRACK_TRACKNUM);
				return true;
			}
			break;

		}
		break;

	}

	if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
			XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
			fullName)) {
		return false;
	}

	this->d->elementStack.push(TAG_UNKNOWN);
	skipFromHere();
	return true;
}


void XspfReader::handleStart(XML_Char const * fullName, XML_Char const ** atts) {
	if (this->d->skip) {
		this->d->elementStack.push(TAG_UNKNOWN);
		return;
	}

	bool res = true;
	if (this->d->insideExtension) {
		res = this->d->extensionReader->handleExtensionStart(fullName, atts);
	} else {
		switch (this->d->elementStack.size() + 1) {
		case 1:
			res = handleStartOne(fullName, atts);
			break;

		case 2:
			res = handleStartTwo(fullName, atts);
			break;

		case 3:
			res = handleStartThree(fullName, atts);
			break;

		case 4:
			res = handleStartFour(fullName, atts);
			break;

		case 5:
			if (!handleError(XSPF_READER_ERROR_ELEMENT_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN,
					fullName)) {
				res = false;
			} else {
				this->d->elementStack.push(TAG_UNKNOWN);
				skipFromHere();
			}
			break;

		}
	}

	if (!res) {
		stop();
	}

	// Grow base URI stack as needed
	size_t const curBaseUriCount = this->d->baseUriStack.size();
	size_t const wantedBaseUriCount = this->d->elementStack.size();
	for (size_t i = curBaseUriCount; i < wantedBaseUriCount; i++) {
		this->d->baseUriStack.push(this->d->baseUriStack.top());
	}
}


bool XspfReader::handleEndOne(XML_Char const * /*fullName*/) {
	if (this->d->firstPlaylistTrackList) {
		if (!handleError(XSPF_READER_ERROR_ELEMENT_MISSING,
				XSPF_READER_TEXT_ZERO_ELEMENT_MISSING(
				XSPF_NS_HOME, _PT("trackList")))) {
			return false;
		}
	}

	// Call property callback
	assert(this->d->callback != NULL);
	// Note: setProps() deletes the props for us
	this->d->callback->setProps(this->d->props);
	this->d->props = NULL;
	return true;
}


bool XspfReader::handleEndTwo(XML_Char const * /*fullName*/) {
	const unsigned int stackTop = this->d->elementStack.top();

	// Collapse elements
	// NOTE: whitespace in the middle of <dateTime>,
	// <nonNegativeInteger>, and <anyURI> is illegal anyway
	// which is why we we only cut head and tail here
	switch (stackTop) {
	case TAG_PLAYLIST_INFO:
	case TAG_PLAYLIST_LOCATION:
	case TAG_PLAYLIST_IDENTIFIER:
	case TAG_PLAYLIST_IMAGE:
	case TAG_PLAYLIST_DATE:
	case TAG_PLAYLIST_LICENSE:
	case TAG_PLAYLIST_LINK:
	case TAG_PLAYLIST_META:
		Toolbox::trimString(this->d->accum);
		break;

	default:
		; // NOOP
	}

	XML_Char const * const finalAccum = this->d->accum.c_str();

	switch (stackTop) {
	case TAG_PLAYLIST_ANNOTATION:
		this->d->props->giveAnnotation(finalAccum, XspfData::COPY);
		break;
/*
	case TAG_PLAYLIST_ATTRIBUTION:
		break;
*/
	case TAG_PLAYLIST_CREATOR:
		this->d->props->giveCreator(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_DATE:
		{
			XspfDateTime * const dateTime = new XspfDateTime;
			if (!XspfDateTime::extractDateTime(finalAccum, dateTime)) {
				delete dateTime;
				if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
						XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
						XSPF_NS_HOME, _PT("date"), _PT("dateTime")))) {
					return false;
				}
			} else {
				this->d->props->giveDate(dateTime, XspfData::TRANSFER);
			}
		}
		break;
/*
	case TAG_PLAYLIST_EXTENSION:
		break;
*/
	case TAG_PLAYLIST_IDENTIFIER:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("identifier"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveIdentifier(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_IMAGE:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("image"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveImage(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_INFO:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("info"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveInfo(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_LICENSE:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("license"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveLicense(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_LINK:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("link"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveAppendLink(
					this->d->lastRelValue.c_str(), XspfData::COPY,
					makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_LOCATION:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("location"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveLocation(
					makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_META:
		this->d->props->giveAppendMeta(
				this->d->lastRelValue.c_str(), XspfData::COPY,
				finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TITLE:
		this->d->props->giveTitle(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST:
		// Check if empty for v0
		if ((this->d->version == 0) && (this->d->firstTrack)) {
			if (!handleError(XSPF_READER_ERROR_ELEMENT_MISSING,
					XSPF_READER_TEXT_ZERO_ELEMENT_MISSING_VERSION_ZERO(
					XSPF_NS_HOME, _PT("track")))) {
				return false;
			}
		}
		break;

	}

	this->d->accum.clear();
	return true;
}


bool XspfReader::handleEndThree(XML_Char const * /*fullName*/) {
	const unsigned int stackTop = this->d->elementStack.top();

	// Collapse elements
	// NOTE: whitespace in the middle of <dateTime>,
	// <nonNegativeInteger>, and <anyURI> is illegal anyway
	// which is why we we only cut head and tail here
	switch (stackTop) {
	case TAG_PLAYLIST_ATTRIBUTION_IDENTIFIER:
	case TAG_PLAYLIST_ATTRIBUTION_LOCATION:
		Toolbox::trimString(this->d->accum);
		break;

	default:
		; // NOOP
	}

	XML_Char const * const finalAccum = this->d->accum.c_str();

	switch (stackTop) {
	case TAG_PLAYLIST_ATTRIBUTION_IDENTIFIER:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("identifier"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveAppendAttributionIdentifier(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_ATTRIBUTION_LOCATION:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("location"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->props->giveAppendAttributionLocation(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK:
		// Call track callback
		assert(this->d->callback != NULL);
		// Note: addTrack() deletes the track for us
		this->d->callback->addTrack(this->d->track);
		this->d->track = NULL;

		this->d->firstTrackTitle = true;
		this->d->firstTrackCreator = true;
		this->d->firstTrackAnnotation = true;
		this->d->firstTrackInfo = true;
		this->d->firstTrackImage = true;
		this->d->firstTrackAlbum = true;
		this->d->firstTrackTrackNum = true;
		this->d->firstTrackDuration = true;
	}

	this->d->accum.clear();
	return true;
}


bool XspfReader::handleEndFour(XML_Char const * /*fullName*/) {
	const unsigned int stackTop = this->d->elementStack.top();

	// Collapse elements
	// NOTE: whitespace in the middle of <dateTime>,
	// <nonNegativeInteger>, and <anyURI> is illegal anyway
	// which is why we we only cut head and tail here
	switch (stackTop) {
	case TAG_PLAYLIST_TRACKLIST_TRACK_LOCATION:
	case TAG_PLAYLIST_TRACKLIST_TRACK_IDENTIFIER:
	case TAG_PLAYLIST_TRACKLIST_TRACK_INFO:
	case TAG_PLAYLIST_TRACKLIST_TRACK_IMAGE:
	case TAG_PLAYLIST_TRACKLIST_TRACK_TRACKNUM:
	case TAG_PLAYLIST_TRACKLIST_TRACK_DURATION:
	case TAG_PLAYLIST_TRACKLIST_TRACK_LINK:
	case TAG_PLAYLIST_TRACKLIST_TRACK_META:
		Toolbox::trimString(this->d->accum);
		break;

	default:
		; // NOOP
	}

	XML_Char const * const finalAccum = this->d->accum.c_str();

	switch (stackTop) {
	case TAG_PLAYLIST_TRACKLIST_TRACK_ALBUM:
		this->d->track->giveAlbum(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_ANNOTATION:
		this->d->track->giveAnnotation(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_CREATOR:
		this->d->track->giveCreator(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_DURATION:
		int duration;
		if (!Toolbox::extractInteger(finalAccum, 0, &duration)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("duration"),
					_PT("unsigned integer")))) {
				return false;
			}
		} else {
			this->d->track->setDuration(duration);
		}
		break;
/*
	case TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION:
		break;
*/
	case TAG_PLAYLIST_TRACKLIST_TRACK_IDENTIFIER:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("identifier"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->track->giveAppendIdentifier(
					makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_IMAGE:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("image"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->track->giveImage(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_INFO:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("info"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->track->giveInfo(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_LINK:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("link"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->track->giveAppendLink(
					this->d->lastRelValue.c_str(), XspfData::COPY,
					makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_LOCATION:
		if (!Toolbox::isUri(finalAccum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("location"), _PT("URI")))) {
				return false;
			}
		} else {
			this->d->track->giveAppendLocation(makeAbsoluteUri(finalAccum), XspfData::TRANSFER);
		}
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_META:
		this->d->track->giveAppendMeta(this->d->lastRelValue.c_str(), XspfData::COPY,
				finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_TITLE:
		this->d->track->giveTitle(finalAccum, XspfData::COPY);
		break;

	case TAG_PLAYLIST_TRACKLIST_TRACK_TRACKNUM:
		int trackNum;
		if (!Toolbox::extractInteger(finalAccum, 1, &trackNum)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(
					XSPF_NS_HOME, _PT("trackNum"),
					_PT("unsigned integer greater zero")))) {
				return false;
			}
		} else {
			this->d->track->setTrackNum(trackNum);
		}
		break;

	}

	this->d->accum.clear();
	return true;
}


void XspfReader::handleEnd(XML_Char const * fullName) {
	if (this->d->skip) {
		if (this->d->elementStack.size() == this->d->skipStopLevel) {
			this->d->skip = false;
		}
		this->d->elementStack.pop();
		return;
	}

	bool extensionLeft = false;
	int pushBackTag = TAG_UNKNOWN; // Init not needed but kills the warning...
	if (this->d->insideExtension) {
		switch (this->d->elementStack.size()) {
		case 2:
			if (this->d->elementStack.top() == TAG_PLAYLIST_EXTENSION) {
				pushBackTag = TAG_PLAYLIST_EXTENSION;
				extensionLeft = true;
			}
			break;

		case 4:
			if (this->d->elementStack.top() == TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION) {
				pushBackTag = TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION;
				extensionLeft = true;
			}
			break;

		}

		if (!this->d->extensionReader->handleExtensionEnd(fullName)) {
			stop();
			return;
		}

		if (extensionLeft) {
			this->d->insideExtension = false;

			// Add extension
			XspfExtension * const extension = this->d->extensionReader->wrap();
			if (extension != NULL) {
				XspfData * const target = (pushBackTag == TAG_PLAYLIST_EXTENSION)
						? static_cast<XspfData *>(this->d->props)
						: static_cast<XspfData *>(this->d->track);
				target->giveAppendExtension(extension, XspfData::TRANSFER);
			}

			// Destroy extension reader
			delete this->d->extensionReader;
			this->d->extensionReader = NULL;

			this->d->elementStack.push(pushBackTag);
		} else {
			return;
		}
	}

	bool res = false;
	switch (this->d->elementStack.size()) {
	case 1:
		res = handleEndOne(fullName);
		break;

	case 2:
		res = handleEndTwo(fullName);
		break;

	case 3:
		res = handleEndThree(fullName);
		break;

	case 4:
		res = handleEndFour(fullName);
		break;

	}

	if (!res) {
		stop();
		return;
	}

	// Shrink base URI stack while always keeping the external
	// base URI at the bottom of the stack
	size_t const curBaseUriCount = this->d->baseUriStack.size();
	size_t const wantedBaseUriCount = this->d->elementStack.size();
	for (size_t i = curBaseUriCount; i > wantedBaseUriCount; i--) {
		this->d->baseUriStack.pop();
	}

	// Prevent popping twice
//	if (!extensionLeft) {
		this->d->elementStack.pop();
//	}

}


void XspfReader::handleCharacters(XML_Char const * s, int len) {
	if (this->d->skip) {
		return;
	}

	if (this->d->insideExtension) {
		if (!this->d->extensionReader->handleExtensionCharacters(s, len)) {
			stop();
		}
		return;
	}

	switch (this->d->elementStack.size()) {
	case 1:
		// Must be all whitespace at root
		if (!Toolbox::isWhiteSpace(s, len)) {
			if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
					XSPF_READER_TEXT_ZERO_TEXT_FORBIDDEN(
					XSPF_NS_HOME, _PT("playlist")))) {
				stop();
				return;
			}
		}
		break;

	case 2:
		switch (this->d->elementStack.top()) {
		case TAG_PLAYLIST_TRACKLIST:
			// Must be all whitespace
			if (!Toolbox::isWhiteSpace(s, len)) {
				if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
						XSPF_READER_TEXT_ZERO_TEXT_FORBIDDEN(
						XSPF_NS_HOME, _PT("trackList")))) {
					stop();
					return;
				}
			}
			break;

		case TAG_PLAYLIST_ATTRIBUTION:
			// Must be all whitespace
			if (!Toolbox::isWhiteSpace(s, len)) {
				if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
						XSPF_READER_TEXT_ZERO_TEXT_FORBIDDEN(
						XSPF_NS_HOME, _PT("attribution")))) {
					stop();
					return;
				}
			}
			break;

		default:
			// Append unmodified
			this->d->accum.append(s, len);

		}
		break;

	case 3:
		switch (this->d->elementStack.top()) {
		case TAG_PLAYLIST_TRACKLIST_TRACK:
			// Must be all whitespace
			if (!Toolbox::isWhiteSpace(s, len)) {
				if (!handleError(XSPF_READER_ERROR_CONTENT_INVALID,
						XSPF_READER_TEXT_ZERO_TEXT_FORBIDDEN(
						XSPF_NS_HOME, _PT("track")))) {
					stop();
					return;
				}
			}
			break;

		default:
			// Append unmodified
			this->d->accum.append(s, len);

		}
		break;

	case 4:
		// Append unmodified
		this->d->accum.append(s, len);
		break;

	}
}


void
XspfReader::handleEntityDeclaration(XML_Char const * entityName,
		XML_Char const * value) {
	XML_Char const * walker = value;
	int valueLen = 0;
	int lookupSum = 0;
	int lookupDepth = 0;
	while (walker[0] != _PT('\0')) {
		XML_Char const * atAmpersand = NULL;
		XML_Char const * afterSemiColon = NULL;
		XML_Char * entityRefname = nextEntityRefMalloc(walker,
				atAmpersand, afterSemiColon);
		valueLen += static_cast<int>(atAmpersand - walker);
		if (entityRefname != NULL) {
			MapType::iterator found = this->d->entityNameToValueLen.find(
					StringType(entityRefname));
			delete[] entityRefname;
			EntityInfo const info =
					(found != this->d->entityNameToValueLen.end())
					? found->second
					: EntityInfo(1, 0, 0);
			valueLen += info.valueLen;
			lookupSum += info.lookupSum + 1;
			int const minLookupDepth = info.lookupDepth + 1;
			if (lookupDepth < minLookupDepth) {
				lookupDepth = minLookupDepth;
			}
		} else {
			valueLen += static_cast<int>(afterSemiColon - walker);
			break;
		}
		walker = afterSemiColon;
	}

	EntityInfo const info(valueLen, lookupSum, lookupDepth);
	this->d->entityNameToValueLen.insert(PairType(entityName, info));

	if (this->d->limitLengthPerEntityValue && (valueLen > this->d->maxLengthPerEntity)) {
		handleFatalError(XSPF_READER_ERROR_MALICIOUS_SPACE,
				_PT("Input considered harmful: Entity taking too much space"));
		stop();
	} else if (this->d->limitLookupSumPerEntityValue && (lookupSum > this->d->maxTotalLookupsPerEntity)) {
		handleFatalError(XSPF_READER_ERROR_MALICIOUS_LOOKUP_SUM,
				_PT("Input considered harmful: Entity requiring too many lookups"));
		stop();
	} else if (this->d->limitLookupDepthPerEntityValue && (lookupDepth > this->d->maxLookupDepthPerEntity)) {
		handleFatalError(XSPF_READER_ERROR_MALICIOUS_LOOKUP_DEPTH,
				_PT("Input considered harmful: Entity requiring too deep lookup"));
		stop();
	}
}


void XspfReader::stop() {
	// Remove handlers
	::XML_SetElementHandler(this->d->parser, NULL, NULL);
	::XML_SetCharacterDataHandler(this->d->parser, NULL);

	// Full stop
	::XML_StopParser(this->d->parser, XML_FALSE);
}


bool XspfReader::handlePlaylistAttribs(XML_Char const ** atts) {
	bool versionFound = false;
	for (int i = 0; atts[i] != NULL; i += 2) {
		if (!::PORT_STRCMP(atts[i], _PT("version"))) {
			// Check and set version
			int dummyVersion;
			if (!Toolbox::extractInteger(atts[i + 1], 0, &dummyVersion)
					|| (dummyVersion > 1)) {
				if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_INVALID,
						XSPF_READER_TEXT_ONE_WRONG_VERSION,
						atts[i + 1])) {
					return false;
				}
				dummyVersion = XSPF_FALLBACK_VERSION;
			}
			this->d->version = dummyVersion;
			versionFound = true;
		} else if (isXmlBase(atts[i])) {
			// xml:base
			XML_Char const * const xmlBase = atts[i + 1];
			if (!handleXmlBaseAttribute(xmlBase)) {
				return false;
			}
		} else {
			// Forbidden attribute
			if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ATTRIBUTE_FORBIDDEN, atts[i])) {
				return false;
			}
		}
	}

	if (!versionFound) {
		if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_MISSING,
				XSPF_READER_TEXT_ZERO_ATTRIBUTE_MISSING(
				_PT("version")))) {
			return false;
		}
		this->d->version = XSPF_FALLBACK_VERSION;
	}

	return true;
}


namespace {
	/**
	 * Checks whether a key URI contains version information.
	 */
	bool containsVersion(XML_Char const * text) {
		if (text == NULL) {
			return true;
		}
		while (true) {
			switch (text[0]) {
			case _PT('\0'):
				return false;
			case _PT('0'): // fall through
			case _PT('1'): // fall through
			case _PT('2'): // fall through
			case _PT('3'): // fall through
			case _PT('4'): // fall through
			case _PT('5'): // fall through
			case _PT('6'): // fall through
			case _PT('7'): // fall through
			case _PT('8'): // fall through
			case _PT('9'): // fall through
				return true;
			default:
				text++;
			}
		}
	}
} // anon namespace


bool
XspfReader::handleMetaLinkAttribs(XML_Char const ** atts, XML_Char const * & rel) {
	rel = NULL;
	for (int i = 0; atts[i] != NULL; i += 2) {
		if (!::PORT_STRCMP(atts[0], _PT("rel"))) {
			// Check URI
			if (Toolbox::isUri(atts[1])) {
				rel = atts[1];

				// Extra checks
				if (!Toolbox::isAbsoluteUri(atts[1])) {
					if (!handleWarning(XSPF_READER_WARNING_KEY_WITH_REL_URI, XSPF_READER_TEXT_ZERO_KEY_WITH_REL_URI(_PT("rel")))) {
						return false;
					}
				}
				if (!containsVersion(atts[1])) {
					if (!handleWarning(XSPF_READER_WARNING_KEY_WITHOUT_VERSION, XSPF_READER_TEXT_ZERO_KEY_WITHOUT_VERSION(_PT("rel")))) {
						return false;
					}
				}
			} else {
				if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_INVALID,
						XSPF_READER_TEXT_ZERO_WRONG_ATTRIBUTE_TYPE(
						_PT("rel"), _PT("URI")))) {
					return false;
				}
			}
		} else if (isXmlBase(atts[i])) {
			// xml:base
			XML_Char const * const xmlBase = atts[i + 1];
			if (!handleXmlBaseAttribute(xmlBase)) {
				return false;
			}
		} else {
			if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ATTRIBUTE_FORBIDDEN, atts[0])) {
				return false;
			}
		}
	}

	if (rel == NULL) {
		if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_MISSING,
				XSPF_READER_TEXT_ZERO_ATTRIBUTE_MISSING(_PT("rel")))) {
			return false;
		}
	}

	return true;
}


bool
XspfReader::handleExtensionAttribs(XML_Char const ** atts,
		XML_Char const * & application) {
	application = NULL;
	for (int i = 0; atts[i] != NULL; i += 2) {
		if (!::PORT_STRCMP(atts[0], _PT("application"))) {
			// Check URI
			if (Toolbox::isUri(atts[1])) {
				application = atts[1];
			} else {
				if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_INVALID,
						XSPF_READER_TEXT_ZERO_WRONG_ATTRIBUTE_TYPE(
						_PT("application"), _PT("URI")))) {
					return false;
				}
			}
		} else if (isXmlBase(atts[i])) {
			// xml:base
			XML_Char const * const xmlBase = atts[i + 1];
			if (!handleXmlBaseAttribute(xmlBase)) {
				return false;
			}
		} else {
			if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ATTRIBUTE_FORBIDDEN,
					atts[0])) {
				return false;
			}
		}
	}

	if (application == NULL) {
		if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_MISSING,
				XSPF_READER_TEXT_ZERO_ATTRIBUTE_MISSING(
				_PT("application")))) {
			return false;
		}
	}

	return true;
}


bool XspfReader::handleNoAttribsExceptXmlBase(XML_Char const ** atts) {
	// No attributes?
	for (int i = 0; atts[i] != NULL; i += 2) {
		if (isXmlBase(atts[i])) {
			// xml:base
			XML_Char const * const xmlBase = atts[i + 1];
			if (!handleXmlBaseAttribute(xmlBase)) {
				return false;
			}
		} else {
			if (!handleError(XSPF_READER_ERROR_ATTRIBUTE_FORBIDDEN,
					XSPF_READER_TEXT_ONE_ATTRIBUTE_FORBIDDEN,
					atts[0])) {
				return false;
			}
		}
	}

	return true; // Continue parsing
}


bool
XspfReader::handleError(int code, XML_Char const * text) {
	int const line = ::XML_GetCurrentLineNumber(this->d->parser);
	int const column = ::XML_GetCurrentColumnNumber(this->d->parser);
	XML_Char const * const finalText
			= (text != NULL) ? text : _PT("");
	assert(this->d->callback != NULL);
	bool const keepParsing = this->d->callback->handleError(
			line, column, code, finalText);
	if (!keepParsing) {
		this->d->errorCode = code;
	}
	return keepParsing;
}


bool
XspfReader::handleError(int code, XML_Char const * format,
		XML_Char const * param) {
	XML_Char * finalText;
	if (param != NULL) {
		size_t const charCount = ::PORT_STRLEN(format) + ::PORT_STRLEN(param) + 1;
		finalText = new XML_Char[charCount];
		::PORT_SNPRINTF(finalText, charCount, format, param);
	} else {
		finalText = const_cast<XML_Char *>(
				(format != NULL) ? format : _PT(""));
	}

	int const line = ::XML_GetCurrentLineNumber(this->d->parser);
	int const column = ::XML_GetCurrentColumnNumber(this->d->parser);
	assert(this->d->callback != NULL);
	bool const keepParsing = this->d->callback->handleError(
			line, column, code, finalText);

	if (param != NULL) {
		delete [] finalText;
	}
	if (!keepParsing) {
		this->d->errorCode = code;
	}
	return keepParsing;
}


void
XspfReader::handleFatalError(int code, XML_Char const * format,
		XML_Char const * param) {
	XML_Char * finalText;
	if (param != NULL) {
		size_t const charCount = ::PORT_STRLEN(format) + ::PORT_STRLEN(param) + 1;
		finalText = new XML_Char[charCount];
		::PORT_SNPRINTF(finalText, charCount, format, param);
	} else {
		finalText = const_cast<XML_Char *>(
				(format != NULL) ? format : _PT(""));
	}

	int const line = ::XML_GetCurrentLineNumber(this->d->parser);
	int const column = ::XML_GetCurrentColumnNumber(this->d->parser);
	assert(this->d->callback != NULL);
	this->d->callback->notifyFatalError(
			line, column, code, finalText);
	this->d->errorCode = code;

	if (param != NULL) {
		delete [] finalText;
	}
}


void
XspfReader::handleFatalError(int code, XML_Char const * text) {
	int const line = ::XML_GetCurrentLineNumber(this->d->parser);
	int const column = ::XML_GetCurrentColumnNumber(this->d->parser);
	XML_Char const * const finalText
			= (text != NULL) ? text : _PT("");
	assert(this->d->callback != NULL);
	this->d->callback->notifyFatalError(
			line, column, code, finalText);
	this->d->errorCode = code;
}


bool
XspfReader::handleWarning(int code, XML_Char const * text) {
	int const line = ::XML_GetCurrentLineNumber(this->d->parser);
	int const column = ::XML_GetCurrentColumnNumber(this->d->parser);
	XML_Char const * const finalText
			= (text != NULL) ? text : _PT("");
	assert(this->d->callback != NULL);
	return this->d->callback->handleWarning(line, column,
			code, finalText);
}


void XspfReader::clearError() {
	this->d->errorCode = XSPF_READER_SUCCESS;
}


/*static*/ void XspfReader::masterStart(void * userData, XML_Char const * fullName, XML_Char const ** atts) {
	XspfReader * const parser = reinterpret_cast<XspfReader *>(userData);
	parser->handleStart(fullName, atts);
}


/*static*/ void XspfReader::masterEnd(void * userData, XML_Char const * fullName) {
	XspfReader * const parser = reinterpret_cast<XspfReader *>(userData);
	parser->handleEnd(fullName);
}


/*static*/ void XspfReader::masterCharacters(void * userData, XML_Char const * s, int len) {
	XspfReader * const parser = reinterpret_cast<XspfReader *>(userData);
	parser->handleCharacters(s, len);
}


/*static*/ void
XspfReader::masterEntityDeclaration(void * userData, XML_Char const * entityName,
			int /*is_parameter_entity*/, XML_Char const * value, int value_length,
			XML_Char const * /*base*/, XML_Char const * /*systemId*/,
			XML_Char const * /*publicId*/, XML_Char const * /*notationName*/) {
	if (value == NULL) {
		return;
	}
	XspfReader * const parser = reinterpret_cast<XspfReader *>(userData);
	XML_Char * const zeroTerminatedValue = new XML_Char[value_length + 1];
	::PORT_STRNCPY(zeroTerminatedValue, value, value_length);
	zeroTerminatedValue[value_length] = _PT('\0');
	parser->handleEntityDeclaration(entityName, zeroTerminatedValue);
	delete[] zeroTerminatedValue;
}


XspfStack<unsigned int> &
XspfReader::getElementStack() const {
	return this->d->elementStack;
}


XspfStack<std::basic_string<XML_Char> > &
XspfReader::getBaseUriStack() const {
	return this->d->baseUriStack;
}


void
XspfReader::notifySuccess() const {
	assert(this->d->callback != NULL);
	this->d->callback->notifySuccess();
}


void
XspfReader::skipFromHere() {
	this->d->skip = true;
	this->d->skipStopLevel = static_cast<int>(
			this->d->elementStack.size());
}


void
XspfReader::limitLengthPerEntityValue(bool enabled) {
	this->d->limitLengthPerEntityValue = enabled;
}


void
XspfReader::limitLookupSumPerEntityValue(bool enabled) {
	this->d->limitLookupSumPerEntityValue = enabled;
}


void
XspfReader::limitLookupDepthPerEntityValue(bool enabled) {
	this->d->limitLookupDepthPerEntityValue = enabled;
}


void
XspfReader::enableMaliciousXmlDetection(bool enabled) {
	limitLengthPerEntityValue(enabled);
	limitLookupSumPerEntityValue(enabled);
	limitLookupDepthPerEntityValue(enabled);
}


void
XspfReader::setMaxLengthPerEntityValue(int maxLength) {
	this->d->maxLengthPerEntity = maxLength;
}


void
XspfReader::setMaxLookupSumPerEntityValue(int maxLookupSum) {
	this->d->maxTotalLookupsPerEntity = maxLookupSum;
}


void
XspfReader::setMaxLookupDepthPerEntityValue(int maxLookupDepth) {
	this->d->maxLookupDepthPerEntity = maxLookupDepth;
}

} // namespace Xspf
