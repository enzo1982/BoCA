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
 * @file XspfProps.cpp
 * Implementation of XspfProps.
 */

#include <xspf/XspfProps.h>
#include <xspf/XspfDateTime.h>
#include <xspf/XspfToolbox.h>
#include <cstring>

namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfProps.
 */
class XspfPropsPrivate {

	friend class XspfProps;

	XML_Char const * location; ///< Location URI
	XML_Char const * license; ///< License URI
	XML_Char const * identifier; ///< Identifier URI
	bool ownLocation; ///< Location memory ownership flag
	bool ownLicense; ///< License memory ownership flag
	bool ownIdentifier; ///< Identifier memory ownership flag
	std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *> * attributions; ///< List of location and identifier URIs
	XspfDateTime const * date; ///< Date
	bool ownDate; ///< Date memory ownership flag
	int version; ///< XSPF version

	/**
	 * Creates a new D object.
	 */
	XspfPropsPrivate()
			:location(NULL),
			license(NULL),
			identifier(NULL),
			ownLocation(false),
			ownLicense(false),
			ownIdentifier(false),
			attributions(NULL),
			date(NULL),
			ownDate(false),
			version(1) { }

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsPrivate(XspfPropsPrivate const & source)
			: location(source.ownLocation
				? Toolbox::newAndCopy(source.location)
				: source.location),
			license(source.ownLicense
				? Toolbox::newAndCopy(source.license)
				: source.license),
			identifier(source.ownIdentifier
				? Toolbox::newAndCopy(source.identifier)
				: source.identifier),
			ownLocation(source.ownLocation),
			ownLicense(source.ownLicense),
			ownIdentifier(source.ownIdentifier),
			attributions(NULL), // Created in appendHelper
			date(source.ownDate
				? new XspfDateTime(*(source.date))
				: source.date),
			ownDate(source.ownDate),
			version(source.version) {
		if (source.attributions != NULL) {
			copyAttributions(this->attributions, source.attributions);
		}
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsPrivate & operator=(XspfPropsPrivate const & source) {
		if (this != &source) {
			free();
			assign(source);
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfPropsPrivate() {
		free();
	}

	static void copyAttributions(std::deque<std::pair<bool,
			std::pair<XML_Char const *, bool> *> *> * & dest,
			const std::deque<std::pair<bool,
			std::pair<XML_Char const *, bool> *> *> * source) {
		std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *>
				::const_iterator iter = source->begin();
		while (iter != source->end()) {
			const std::pair<bool, std::pair<XML_Char const *, bool> *>
					* const entry = *iter;
			bool const ownership = entry->second->second;
			XML_Char const * const value = ownership
					? Toolbox::newAndCopy(entry->second->first)
					: entry->second->first;
			bool const isLocation = entry->first;
			XspfProps::appendHelper(dest, value,
					ownership, isLocation);
			iter++;
		}
	}

	void assign(XspfPropsPrivate const & source) {
		// Assigns all members, does not free current content
		Toolbox::copyIfOwned(this->location, this->ownLocation, source.location, source.ownLocation);
		Toolbox::copyIfOwned(this->license, this->ownLicense, source.license, source.ownLicense);
		Toolbox::copyIfOwned(this->identifier, this->ownIdentifier, source.identifier, source.ownIdentifier);
		this->attributions = NULL; // Created by appendHelper
		this->date = source.ownDate
				? new XspfDateTime(*(source.date))
				: source.date;
		this->ownDate = source.ownDate;
		this->version = source.version;

		if (source.attributions != NULL) {
			copyAttributions(this->attributions, source.attributions);
		}
	}

	void free() {
		// Frees all data, does not set to NULL
		Toolbox::freeIfOwned(this->location, this->ownLocation);
		Toolbox::freeIfOwned(this->identifier, this->ownIdentifier);
		Toolbox::freeIfOwned(this->license, this->ownLicense);
		if (this->attributions != NULL) {
			std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *>::const_iterator iter = this->attributions->begin();
			while (iter != this->attributions->end()) {
				std::pair<bool, std::pair<XML_Char const *, bool> *> * const entry = *iter;
				if (entry->second->second) {
					delete [] entry->second->first;
				}
				delete entry->second;
				delete entry;
				iter++;
			}
			delete this->attributions;
			this->attributions = NULL;
		}
		if (this->ownDate && (this->date != NULL)) {
			delete this->date;
			this->date = NULL;
		}
	}

};

/// @endcond


XspfProps::XspfProps()
		: XspfData(),
		d(new XspfPropsPrivate()) {

}


XspfProps::XspfProps(XspfProps const & source)
		: XspfData(source),
		d(new XspfPropsPrivate(*(source.d))) {

}


XspfProps & XspfProps::operator=(XspfProps const & source) {
	if (this != &source) {
		XspfData::operator=(source);
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfProps::~XspfProps() {
	delete this->d;
}


void XspfProps::giveLocation(XML_Char const * location, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->location, this->d->ownLocation, location, copy);
}


void XspfProps::giveLicense(XML_Char const * license, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->license, this->d->ownLicense, license, copy);
}


void XspfProps::giveIdentifier(XML_Char const * identifier, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->identifier, this->d->ownIdentifier, identifier, copy);
}


void XspfProps::giveAppendAttributionIdentifier(XML_Char const * identifier, bool copy) {
	bool const IS_IDENTIFIER = false;
	appendHelper(this->d->attributions, copy ? Toolbox::newAndCopy(identifier)
		: identifier, true, IS_IDENTIFIER);
}


void XspfProps::giveAppendAttributionLocation(XML_Char const * location, bool copy) {
	bool const IS_LOCATION = true;
	appendHelper(this->d->attributions, copy ? Toolbox::newAndCopy(location)
		: location, true, IS_LOCATION);
}


void XspfProps::giveDate(XspfDateTime const * date, bool copy) {
	XspfProps::deleteNewAndCopy(this->d->date, this->d->ownDate, date, copy);
}


void XspfProps::lendLocation(XML_Char const * location) {
	Toolbox::deleteNewAndCopy(this->d->location, this->d->ownLocation, location, false);
}


void XspfProps::lendLicense(XML_Char const * license) {
	Toolbox::deleteNewAndCopy(this->d->license, this->d->ownLicense, license, false);
}


void XspfProps::lendIdentifier(XML_Char const * identifier) {
	Toolbox::deleteNewAndCopy(this->d->identifier, this->d->ownIdentifier, identifier, false);
}


void XspfProps::lendAppendAttributionIdentifier(XML_Char const * identifier) {
	appendHelper(this->d->attributions, identifier, false, false);
}


void XspfProps::lendAppendAttributionLocation(XML_Char const * location) {
	appendHelper(this->d->attributions, location, false, true);
}


void XspfProps::lendDate(XspfDateTime const * date) {
	XspfProps::deleteNewAndCopy(this->d->date, this->d->ownDate, date, false);
}


XML_Char * XspfProps::stealLocation() {
	return XspfData::stealHelper(this->d->location, this->d->ownLocation);
}


XML_Char * XspfProps::stealIdentifier() {
	return XspfData::stealHelper(this->d->identifier, this->d->ownIdentifier);
}


XML_Char * XspfProps::stealLicense() {
	return XspfData::stealHelper(this->d->license, this->d->ownLicense);
}


std::pair<bool, XML_Char *> * XspfProps::stealFirstAttribution() {
	return stealFirstHelper(this->d->attributions);
}


XspfDateTime * XspfProps::stealDate() {
	return stealHelper(this->d->date, this->d->ownDate);
}


void XspfProps::setVersion(int version) {
	this->d->version = version;
}


XML_Char const * XspfProps::getLicense() const {
	return this->d->license;
}


XML_Char const * XspfProps::getLocation() const {
	return this->d->location;
}


XML_Char const * XspfProps::getIdentifier() const {
	return this->d->identifier;
}


std::pair<bool, XML_Char const *> * XspfProps::getAttribution(int index) const {
	return getHelper(this->d->attributions, index);
}


XspfDateTime const * XspfProps::getDate() const {
	return this->d->date;
}


int XspfProps::getVersion() const {
	return this->d->version;
}


int XspfProps::getAttributionCount() const {
	return (this->d->attributions == NULL) ? 0 : static_cast<int>(this->d->attributions->size());
}


/*static*/ void XspfProps::appendHelper(
		std::deque<std::pair<bool,
		std::pair<XML_Char const *, bool> *> *> * & container,
		XML_Char const * value, bool ownership, bool isLocation) {
	if (container == NULL) {
		container = new std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *>;
	}
	std::pair<XML_Char const *, bool> * const second = new std::pair<XML_Char const *, bool>(value, ownership);
	std::pair<bool, std::pair<XML_Char const *, bool> *> * const entry =
		new std::pair<bool, std::pair<XML_Char const *, bool> *>(isLocation, second);
	container->push_back(entry);
}


/*static*/ std::pair<bool, XML_Char const *> * XspfProps::getHelper(
		std::deque<std::pair<bool,
		std::pair<XML_Char const *, bool> *> *> * & container,
		int index) {
	if ((container == NULL) || container->empty() || (index < 0)
			|| (index >= static_cast<int>(container->size()))) {
		return NULL;
	}
	std::pair<bool, std::pair<XML_Char const *, bool> *> * const entry
			= container->at(index);

	// NOTE: getX() just peeps at data so don't clone anything
	std::pair<bool, XML_Char const *> * const res
			= new std::pair<bool, XML_Char const *>(
			entry->first, entry->second->first);
	return res;
}


/*static*/ XspfDateTime * XspfProps::stealHelper(
		XspfDateTime const * & dateTime, bool own) {
	XspfDateTime const * const res
			= Toolbox::getSetNull<XspfDateTime>(dateTime);
	if (own) {
		return const_cast<XspfDateTime *>(res);
	} else if (res == NULL) {
		return NULL;
	} else {
		return res->clone();
	}
}


/*static*/ std::pair<bool, XML_Char *> * XspfProps::stealFirstHelper(
		std::deque<std::pair<bool,
		std::pair<XML_Char const *, bool> *> *> * & container) {
	if ((container == NULL) || container->empty()) {
		return NULL;
	}
	std::pair<bool, std::pair<XML_Char const *, bool> *> * const entry =
			container->front();
	container->pop_front();
	std::pair<bool, XML_Char *> * const res = new std::pair<bool, XML_Char *>(
		entry->first, entry->second->second
		? const_cast<XML_Char *>(entry->second->first)
		: Toolbox::newAndCopy(entry->second->first));
	delete entry->second;
	delete entry;
	return res;
}


/*static*/ void XspfProps::deleteNewAndCopy(XspfDateTime const * & dest,
		bool & destOwnership, XspfDateTime const * source, bool sourceCopy) {
	// Delete old memory if owner
	if (destOwnership && (dest != NULL)) {
		delete [] dest;
	}

	if (source == NULL) {
		dest = NULL;
		destOwnership = false;
	} else {
		// Copy new memory if desired
		if (sourceCopy) {
			XspfDateTime * const dup = source->clone();
			dest = dup;
			destOwnership = true;
		} else {
			dest = source;
			destOwnership = false;
		}
	}
}


}
