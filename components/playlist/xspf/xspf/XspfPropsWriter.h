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
 * @file XspfPropsWriter.h
 * Interface of XspfPropsWriter.
 */

#ifndef XSPF_PROPS_WRITER_H
#define XSPF_PROPS_WRITER_H


#include "XspfDataWriter.h"

namespace Xspf {


class XspfXmlFormatter;
class XspfProps;
class XspfWriter;
class XspfPropsWriterPrivate;


/**
 * Writes playlist properties to an XML formatter.
 */
class XspfPropsWriter : public XspfDataWriter {

private:
	/// @cond DOXYGEN_NON_API
	XspfPropsWriterPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new playlist property writer.
	 */
	XspfPropsWriter();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsWriter(XspfPropsWriter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfPropsWriter & operator=(XspfPropsWriter const & source);

	/**
	 * Destroys this playlist property writer.
	 */
	~XspfPropsWriter();

protected:
	/**
	 * Initializes the playlist properties writer.
	 * Must be called before writing.
	 *
	 * @param output	Output formatter to write to
	 * @param baseUri	Absolute base URI to reduce against.
	 * @param embedBase	Embeds base URI as xml:base in root
	 *					node if true or not if false
	 */
	void init(XspfXmlFormatter & output,
			XML_Char const * baseUri, bool embedBase);

	/**
	 * Opens the <i>playlist</i> tag and writes all playlist
	 * properties not handled by writeStartTracklist().
	 */
	void writeStartPlaylist();

	/**
	 * Writes all playlist properties not handled by
	 * writeStartPlaylist() and opens the <i>tracklist</i> tag.
	 *
	 * @param trackListEmpty	Tracklist empty flag
	 */
	void writeStartTracklist(bool trackListEmpty);

	/**
	 * Closes the <i>tracklist</i> tag.
	 */
	void writeEndTracklist();

	/**
	 * Closes the <i>playlist</i> tag.
	 */
	void writeEndPlaylist();

	/**
	 * Writes the list of attributions.
	 */
	void writeAttribution();

	/**
	 * Writes the date property.
	 */
	void writeDate();

	/**
	 * Writes the identifier property.
	 */
	void writeIdentifier();

	/**
	 * Writes the license property.
	 */
	void writeLicense();

	/**
	 * Writes the location property.
	 */
	void writeLocation();

	/**
	 * Writes the closing playlist tag.
	 */
	void writePlaylistClose();

	/**
	 * Writes the opening playlist tag.
	 */
	void writePlaylistOpen();

	/**
	 * Writes the closing trackList tag.
	 */
	void writeTrackListClose();

	/**
	 * Writes the opening trackList tag.
	 */
	void writeTrackListOpen();

	// TODO
	void setProps(XspfProps const * props);

private:
	/**
	 * Preregisters an XML namespace.
	 *
	 * @param uri				Namespace URI
	 * @param prefixSuggestion	Suggested prefix
	 * @return                  Success flag
	 */
	bool registerNamespace(XML_Char const * uri,
			XML_Char const * prefixSuggestion);

	friend class XspfWriter;

};


}

#endif // XSPF_PROPS_WRITER_H
