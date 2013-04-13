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
 * @file XspfDataWriter.h
 * Interface of XspfDataWriter.
 */

#ifndef XSPF_DATA_WRITER_H
#define XSPF_DATA_WRITER_H


#include <xspf/XspfDefines.h>
#include <string>

namespace Xspf {


class XspfData;
class XspfXmlFormatter;
class XspfDataWriterPrivate;


/**
 * Writes a data object to an XML formatter.
 */
class XspfDataWriter {

private:
	/// @cond DOXYGEN_NON_API
	XspfDataWriterPrivate * const d; ///< D pointer
	/// @endcond

protected:
	/**
	 * Creates a new data writer.
	 */
	XspfDataWriter();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfDataWriter(XspfDataWriter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfDataWriter & operator=(XspfDataWriter const & source);

	/**
	 * Destroys this data writer.
	 */
	virtual ~XspfDataWriter();

	/**
	 * Assigns the data object to write.
	 *
	 * @param data	Data object to write
	 */
	void setData(XspfData const * data);

	/**
	 * Write a primitive element in one go.
	 * Primitives are elements with no nested content.
	 *
	 * @param name	Element name
	 * @param body	String body
	 */
	void writePrimitive(XML_Char const * name, XML_Char const * body);

	/**
	 * Write a primitive element in one go.
	 * Primitives are elements with no nested content.
	 *
	 * @param name	Element name
	 * @param body	Number body
	 */
	void writePrimitive(XML_Char const * name, int body);

	/**
	 * Writes the image property.
	 */
	void writeImage();

	/**
	 * Writes the info property.
	 */
	void writeInfo();

	/**
	 * Writes the annotation property.
	 */
	void writeAnnotation();

	/**
	 * Writes the creator property.
	 */
	void writeCreator();

	/**
	 * Writes the title property.
	 */
	void writeTitle();

	/**
	 * Writes the list of link pairs.
	 */
	void writeLinks();

	/**
	 * Writes the list of meta pairs.
	 */
	void writeMetas();

	/**
	 * Writes the list of extensions.
	 */
	void writeExtensions();

	/**
	 * Reduces an (usually) absolute URI to a relative one
	 * if possible according to the current base URI.
	 *
	 * @param sourceUri  URI to reduce
	 * @return           New'ed URI
	 */
	XML_Char * makeRelativeUri(XML_Char const * sourceUri) const;

	/**
	 * Gives access to the XML formatter in use.
	 *
	 * @return  XML formatter reference
	 */
	XspfXmlFormatter * & getOutput() const;

	/**
	 * Return the base URI.
	 *
	 * @return  Base URI, can be \c NULL
	 */
	XML_Char const * getBaseUri() const;

	/**
	 * Sets the base URI.
	 *
	 * @param baseUri  Base URI to set, can be \c NULL
	 */
	void setBaseUri(XML_Char const * baseUri);

};


}

#endif // XSPF_DATA_WRITER_H
