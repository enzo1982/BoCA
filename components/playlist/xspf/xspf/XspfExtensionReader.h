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
 * @file XspfExtensionReader.h
 * Interface of XspfExtensionReader.
 */

#ifndef XSPF_EXTENSION_READER_H
#define XSPF_EXTENSION_READER_H


#include "XspfDefines.h"
#include <string>

namespace Xspf {


template <class T> class XspfStack;
class XspfReader;
class XspfExtension;
class XspfExtensionReaderPrivate;
class XspfExtensionReaderFactoryPrivate;


/**
 * Provides the base for specific extention readers to derive from.
 */
class XspfExtensionReader {

	friend class XspfReader;
	friend class XspfReaderPrivate;
	friend class XspfExtensionReaderFactory;
	friend class XspfExtensionReaderFactoryPrivate;

private:
	/// @cond DOXYGEN_NON_API
	XspfExtensionReaderPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new XspfExtensionReader object.
	 *
	 * @param reader  XspfReader to interact with
	 */
	XspfExtensionReader(XspfReader * reader);

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReader(XspfExtensionReader const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReader & operator=(XspfExtensionReader const & source);

	/**
	 * Destroys this XspfExtensionReader object and deletes all
	 * memory associated with it.
	 */
	virtual ~XspfExtensionReader();

protected:
	/**
	 * Passes error handling to the parent XspfReader.
	 *
	 * @param code  Error code
	 * @param text  Error description
	 */
	bool handleError(int code, XML_Char const * text);

	/**
	 * Passes error handling to the parent XspfReader.
	 *
	 * @param code    Error code
	 * @param format  Error description format string containg <c>%s</c>
	 * @param param   Text parameter to insert for <c>%s</c>
	 */
	bool handleError(int code, XML_Char const * format, XML_Char const * param);

	/**
	 * Gives access to the element stack in use.
	 *
	 * @return  Element stack reference
	 */
	XspfStack<unsigned int> & getElementStack() const;

	/**
	 * Gives access to the Base URI stack in use.
	 *
	 * @return  Base URI stack reference
	 */
	XspfStack<std::basic_string<XML_Char> > & getBaseUriStack() const;

	/**
	 * Passes xml:base handling to the parent XspfReader.
	 *
	 * @param xmlBase  Value of xml:base attribute
	 * @return         Continue parsing flag
	 */
	bool handleXmlBaseAttribute(XML_Char const * xmlBase);

protected:
	/**
	 * Handles tag opening inside an extension including
	 * the extension tag itself.
	 *
	 * @param fullName  Full tag name ("<namespace_uri> <localname>")
	 * @param atts      Alternating list of attribute keys and values
	 * @return          Continue parsing flag
	 */
	virtual bool handleExtensionStart(XML_Char const * fullName,
			XML_Char const ** atts) = 0;

	/**
	 * Handles tag closing inside an extension including
	 * the extension tag itself.
	 *
	 * @param fullName  Full tag name ("<namespace_uri> <localname>")
	 * @return          Continue parsing flag
	 */
	virtual bool handleExtensionEnd(XML_Char const * fullName) = 0;

	/**
	 * Handles element content.
	 *
	 * @param s     Text content
	 * @param len   Characters allowed to read
	 */
	virtual bool handleExtensionCharacters(XML_Char const * s, int len) = 0;

	/**
	 * Makes a XspfExtension of the data collected.
	 *
	 * @return  New built extension
	 */
	virtual XspfExtension * wrap() = 0;

	/**
	 * Creates new XspfExtensionReader of the very same
	 * type as this reader.
	 *
	 * @param reader  XspfReader to interact with, must not be NULL
	 * @return        A new extension reader of the same type.
	 */
	virtual XspfExtensionReader * createBrother(XspfReader * reader) const = 0;

private:
	/**
	 * Creates new XspfExtensionReader of the very same
	 * type as this reader that will work with the same
	 * XspfReader as this instance.
	 *
	 * @return  A new extension reader of the same type.
	 */
	XspfExtensionReader * createBrother() const;

protected:
	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

};


} // namespace Xspf

#endif // XSPF_EXTENSION_READER_H
