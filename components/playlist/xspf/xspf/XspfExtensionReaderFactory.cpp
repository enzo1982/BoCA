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
 * @file XspfExtensionReaderFactory.cpp
 * Implementation of XspfExtensionReaderFactory.
 */

#include <xspf/XspfExtensionReaderFactory.h>
#include <xspf/XspfExtensionReader.h>
#include <xspf/XspfToolbox.h>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfExtensionReaderFactory.
 */
class XspfExtensionReaderFactoryPrivate {

	friend class XspfExtensionReaderFactory;

	///< Map of extension readers for playlist extensions
	std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare> playlistExtensionReaders;

	///< Map of extension readers for track extensions
	std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare> trackExtensionReaders; 

	///< Catch-all extension reader for playlist extensions
	XspfExtensionReader const * playlistCatchAllReader;

	///< Catch-all extension reader for track extensions
	XspfExtensionReader const * trackCatchAllReader; 

	/**
	 * Copies the extension reader and URI clones kept in a container
	 * over to another container.
	 * NOTE: The destination container is <strong>not</strong>
	 * cleared before.
	 *
	 * @param dest    Container to copy into
	 * @param source  Container to copy from
	 */
	static void copyMap(std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & dest,
			const std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & source) {
		// Copy examples and application URIs
		std::map<XML_Char const *, XspfExtensionReader const *,
				Toolbox::XspfStringCompare>
				::const_iterator iter = source.begin();
		while (iter != source.end()) {
			XML_Char const * const applicationUri
				= Toolbox::newAndCopy(iter->first);
			XspfExtensionReader const * const clone
					= iter->second->createBrother();
			dest.insert(std::pair<XML_Char const *,
					XspfExtensionReader const *>(applicationUri, clone));

			iter++;
		}
	}

	/**
	 * Deletes the extension reader and URI clones kept in a container.
	 * NOTE: The container itself is not deleted.
	 *
	 * @param container		Container to delete in
	 */
	static void freeMap(
			std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & container) {
		// Free examples and application URIs
		std::map<XML_Char const *, XspfExtensionReader const *,
				Toolbox::XspfStringCompare>::iterator iter
				= container.begin();
		while (iter != container.end()) {
			delete [] iter->first;
			delete iter->second;
			iter++;
		}
	}

