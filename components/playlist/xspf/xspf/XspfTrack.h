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
 * @file XspfTrack.h
 * Interface of XspfTrack.
 */

#ifndef XSPF_TRACK_H
#define XSPF_TRACK_H


#include "XspfData.h"

namespace Xspf {


class XspfTrackPrivate;


/**
 * Represents an XSPF track without extensions.
 */
class XspfTrack : public XspfData {

	friend class XspfTrackPrivate;

private:
	/// @cond DOXYGEN_NON_API
	XspfTrackPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new, blank track.
	 */
	XspfTrack();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrack(XspfTrack const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrack & operator=(XspfTrack const & source);

	/**
	 * Deletes all memory that has not been stolen before.
	 */
	~XspfTrack();

	/**
	 * Overwrites the album property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param album		Album string to set
	 * @param copy		Copy flag
	 */
	void giveAlbum(XML_Char const * album, bool copy);

	/**
	 * Appends an identifier to the identifier list.
	 *
	 * @param identifier	Identifier to append
	 * @param copy		Copy flag
	 */
	void giveAppendIdentifier(XML_Char const * identifier, bool copy);

	/**
	 * Appends an location to the location list.
	 *
	 * @param location	Location to append
	 * @param copy		Copy flag
	 */
	void giveAppendLocation(XML_Char const * location, bool copy);

	/**
	 * Overwrites the album property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param album		Album string to set
	 */
	void lendAlbum(XML_Char const * album);

	/**
	 * Appends an location to the location list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param location	Location to append
	 */
	void lendAppendLocation(XML_Char const * location);

	/**
	 * Appends an identifier to the identifier list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param identifier	Identifier to append
	 */
	void lendAppendIdentifier(XML_Char const * identifier);

	/**
	 * Overwrites the track number property.
	 *
	 * @param trackNum		Track number to set
	 */
	void setTrackNum(int trackNum);

	/**
	 * Overwrites the duration property.
	 * Durations are in milliseconds.
	 *
	 * @param duration		Duration to set
	 */
	void setDuration(int duration);

	/**
	 * Steals the album property.
	 *
	 * @return	Album, can be NULL
	 */
	XML_Char * stealAlbum();

	/**
	 * Steals the first identifier from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * @return		First identifier, can be NULL
	 */
	XML_Char * stealFirstIdentifier();

	/**
	 * Steals the first location from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * @return		First location, can be NULL
	 */
	XML_Char * stealFirstLocation();

	/**
	 * Returns the album property.
	 *
	 * @return	Album, can be NULL
	 */
	XML_Char const * getAlbum() const;

	/**
	 * Gets a specific identifier from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * @return		Specified identifier, can be NULL
	 */
	XML_Char const * getIdentifier(int index) const;

	/**
	 * Gets a specific location from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * @return		Specified location, can be NULL
	 */
	XML_Char const * getLocation(int index) const;

	/**
	 * Returns the number of identifiers.
	 *
	 * @return	Number of identifiers
	 */
	int getIdentifierCount() const;

	/**
	 * Returns the number of locations.
	 *
	 * @return	Number of locations
	 */
	int getLocationCount() const;

	/**
	 * Returns the duration property.
	 * Durations are measured in milliseconds.
	 *
	 * @return	Duration, zero-based, -1 if unknown
	 */
	int getDuration() const;

	/**
	 * Returns the track number property.
	 *
	 * @return	Track number, one-based, -1 if unknown
	 */
	int getTrackNum() const;

private:
	/**
	 * Appends an entry to a container.
	 *
	 * @param container		Container to work with
	 * @param value			Value to append
	 * @param ownership		Ownership flag
	 */
	static void appendHelper(std::deque<std::pair<XML_Char const *, bool> *> * & container, XML_Char const * value, bool ownership);

	/**
	 * Steals the first entry from a container.
	 *
	 * @param container		Container to steal from
	 * @return				First entry, can be NULL
	 */
	static XML_Char * stealFirstHelper(std::deque<std::pair<XML_Char const *, bool> *> * & container);

	/**
	 * Returns a specific entry from a container
	 * or <c>NULL</c> if the entry does not exist.
	 *
	 * @param container		Container to work with
	 * @param index			Index of the entry to return
	 * @return				Entry content, can be NULL
	 */
	static XML_Char const * getHelper(std::deque<std::pair<XML_Char const *, bool> *> * & container, int index);

};


}

#endif // XSPF_TRACK_H
