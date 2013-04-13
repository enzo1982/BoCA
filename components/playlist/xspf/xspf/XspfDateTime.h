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
 * @file XspfDateTime.h
 * Interface of XspfDateTime.
 */

#ifndef XSPF_DATE_TIME_H
#define XSPF_DATE_TIME_H

#include "XspfDefines.h"


namespace Xspf {


class XspfDateTimePrivate;


/**
 * Represents a "dateTime" timestamp
 * without fractional second component.
 */
class XspfDateTime {

private:
	/// @cond DOXYGEN_NON_API
	XspfDateTimePrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new dateTime timestamp.
	 * The given parameters must be valid and are not
	 * checked for validity inside.
	 *
	 * @param year			Year [-9999..+9999] but not zero
	 * @param month			Month [1..12]
	 * @param day			Day [1..31]
	 * @param hour			Hour [0..23]
	 * @param minutes		Minutes [0..59]
	 * @param seconds		Seconds [0..59]
	 * @param distHours		Time shift hours [-14..+14]
	 * @param distMinutes	Time shift minutes [-59..+59]
	 */
	XspfDateTime(int year, int month, int day, int hour, int minutes,
			int seconds, int distHours, int distMinutes);

	/**
	 * Creates a new dateTime timestamp.
	 */
	XspfDateTime();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfDateTime(XspfDateTime const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfDateTime & operator=(XspfDateTime const & source);

	/**
	 * Destroys this dateTime timestamp.
	 */
	~XspfDateTime();

	/**
	 * Clones this dateTime object.
	 *
	 *`@return	Cloned object
	 */
	XspfDateTime * clone() const;

	/**
	 * Returns the year.
	 *
	 * @return  Year
	 */
	int getYear() const;

	/**
	 * Returns the month.
	 *
	 * @return  Month
	 */
	int getMonth() const;

	/**
	 * Returns the day.
	 *
	 * @return  Day
	 */
	int getDay() const;

	/**
	 * Returns the hour.
	 *
	 * @return  Hour
	 */
	int getHour() const;

	/**
	 * Returns the minutes.
	 *
	 * @return  Minutes
	 */
	int getMinutes() const;

	/**
	 * Returns the seconds.
	 *
	 * @return  Seconds
	 */
	int getSeconds() const;

	/**
	 * Returns the time shift hours.
	 *
	 * @return  Time shift hours
	 */
	int getDistHours() const;

	/**
	 * Returns the time shift minutes.
	 *
	 * @return  Time shift minutes
	 */
	int getDistMinutes() const;

	/**
	 * Sets the year.
	 *
	 * @param year  Year to set
	 */
	void setYear(int year);

	/**
	 * Sets the month.
	 *
	 * @param month  Month to set
	 */
	void setMonth(int month);

	/**
	 * Sets the day.
	 *
	 * @param day  Day to set
	 */
	void setDay(int day);

	/**
	 * Sets the hour.
	 *
	 * @param hour  Hour to set
	 */
	void setHour(int hour);

	/**
	 * Sets the minutes.
	 *
	 * @param minutes  Minutes to set
	 */
	void setMinutes(int minutes);

	/**
	 * Sets the seconds.
	 *
	 * @param seconds  Seconds to set
	 */
	void setSeconds(int seconds);

	/**
	 * Sets the time shift hours.
	 *
	 * @param distHours  Time shift hours
	 */
	void setDistHours(int distHours);

	/**
	 * Sets the time shift minutes.
	 *
	 * @param distMinutes  Time shift minutes
	 */
	void setDistMinutes(int distMinutes);

	/**
	 * Extracts a dateTime from <c>text</c>.
	 *
	 * @param text				Text
	 * @param output			dateTime storage destination
	 * @return					Valid dateTime flag
	 * @since 1.0.0rc1
	 */
	static bool extractDateTime(XML_Char const * text,
			XspfDateTime * output);

};


} // namespace Xspf

#endif // XSPF_DATE_TIME_H
