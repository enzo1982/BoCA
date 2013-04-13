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
 * @file XspfXmlFormatter.h
 * Interface of XspfXmlFormatter.
 */

#ifndef XSPF_XML_FORMATTER_H
#define XSPF_XML_FORMATTER_H


#include "XspfDefines.h"
#include <sstream>

namespace Xspf {


class XspfWriter;


/// @cond DOXYGEN_NON_API

/**
 * Holds information necessary to undo a namespace registration
 */
struct XspfNamespaceRegistrationUndo {
	int level; ///< Level, the namespace was registered on. Root is level 0.
	XML_Char const * uri; ///< Namespace URI

	/**
	 * Creates a new undo entry.
	 *
	 * @param level	Nesting level, 0 for root element
	 * @param uri	Namespace URI
	 */
	XspfNamespaceRegistrationUndo(int level, XML_Char const * uri)
			: level(level), uri(uri) {

	}

};

/// @endcond


class XspfXmlFormatterPrivate;


/**
 * Outputs XML.
 */
class XspfXmlFormatter {

private:
	/// @cond DOXYGEN_NON_API
	XspfXmlFormatterPrivate * const d; ///< D pointer
	/// @endcond

protected:
	/**
	 * Creates a new formatter.
	 */
	XspfXmlFormatter();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfXmlFormatter(XspfXmlFormatter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfXmlFormatter & operator=(XspfXmlFormatter const & source);

	/**
	 * Destroys this formatter and deletes all memory
	 * associated with it.
	 */
	virtual ~XspfXmlFormatter();

	/**
	 * Writes the XML declaration.
	 */
	virtual void writeXmlDeclaration();

private:
	/**
	 * Looks up the associated prefix for namespace
	 * <code>nsUri</code>.
	 *
	 * @param nsUri		Namespace URI
	 * @return			Registered namespace prefix
	 */
	XML_Char const * getPrefix(XML_Char const * nsUri) const;

	/**
	 * Looks up the associated prefix for namespace <code>nsUri</code>
	 * and makes a full element name of that and <code>localName</code>.
	 *
	 * @param nsUri			Namespace URI
	 * @param localName		Local element name
	 * @return				Full element name, delete on your own
	 */
	XML_Char * makeFullName(XML_Char const * nsUri,
			XML_Char const * localName) const;

	/**
	 * Unregisteres all namespace URIs that were registered
	 * on a higher level and are thus not needed any more.
	 */
	void cleanupNamespaceRegs();

	/**
	 * Finds an unmapped namespace prefix based on the
	 * given suggestion and maps the namespace URI to it.
	 *
	 * @param uri				Namespace URI
	 * @param prefixSuggestion	Suggested prefix
	 */
	bool registerNamespace(XML_Char const * uri,
			XML_Char const * prefixSuggestion);

public:
	/**
	 * Associate the formatter with an output accumulator.
	 *
	 * @param output	Output accumulator
	 */
	void setOutput(std::basic_ostringstream<XML_Char> & output);

	/**
	 * Opens the tag <code>localname</code> from namespace
	 * <code>ns</code>.
	 *
	 * @param ns			Namespace URI
	 * @param localName		Local element name
	 * @param atts			NULL-terminated list of attributes (key/value pairs)
	 * @param nsRegs		NULL-terminated list of namespace registrations (uri/prefix pairs)
	 */
	void writeStart(XML_Char const * ns, XML_Char const * localName,
			XML_Char const * const * atts, XML_Char const * const * nsRegs = NULL);

	/**
	 * Closes the tag <code>localname</code> from namespace
	 * <code>ns</code>.
	 *
	 * @param ns			Namespace URI
	 * @param localName		Local element name
	 */
	void writeEnd(XML_Char const * ns, XML_Char const * localName);

	/**
	 * Opens the tag <code>localname</code> from the XSPF namespace.
	 *
	 * @param localName		Local element name
	 * @param atts			NULL-terminated list of attributes (key/value pairs)
	 * @param nsRegs		NULL-terminated list of namespace registrations (uri/prefix pairs)
	 */
	void writeHomeStart(XML_Char const * localName,
			XML_Char const * const * atts, XML_Char const * const * nsRegs = NULL);

	/**
	 * Closes the tag <code>localname</code> from the XSPF namespace.
	 *
	 * @param localName		Local element name
	 */
	void writeHomeEnd(XML_Char const * localName);

protected:
	/**
	 * Opens the tag <c>name</c> and adds the attributes <c>atts</c>.
	 * <c>atts</c> is not <c>NULL</c> and <c>*atts</c> is an alternating
	 * list of attribute keys and values. Its length is uneven and the last
	 * entry is <c>NULL</c>.
	 *
	 * @param name	Name of the tag to open
	 * @param atts	Alternating list of attribute key and value
	 */
	virtual void writeStart(XML_Char const * name,
			XML_Char const * const * atts) = 0;

	/**
	 * Closes the tag <c>name</c>.
	 *
	 * @param name	Name of the tag to close
	 */
	virtual void writeEnd(XML_Char const * name) = 0;

public:
	/**
	 * Adds element content.
	 *
	 * @param text	Text content
	 */
	virtual void writeBody(XML_Char const * text) = 0;

	/**
	 * Adds element content.
	 *
	 * @param number	Integer content
	 */
	virtual void writeBody(int number) = 0;

	/**
	 * Writes well-formed character data to the output accumulator.
	 *
	 * @param data	Character data
	 */
	void writeCharacterData(XML_Char const * data);

	static XML_Char const * const namespaceKey; ///< Namespace key pointer

protected:
	/**
	 * Returns the output stream in use.
	 */
	std::basic_ostringstream<XML_Char> * & getOutput();

	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

	friend class XspfWriter; // allow access to registerNamespace

};


} // namespace Xspf

#endif // XSPF_XML_FORMATTER_H
