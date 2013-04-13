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
 * @file XspfExtension.h
 * Interface of XspfExtension.
 */

#ifndef XSPF_EXTENSION_H
#define XSPF_EXTENSION_H


#include "XspfDefines.h"

namespace Xspf {


class XspfXmlFormatter;
class XspfExtensionWriter;
class XspfTrackWriter;
class XspfPropsWriter;
class XspfExtensionPrivate;


/**
 * Extends XspfTrack and XspfProps objects by arbitrary information.
 */
class XspfExtension {

private:
	/// @cond DOXYGEN_NON_API
	XspfExtensionPrivate * const d; ///< D pointer
	/// @endcond

protected:
	/**
	 * Creates a new XspfExtension object.
	 *
	 * @param applicationUri	Application URI
	 */
	XspfExtension(XML_Char const * applicationUri);

public:
	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtension(XspfExtension const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtension & operator=(XspfExtension const & source);

	/**
	 * Destroys this XspfExtension object and deletes all
	 * memory associated with it.
	 */
	virtual ~XspfExtension();

	/**
	 * Clones this extension.
	 *
	 * @return	A clone of this extension
	 */
	virtual XspfExtension * clone() const = 0;

	/**
	 * Returns the application URI.
	 *
	 * @return		Application URI
	 */
	XML_Char const * getApplicationUri() const;

	/**
	 * Creates a new writer that can write
	 * this extension instance
	 *
	 * @param output	Output formatter to use
	 * @param baseUri	Base URI to reduce URIs against
	 * @return			Extension writer for this
	 */
	virtual XspfExtensionWriter * newWriter(XspfXmlFormatter * output,
			XML_Char const * baseUri) const = 0;

protected:
	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

};


} // namespace Xspf

#endif // XSPF_EXTENSION_H
