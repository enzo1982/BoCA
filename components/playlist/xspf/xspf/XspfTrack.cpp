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
 * @file XspfTrack.cpp
 * Implementation of XspfTrack.
 */

#include <xspf/XspfTrack.h>
#include <xspf/XspfToolbox.h>
#include <cstring>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfTrack.
 */
class XspfTrackPrivate {

	friend class XspfTrack;

	XML_Char const * album; ///< Album
	bool ownAlbum; ///< Album memory ownership flag
	std::deque<std::pair<XML_Char const *, bool> *> * locations; ///< List of URI locations
	std::deque<std::pair<XML_Char const *, bool> *> * identifiers; ///< List of URI identifiers
	int trackNum; ///< Number of the track, must be positive
	int duration; ///< Duration in milliseconds, must be non-negative

	/**
	 * Creates a new D object.
	 */
	XspfTrackPrivate()
			: album(NULL),
			ownAlbum(false),
			locations(NULL),
			identifiers(NULL),
			trackNum(-1),
			duration(-1) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrackPrivate(XspfTrackPrivate const & source)
			: album(source.ownAlbum
				? Toolbox::newAndCopy(source.album)
				: source.album),
			ownAlbum(source.ownAlbum),
			locations(NULL), // Created in copyDeque
			identifiers(NULL), // Created in copyDeque
			trackNum(source.trackNum),
			duration(source.duration) {
		if (source.locations != NULL) {
			copyDeque(this->locations, source.locations);
		}
		if (source.identifiers != NULL) {
			copyDeque(this->identifiers, source.identifiers);
		}
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrackPrivate & operator=(XspfTrackPrivate const & source) {
		if (this != &source) {
		    free();
            assign(source);
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfTrackPrivate() {
		free();
	}
	
	void free() {
		Toolbox::freeIfOwned(this->album, this->ownAlbum);
		if (this->locations != NULL) {
			freeDeque(this->locations);
		}
		if (this->identifiers != NULL) {
			freeDeque(this->identifiers);
		}
	}
	
	void assign(XspfTrackPrivate const & source) {
        Toolbox::copyIfOwned(this->album, this->ownAlbum, source.album, source.ownAlbum);

		if (source.locations != NULL) {
			copyDeque(this->locations, source.locations);
		}

		if (source.identifiers != NULL) {
			copyDeque(this->identifiers, source.identifiers);
		}

		this->trackNum = source.trackNum;
		this->duration = source.duration;
	}

	static void freeDeque(std::deque<
			std::pair<XML_Char const *, bool> *> * & container) {
		std::deque<std::pair<XML_Char const *, bool> *>::const_iterator
				iter = container->begin();
		while (iter != container->end()) {
			std::pair<XML_Char const *, bool> * const entry = *iter;
			if (entry->second) {
				delete [] entry->first;
			}
			delete entry;
			iter++;
		}
		container->clear();
		delete container;
		container = NULL;
	}

	static void copyDeque(std::deque<
			std::pair<XML_Char const *, bool> *> * & dest,
			const std::deque<
			std::pair<XML_Char const *, bool> *> * source) {
		std::deque<std::pair<XML_Char const *, bool> *>::const_iterator
				iter = source->begin();
		while (iter != source->end()) {
			std::pair<XML_Char const *, bool> * const entry = *iter;

			bool const ownership = entry->second;
			XML_Char const * const value = ownership
					? Toolbox::newAndCopy(entry->first)
					: entry->first;
			XspfTrack::appendHelper(dest, value, ownership);

			iter++;
		}
	}

};

/// @endcond


XspfTrack::XspfTrack()
		: XspfData(),
		d(new XspfTrackPrivate()) {

}


XspfTrack::XspfTrack(XspfTrack const & source)
		: XspfData(source),
		d(new XspfTrackPrivate(*(source.d))) {

}


XspfTrack & XspfTrack::operator=(XspfTrack const & source) {
	if (this != &source) {
		XspfData::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfTrack::~XspfTrack() {
	delete this->d;
}


void XspfTrack::giveAlbum(XML_Char const * album, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->album, this->d->ownAlbum, album, copy);
}


void XspfTrack::lendAlbum(XML_Char const * album) {
	Toolbox::deleteNewAndCopy(this->d->album, this->d->ownAlbum, album, false);
}


void XspfTrack::setTrackNum(int trackNum) {
	this->d->trackNum = trackNum;
}


void XspfTrack::setDuration(int duration) {
	this->d->duration = duration;
}


int XspfTrack::getTrackNum() const {
	return this->d->trackNum;
}


int XspfTrack::getDuration() const {
	return this->d->duration;
}


XML_Char * XspfTrack::stealAlbum() {
	return XspfData::stealHelper(this->d->album, this->d->ownAlbum);
}


XML_Char const * XspfTrack::getAlbum() const {
	return this->d->album;
}


void XspfTrack::giveAppendIdentifier(XML_Char const * identifier, bool copy) {
	appendHelper(this->d->identifiers, copy
			? Toolbox::newAndCopy(identifier)
			: identifier,
			true);
}


void XspfTrack::giveAppendLocation(XML_Char const * location, bool copy) {
	appendHelper(this->d->locations, copy
			? Toolbox::newAndCopy(location)
			: location,
			true);
}


void XspfTrack::lendAppendIdentifier(XML_Char const * identifier) {
	appendHelper(this->d->identifiers, identifier, false);
}


void XspfTrack::lendAppendLocation(XML_Char const * location) {
	appendHelper(this->d->locations, location, false);
}


XML_Char * XspfTrack::stealFirstIdentifier() {
	return stealFirstHelper(this->d->identifiers);
}


XML_Char * XspfTrack::stealFirstLocation() {
	return stealFirstHelper(this->d->locations);
}


XML_Char const * XspfTrack::getIdentifier(int index) const {
	return getHelper(this->d->identifiers, index);
}


XML_Char const * XspfTrack::getLocation(int index) const {
	return getHelper(this->d->locations, index);
}


int XspfTrack::getIdentifierCount() const {
	return (this->d->identifiers == NULL) ? 0 : static_cast<int>(this->d->identifiers->size());
}


int XspfTrack::getLocationCount() const {
	return (this->d->locations == NULL) ? 0 : static_cast<int>(this->d->locations->size());
}


/*static*/ void XspfTrack::appendHelper(
		std::deque<std::pair<XML_Char const *, bool> *> * & container,
		XML_Char const * value, bool ownership) {
	if (container == NULL) {
		container = new std::deque<std::pair<XML_Char const *, bool> *>;
	}
	std::pair<XML_Char const *, bool> * const entry =
			new std::pair<XML_Char const *, bool>(value, ownership);
	container->push_back(entry);
}


/*static*/ XML_Char * XspfTrack::stealFirstHelper(
		std::deque<std::pair<XML_Char const *, bool> *> * & container) {
	if ((container == NULL) || container->empty()) {
		return NULL;
	}
	std::pair<XML_Char const *, bool> * const entry = container->front();
	container->pop_front();
	XML_Char * const res = entry->second
			? const_cast<XML_Char *>(entry->first)
			: Toolbox::newAndCopy(entry->first);
	delete entry;
	return res;
}


/*static*/ XML_Char const * XspfTrack::getHelper(
		std::deque<std::pair<XML_Char const *, bool> *> * & container,
		int index) {
	if ((container == NULL) || container->empty() || (index < 0)
			|| (index >= static_cast<int>(container->size()))) {
		return NULL;
	}
	std::pair<XML_Char const *, bool> * const entry = container->at(index);

	// NOTE: getX() just peeps at data so don't clone anything
	return entry->first;
}


}