	/**
	 * Creates a new D object.
	 */
	XspfExtensionReaderFactoryPrivate()
			: playlistCatchAllReader(NULL),
			trackCatchAllReader(NULL) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReaderFactoryPrivate(
			XspfExtensionReaderFactoryPrivate const & source)
			: playlistCatchAllReader((source.playlistCatchAllReader == NULL)
				? NULL
				: source.playlistCatchAllReader->createBrother()),
			trackCatchAllReader((source.trackCatchAllReader == NULL)
				? NULL
				: source.trackCatchAllReader->createBrother()) {
		copyMap(this->playlistExtensionReaders, source.playlistExtensionReaders);
		copyMap(this->trackExtensionReaders, source.trackExtensionReaders);
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReaderFactoryPrivate & operator=(
			XspfExtensionReaderFactoryPrivate const & source) {
		// playlistExtensionReaders
		freeMap(this->playlistExtensionReaders);
		this->playlistExtensionReaders.clear();
		copyMap(this->playlistExtensionReaders, source.playlistExtensionReaders);

		// trackExtensionReaders
		freeMap(this->trackExtensionReaders);
		this->trackExtensionReaders.clear();
		copyMap(this->trackExtensionReaders, source.trackExtensionReaders);

		// playlistCatchAllReader
		if (this->playlistCatchAllReader != NULL) {
			delete this->playlistCatchAllReader;
		}
		this->playlistCatchAllReader
				= (source.playlistCatchAllReader == NULL)
				? NULL
				: source.playlistCatchAllReader->createBrother();

		// trackCatchAllReader
		if (this->trackCatchAllReader != NULL) {
			delete this->trackCatchAllReader;
		}
		this->trackCatchAllReader
			= (source.trackCatchAllReader == NULL)
			? NULL
			: source.trackCatchAllReader->createBrother();
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfExtensionReaderFactoryPrivate() {
		freeMap(this->playlistExtensionReaders);
		freeMap(this->trackExtensionReaders);
		if (this->playlistCatchAllReader != NULL) {
			delete this->playlistCatchAllReader;
		}
		if (this->trackCatchAllReader != NULL) {
			delete this->trackCatchAllReader;
		}
	}

};

/// @endcond


XspfExtensionReaderFactory::XspfExtensionReaderFactory()
		: d(new XspfExtensionReaderFactoryPrivate()) {

}


XspfExtensionReaderFactory::XspfExtensionReaderFactory(
		XspfExtensionReaderFactory const & source)
		: d(new XspfExtensionReaderFactoryPrivate(*(source.d))) {

}


XspfExtensionReaderFactory &
XspfExtensionReaderFactory::operator=(
		XspfExtensionReaderFactory const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfExtensionReaderFactory::~XspfExtensionReaderFactory() {
	delete this->d;
}


inline void
XspfExtensionReaderFactory::registerReader(
		std::map<XML_Char const *, XspfExtensionReader const *,
		Toolbox::XspfStringCompare> & container, XspfExtensionReader const * & catchAll,
		XspfExtensionReader const * example,
		XML_Char const * triggerUri) {
	if (example == NULL) {
		return;
	}
	XspfExtensionReader const * const clone = example->createBrother();

	// CatchAll reader?
	if (triggerUri == NULL) {
		// Overwrite old catcher
		if (catchAll != NULL) {
			delete catchAll;
		}
		catchAll = clone;
	} else {
		std::map<XML_Char const *, XspfExtensionReader const *,
				Toolbox::XspfStringCompare>
				::iterator found = container.find(triggerUri);
		if (found != container.end()) {
			// Overwrite existing entry
			delete found->second;
			found->second = clone;
		} else {
			// Add new entry with duped URI
			container.insert(std::pair<XML_Char const *,
					XspfExtensionReader const *>(
					Toolbox::newAndCopy(triggerUri),
					clone));
		}
	}
}


void
XspfExtensionReaderFactory::registerPlaylistExtensionReader(
		XspfExtensionReader const * example,
		XML_Char const * triggerUri) {
	registerReader(this->d->playlistExtensionReaders,
			this->d->playlistCatchAllReader, example, triggerUri);
}


void
XspfExtensionReaderFactory::registerTrackExtensionReader(
		XspfExtensionReader const * example,
		XML_Char const * triggerUri) {
	registerReader(this->d->trackExtensionReaders,
			this->d->trackCatchAllReader, example, triggerUri);
}


inline void
XspfExtensionReaderFactory::unregisterReader(
		std::map<XML_Char const *, XspfExtensionReader const *,
		Toolbox::XspfStringCompare> & container,
		XspfExtensionReader const * & catchAll,
		XML_Char const * triggerUri) {
	// CatchAll reader?
	if (triggerUri == NULL) {
		// Remove catcher
		if (catchAll != NULL) {
			delete catchAll;
			catchAll = NULL;
		}
	} else {
		std::map<XML_Char const *, XspfExtensionReader const *,
				Toolbox::XspfStringCompare>
				::iterator found = container.find(triggerUri);
		if (found != container.end()) {
			delete found->second;
			container.erase(found);
		}
	}
}


void
XspfExtensionReaderFactory::unregisterPlaylistExtensionReader(
		XML_Char const * triggerUri) {
	unregisterReader(this->d->playlistExtensionReaders,
			this->d->playlistCatchAllReader, triggerUri);
}


void
XspfExtensionReaderFactory::unregisterTrackExtensionReader(
		XML_Char const * triggerUri) {
	unregisterReader(this->d->trackExtensionReaders,
			this->d->trackCatchAllReader, triggerUri);
}


inline XspfExtensionReader *
XspfExtensionReaderFactory::newReader(
		std::map<XML_Char const *, XspfExtensionReader const *,
		Toolbox::XspfStringCompare> & container,
		XspfExtensionReader const * catchAll,
		XML_Char const * applicationUri,
		XspfReader * reader) {
	std::map<XML_Char const *, XspfExtensionReader const *,
			Toolbox::XspfStringCompare>::const_iterator
			found = container.find(applicationUri);
	if (found != container.end()) {
		return found->second->createBrother(reader);
	} else {
		if (catchAll != NULL) {
			return catchAll->createBrother(reader);
		} else {
			return NULL;
		}
	}
}


XspfExtensionReader *
XspfExtensionReaderFactory::newPlaylistExtensionReader(XML_Char const * applicationUri,
		XspfReader * reader) {
	return newReader(this->d->playlistExtensionReaders,
			this->d->playlistCatchAllReader, applicationUri, reader);
}


XspfExtensionReader *
XspfExtensionReaderFactory::newTrackExtensionReader(
		XML_Char const * applicationUri, XspfReader * reader) {
	return newReader(this->d->trackExtensionReaders,
			this->d->trackCatchAllReader, applicationUri, reader);
}


} // namespace Xspf
