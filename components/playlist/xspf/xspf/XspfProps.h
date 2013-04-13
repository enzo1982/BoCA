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
 * @file XspfProps.h
 * Interface of XspfProps.
 */

#ifndef XSPF_PROPS_H
#define XSPF_PROPS_H


#include "XspfData.h"

namespace Xspf {


class XspfPropsPrivate;
class XspfDateTime;


/**
 * Represents the properties of playlist
 * without extensions. This includes all
 * information except the track list.
 */
class XspfProps : public XspfData {

	friend class XspfPropsPrivate; // Give access to static helpers

private:
	/// @cond DOXYGEN_NON_API
	XspfPropsPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new, blank playlist property package.
	 */
	XspfProps();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfProps(XspfProps const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfProps & operator=(XspfProps const & source);

	/**
	 * Destroys this XspfProps object and deletes all
	 * memory associated with it that has not been stolen before.
	 */
	~XspfProps();

	/**
	 * Overwrites the identifier property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param identifier	Identifier string to set
	 * @param copy			Copy flag
	 */
	void giveIdentifier(XML_Char const * identifier, bool copy);

	/**
	 * Overwrites the license property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param license	License string to set
	 * @param copy		Copy flag
	 */
	void giveLicense(XML_Char const * license, bool copy);

	/**
	 * Overwrites the location property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param location	Location string to set
	 * @param copy		Copy flag
	 */
	void giveLocation(XML_Char const * location, bool copy);

	/**
	 * Appends an identifier to the attribution list.
	 *
	 * @param identifier	Identifier to append
	 * @param copy		Copy flag
	 */
	void giveAppendAttributionIdentifier(XML_Char const * identifier, bool copy);

	/**
	 * Appends an location to the attribution list.
	 *
	 * @param location	Location to append
	 * @param copy		Copy flag
	 */
	void giveAppendAttributionLocation(XML_Char const * location, bool copy);

	/**
	 * Overwrites the date property. If <c>copy</c> is true
	 * the date object will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param date		Date object to set
	 * @param copy		Copy flag
	 */
	void giveDate(XspfDateTime const * date, bool copy);

	/**
	 * Overwrites the identifier property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param identifier		Identifier string to set
	 */
	void lendIdentifier(XML_Char const * identifier);

	/**
	 * Overwrites the license property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param license		License string to set
	 */
	void lendLicense(XML_Char const * license);

	/**
	 * Overwrites the location property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param location		Location string to set
	 */
	void lendLocation(XML_Char const * location);

	/**
	 * Appends an identifier to the attribution list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param identifier	Identifier to append
	 */
	void lendAppendAttributionIdentifier(XML_Char const * identifier);

	/**
	 * Appends an location to the attribution list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param location	Location to append
	 */
	void lendAppendAttributionLocation(XML_Char const * location);

	/**
	 * Overwrites the date property. The date object is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param date		Date object to set
	 */
	void lendDate(XspfDateTime const * date);

	/**
	 * Overwrites the XSPF version property.
	 *
	 * @param version		XSPF version (0 or 1)
	 */
	void setVersion(int version);

	/**
	 * Steals the license property.
	 *
	 * @return	License URI, can be NULL
	 */
	XML_Char * stealLicense();

	/**
	 * Steals the location property.
	 *
	 * @return	Location URI, can be NULL
	 */
	XML_Char * stealLocation();

	/**
	 * Steals the identifier property.
	 *
	 * @return	Identifier URI, can be NULL
	 */
	XML_Char * stealIdentifier();

	/**
	 * Steals the first attribution entry from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: Do not forget to delete the pair!
	 *
	 * @return		First attribution entry, can be NULL
	 */
	std::pair<bool, XML_Char *> * stealFirstAttribution();

	/**
	 * Steals the date object property.
	 *
	 * @return	Date object, can be NULL
	 */
	XspfDateTime * stealDate();

	/**
	 * Returns the license property.
	 *
	 * @return	License, can be NULL
	 */
	XML_Char const * getLicense() const;

	/**
	 * Returns the location property.
	 *
	 * @return	Location, can be NULL
	 */
	XML_Char const * getLocation() const;

	/**
	 * Returns the identifier property.
	 *
	 * @return	Identifier, can be NULL
	 */
	XML_Char const * getIdentifier() const;

	/**
	 * Gets the specific attribution entry from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @return	Specified attribution entry, can be NULL
	 */
	std::pair<bool, XML_Char const *> * getAttribution(int index) const;

	/**
	 * Returns the number of attributions.
	 *
	 * @return	Number of attributions
	 */
	int getAttributionCount() const;

	/**
	 * Returns the date object property.
	 *
	 * @return	Date object, can be NULL
	 */
	XspfDateTime const * getDate() const;

	/**
	 * Returns the XSPF version property.
	 *
	 * @return	XSPF version (0 or 1)
	 */
	int getVersion() const;

private:
	/**
	 * Appends a location or identifier to a container.
	 *
	 * @param container		Container to work with
	 * @param value			Value
	 * @param ownership		Ownership flag
	 * @param isLocation	Location/identifier switch
	 */
	static void appendHelper(std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *> * & container,
			XML_Char const * value, bool ownership, bool isLocation);

	/**
	 * Returns a specific entry from a container
	 * or <c>NULL</c> if the entry does not exist.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @param container		Container to work with
	 * @param index			Index of the entry to return
	 * @return				Entry content, can be NULL
	 */
	static std::pair<bool, XML_Char const *> * getHelper(
			std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *> * & container, int index);

	/**
	 * Steals a XspfDateTime. If its memory is not owned
	 * a clone is returned. In any case you own the memory
	 * return and have to delete it.
	 *
	 * @param dateTime	XspfDateTime to steal
	 * @param own		Owner flag
	 * @return			Stolen XspfDateTime value, can be NULL
	 */
	static XspfDateTime * stealHelper(XspfDateTime const * & dateTime,
			bool own);

	/**
	 * Steals the first entry from a container.
	 *
	 * @param container		Container to steal from
	 * @return				First entry, can be NULL
	 */
	static std::pair<bool, XML_Char *> * stealFirstHelper(
			std::deque<std::pair<bool, std::pair<XML_Char const *, bool> *> *> * & container);

	/**
	 * Replaces the date object in <c>dest</c> by a duplicate of the date object
	 * in <c>src</c>  (using new() not malloc()). The old object is deleted.
	 * If <c>destOwnership</c> is false the old string is not deleted.
	 * If <c>sourceCopy</c> is false only <c>source</c>'s pointer is copied,
	 * not the whole object.
	 *
	 * @param dest			Destination date object
	 * @param destOwnership	Destination ownership flag
	 * @param source		Source date object
	 * @param sourceCopy	Source copy flag
	 */
	static void deleteNewAndCopy(XspfDateTime const * & dest, bool & destOwnership,
			XspfDateTime const * source, bool sourceCopy);

};


}

#endif // XSPF_PROPS_H
