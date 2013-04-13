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
 * @file XspfExtensionWriter.h
 * Interface of XspfExtensionWriter.
 */

#ifndef XSPF_EXTENSION_WRITER_H
#define XSPF_EXTENSION_WRITER_H


#include "XspfDefines.h"
#include <string>

namespace Xspf {


class XspfExtension;
class XspfXmlFormatter;
class XspfExtensionWriterPrivate;


/**
 * Writes a XspfExtension to a XspfXmlFormatter.
 */
class XspfExtensionWriter {

private:
	/// @cond DOXYGEN_NON_API
	XspfExtensionWriterPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new XspfExtensionWriter object.
	 *
	 * @param extension  Extension to write
	 * @param output     Output formatter to write to
	 * @param baseUri    Absolute base URI to reduce URIs against,
	 *                   is \c NULL if only absolute URIs are wanted
	 */
	XspfExtensionWriter(XspfExtension const * extension,
			XspfXmlFormatter * output, XML_Char const * baseUri);

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionWriter(XspfExtensionWriter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionWriter & operator=(XspfExtensionWriter const & source);

	/**
	 * Destroys this XspfExtensionWriter object and deletes all
	 * memory associated with it.
	 */
	virtual ~XspfExtensionWriter();

	/**
	 * Writes the associated extension to the output formater.
	 */
	void write();

private:
	/**
	 * Writes the opening extension tag also registering all
	 * previously unknown namespaces.
	 *
	 * @param nsRegs  NULL-terminated list of namespace registrations (uri/prefix pairs)
	 */
	void writeExtensionStart(XML_Char const * const * nsRegs);

protected:
	/**
	 * Writes the extension body.
	 */
	virtual void writeExtensionBody() = 0;

private:
	/**
	 * Writes the closing extension tag.
	 */
	void writeExtensionStop();

protected:
	/**
	 * Returns a list of the namespaces used inside
	 * the extension body. Moreprecisely this
	 * is an alternating NULL-terminated list
	 * of URI/prefix pairs.
	 *
	 * @return	List of URI/prefix pairs
	 */
	virtual XML_Char const * const * getNamespaceRegs();

	/**
	 * Returns the extension to be written.
	 *
	 * @return  Extension
	 */
	XspfExtension const * getExtension();

	/**
	 * Gives access to the XML formatter in use.
	 *
	 * @return  XML formatter reference
	 */
	XspfXmlFormatter * & getOutput();

	/**
	 * Returns the base URI in use.
	 *
	 * @return  Base URI, can be \c NULL
	 */
	XML_Char const * getBaseUri() const;

	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

};


} // namespace Xspf

#endif // XSPF_EXTENSION_WRITER_H
