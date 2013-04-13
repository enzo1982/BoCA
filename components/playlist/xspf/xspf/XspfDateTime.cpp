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
 * @file XspfDateTime.cpp
 * Implementation of XspfDateTime.
 */

#include <xspf/XspfDateTime.h>
#include <cstring>

#if defined(sun) || defined(__sun)
# include <strings.h> // for strncmp()
#endif


namespace Xspf {


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfDateTime.
 */
class XspfDateTimePrivate {

	friend class XspfDateTime;

	int year; ///< Year [-9999..+9999] but not zero
	int month; ///< Month [1..12]
	int day; ///< Day [1..31]
	int hour; ///< Hour [0..23]
	int minutes; ///< Minutes [0..59]
	int seconds; ///< Seconds [0..59]
	int distHours; ///< Time shift hours [-14..+14]
	int distMinutes; ///< Time shift minutes [-59..+59]

	/**
	 * Creates a new D object.
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
	XspfDateTimePrivate(int year, int month, int day, int hour,
			int minutes, int seconds, int distHours, int distMinutes)
			: year(year),
			month(month),
			day(day),
			hour(hour),
			minutes(minutes),
			seconds(seconds),
			distHours(distHours),
			distMinutes(distMinutes) {

	}

	/**
	 * Destroys this D object.
	 */
	~XspfDateTimePrivate() {

	}

};

/// @endcond


XspfDateTime::XspfDateTime(int year, int month, int day,
		int hour, int minutes, int seconds, int distHours,
		int distMinutes)
		: d(new XspfDateTimePrivate(year, month, day, hour,
			minutes, seconds, distHours, distMinutes)) {

}


XspfDateTime::XspfDateTime()
		: d(new XspfDateTimePrivate(0, 0, 0, -1, -1, -1, 0, 0)) {

}


XspfDateTime::XspfDateTime(XspfDateTime const & source)
		: d(new XspfDateTimePrivate(*(source.d))) {

}


XspfDateTime & XspfDateTime::operator=(XspfDateTime const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfDateTime::~XspfDateTime() {
	delete this->d;
}


XspfDateTime * XspfDateTime::clone() const {
	return new XspfDateTime(this->d->year, this->d->month, this->d->day,
			this->d->hour, this->d->minutes, this->d->seconds, this->d->distHours,
			this->d->distMinutes);
}


int XspfDateTime::getYear() const {
	return this->d->year;
}


int XspfDateTime::getMonth() const {
	return this->d->month;
}


int XspfDateTime::getDay() const {
	return this->d->day;
}


int XspfDateTime::getHour() const {
	return this->d->hour;
}


int XspfDateTime::getMinutes() const {
	return this->d->minutes;
}


int XspfDateTime::getSeconds() const {
	return this->d->seconds;
}


int XspfDateTime::getDistHours() const {
	return this->d->distHours;
}


int XspfDateTime::getDistMinutes() const {
	return this->d->distMinutes;
}


void XspfDateTime::setYear(int year) {
	this->d->year = year;
}


void XspfDateTime::setMonth(int month) {
	this->d->month = month;
}


void XspfDateTime::setDay(int day) {
	this->d->day = day;
}


void XspfDateTime::setHour(int hour) {
	this->d->hour = hour;
}


void XspfDateTime::setMinutes(int minutes) {
	this->d->minutes = minutes;
}


void XspfDateTime::setSeconds(int seconds) {
	this->d->seconds = seconds;
}


void XspfDateTime::setDistHours(int distHours) {
	this->d->distHours = distHours;
}


void XspfDateTime::setDistMinutes(int distMinutes) {
	this->d->distMinutes = distMinutes;
}


namespace {


/**
 * Calls atoi() on a limited number of characters.
 *
 * @param text	Text
 * @param len	Number of characters to read
 * @return		Result of atoi()
 * @since 1.0.0rc1
 */
int PORT_ANTOI(XML_Char const * text, int len) {
	XML_Char * final = new XML_Char[len + 1];
	::PORT_STRNCPY(final, text, len);
	final[len] = _PT('\0');
	int const res = ::PORT_ATOI(final);
	delete [] final;
	return res;
}


} // anon namespace


/*static*/ bool
XspfDateTime::extractDateTime(XML_Char const * text,
		XspfDateTime * output) {
	// http://www.w3.org/TR/xmlschema-2/#dateTime-lexical-representation
	// '-'? yyyy '-' mm '-' dd 'T' hh ':' mm ':' ss ('.' s+)? (zzzzzz)?

	// '-'?
	bool const leadingMinus = (*text == _PT('-'));
	if (leadingMinus) {
		text++;
	}

	// yyyy
	if ((::PORT_STRNCMP(text, _PT("0001"), 4) < 0) || (::PORT_STRNCMP(_PT("9999"), text, 4) < 0)) {
		return false;
	}
	int const year = PORT_ANTOI(text, 4);
	output->setYear(year);
	text += 4;

	// '-' mm
	if ((::PORT_STRNCMP(text, _PT("-01"), 3) < 0) || (::PORT_STRNCMP(_PT("-12"), text, 3) < 0)) {
		return false;
	}
	int const month = PORT_ANTOI(text + 1, 2);
	output->setMonth(month);
	text += 3;

	// '-' dd
	if ((::PORT_STRNCMP(text, _PT("-01"), 3) < 0) || (::PORT_STRNCMP(_PT("-31"), text, 3) < 0)) {
		return false;
	}
	int const day = PORT_ANTOI(text + 1, 2);
	output->setDay(day);
	text += 3;

	// Month specific day check
	switch (month) {
	case 2:
		switch (day) {
		case 31:
		case 30:
			return false;

		case 29:
			if (((year % 400) != 0) && (((year % 4) != 0)
					|| ((year % 100) == 0))) {
				// Not a leap year
				return false;
			}
			break;

		case 28:
			break;
		}
		break;

	case 4:
	case 6:
	case 9:
	case 11:
		if (day > 30) {
			return false;
		}
		break;

	}

	// 'T' hh
	if ((::PORT_STRNCMP(text, _PT("T00"), 3) < 0) || (::PORT_STRNCMP(_PT("T23"), text, 3) < 0)) {
		return false;
	}
	output->setHour(PORT_ANTOI(text + 1, 2));
	text += 3;

	// ':' mm
	if ((::PORT_STRNCMP(text, _PT(":00"), 3) < 0) || (::PORT_STRNCMP(_PT(":59"), text, 3) < 0)) {
		return false;
	}
	output->setMinutes(PORT_ANTOI(text + 1, 2));
	text += 3;

	// ':' ss
	if ((::PORT_STRNCMP(text, _PT(":00"), 2) < 0) || (::PORT_STRNCMP(_PT(":59"), text, 2) < 0)) {
		return false;
	}
	output->setSeconds(PORT_ANTOI(text + 1, 2));
	text += 3;

	// ('.' s+)?
	if (*text == _PT('.')) {
		text++;
		int counter = 0;
		while ((*text >= _PT('0')) && (*text <= _PT('9'))) {
			text++;
			counter++;
		}
		if (counter == 0) {
			return false;
		} else {
			// The fractional second string, if present, must not end in '0'
			if (*(text - 1) == _PT('0')) {
				return false;
			}
		}
	}

	// (zzzzzz)? := (('+' | '-') hh ':' mm) | 'Z'
	XML_Char const * const timeZoneStart = text;
	switch (*text) {
	case _PT('+'):
	case _PT('-'):
		{
			text++;
			if ((::PORT_STRNCMP(text, _PT("00"), 2) < 0) || (::PORT_STRNCMP(_PT("14"), text, 2) < 0)) {
				return false;
			}
			int const distHours = PORT_ANTOI(text, 2);
			output->setDistHours(distHours);
			text += 2;

			if ((::PORT_STRNCMP(text, _PT(":00"), 3) < 0) || (::PORT_STRNCMP(_PT(":59"), text, 3) < 0)) {
				return false;
			}
			int const distMinutes = PORT_ANTOI(text + 1, 2);
			output->setDistMinutes(distMinutes);
			if ((distHours == 14) && (distMinutes != 0)) {
				return false;
			}
			text += 3;

			if (*text != _PT('\0')) {
				return false;
			}

			if (*timeZoneStart == _PT('-')) {
				output->setDistHours(-distHours);
				output->setDistMinutes(-distMinutes);
			}
		}
		break;

	case _PT('Z'):
		text++;
		if (*text != _PT('\0')) {
			return false;
		}
		// NO BREAK

	case _PT('\0'):
		output->setDistHours(0);
		output->setDistMinutes(0);
		break;

	default:
		return false;

	}

	return true;
}


} // namespace Xspf
