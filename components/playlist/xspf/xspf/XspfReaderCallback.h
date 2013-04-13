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
 * @file XspfReaderCallback.h
 * Interface of XspfReaderCallback.
 */

#ifndef XSPF_READER_CALLBACK_H
#define XSPF_READER_CALLBACK_H

#include "XspfDefines.h"

namespace Xspf {


class XspfProps;
class XspfTrack;
class XspfReader;
class XspfReaderCallbackPrivate;


/**
 * Is called by a XspfReader when new pieces of information
 * become available.
 */
class XspfReaderCallback {

public:
	/**
	 * Creates a new reader callback.
	 */
	XspfReaderCallback();

	/**
	 * Destroys this reader callback.
	 */
	virtual ~XspfReaderCallback();

protected:
	/**
	 * Is called when a new, complete track is available.
	 * Deleting <c>track</c> is up to you.
	 *
	 * @param track		New track, never <c>NULL</c>
	 */
	virtual void addTrack(XspfTrack * track);

	/**
	 * Is called when all playlist properties are available.
	 * Deleting <c>props</c> is up to you.
	 *
	 * @param props		Playlist properties, never <c>NULL</c>
	 */
	virtual void setProps(XspfProps * props);

	/**
	 * Is called when a fatal error occurs. This includes
	 * errors on XML level which cannot be skipped due to the
	 * nature of XML.
	 *
	 * @param line         Line of the input containing the error, can be < 1
	 * @param column       Column of the input containing the error, can be < 1
	 * @param errorCode    Code identifying the error
	 * @param description  Description of the error
	 * @since 1.0.0.rc3
	 */
	virtual void notifyFatalError(int line, int column, int errorCode,
			XML_Char const * description);

	/**
	 * Is called when a XSPF content error occurs. Return \c true to
	 * continue parsing or \c to abort.
	 * Please note that \p line and \p column might not be precise.
	 *
	 * @param line         Line of the input containing the error
	 * @param column       Column of the input containing the error
	 * @param errorCode    Code identifying the error
	 * @param description  Description of the error
	 * @return             Continue parsing flag
	 * @since 1.0.0rc1
	 */
	virtual bool handleError(int line, int column, int errorCode,
			XML_Char const * description);

	/**
	 * Is called when a warning occurs. Return \c true to continue parsing
	 * or \c to abort. Please note that the precision of the \p line and \p
	 * column values in part is the responsibility of Expat.
	 *
	 * @param line         Line of the input producing the warning
	 * @param column       Column of the input producing the warning
	 * @param warningCode  Code identifying the warning
	 * @param description  Description of the warning
	 * @return             Continue parsing flag
	 * @since 1.0.0rc1
	 */
	virtual bool handleWarning(int line, int column, int warningCode,
			XML_Char const * description);

	/**
	 * Notifies this reader callback, that reading this
	 * file has finished successfully. 'Success' here means
	 * that either no errors occured at all or that all errors
	 * have been ignored/skipped.
	 *
	 * @since 1.0.0.rc3
	 */
	virtual void notifySuccess();

	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

	friend class XspfReader;

};


} // namespace Xspf

#endif // XSPF_READER_CALLBACK_H
