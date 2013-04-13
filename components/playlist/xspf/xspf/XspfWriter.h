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
 * @file XspfWriter.h
 * Interface of XspfWriter.
 */

#ifndef XSPF_WRITER_H
#define XSPF_WRITER_H

#include "XspfDefines.h"

namespace Xspf {


class XspfXmlFormatter;
class XspfTrack;
class XspfProps;
class XspfWriterPrivate;


/**
 * Specifies the result of a write operation.
 */
enum XspfWriterReturnCode {
	XSPF_WRITER_SUCCESS, ///< Everything fine

	XSPF_WRITER_ERROR_OPENING, ///< File could not be opened
	XSPF_WRITER_ERROR_BASE_URI_USELESS ///< Given base URI is not a valid absolute URI
};


/**
 * Writes a playlist as XSPF to a file.
 * XSPF version 0 and 1 are supported.
 */
class XspfWriter {

private:
	/// @cond DOXYGEN_NON_API
	XspfWriterPrivate * const d; ///< D pointer
	/// @endcond

private:
	/**
	 * Creates a new playlist writer.
	 *
	 * @param formatter		XML formatter to use
	 * @param baseUri		Absolute base URI to reduce against,
     *                      pass \c NULL to keep all URIs absolute
	 * @param embedBase		Embeds base URI as xml:base attribute in
	 *						root node if true or not if false
	 * @since 1.0.0
	 */
	XspfWriter(XspfXmlFormatter & formatter, XML_Char const * baseUri,
			bool embedBase);

public:
	/**
	 * Creates a new playlist writer.
	 *
	 * @param formatter		XML formatter to use
	 * @param baseUri		Absolute base URI to reduce against,
     *                      pass \c NULL to keep all URIs absolute
	 * @param embedBase		Embeds base URI as xml:base in root
	 *						node if true or not if false
	 * @param errorCode     Place to write error code or
	 *                      \c XSPF_WRITER_SUCCESS to.
	 * @return              \c NULL on error or a new'ed XspfWriter
	 *                      that you have to delete manually later.
	 * @since 1.0.0
	 */
	static XspfWriter * makeWriter(XspfXmlFormatter & formatter,
			XML_Char const * baseUri, bool embedBase = false,
			int * errorCode = NULL);

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfWriter(XspfWriter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfWriter & operator=(XspfWriter const & source);

	/**
	 * Frees all own memory.
	 */
	~XspfWriter();

	/**
	 * Pre-registers a namespace so it can still
	 * appear in the root element.
	 *
	 * @attention
	 * registerNamespace() must be called before any calls
	 * to setProps() or addTrack().
	 *
	 * @param uri				Namespace URI
	 * @param prefixSuggestion	Suggested prefix
	 * @return                  Success flag
	 */
	bool registerNamespace(XML_Char const * uri,
			XML_Char const * prefixSuggestion);

	/**
	 * Appends a track to the playlist.
	 * The \p track instance passed can be safely deleted
	 * after the call returns.
	 *
	 * @param track  Track to append
	 * @return       Success flag
	 * @since 1.0.0
	 */
	bool addTrack(XspfTrack const * track);

	/**
	 * Appends a track to the playlist.
	 * The \p track instance passed can be safely deleted
	 * after the call returns.
	 *
	 * @param track  Track to append
	 * @return       Success flag
	 * @since 1.0.0
	 */
	bool addTrack(XspfTrack const & track);

	/**
	 * Sets playlist-wide properties of the playlist to be written
	 * including the version of XSPF to be used.
	 * The \p props instance passed can be safely deleted
	 * after the call returns.
	 *
	 * @attention
	 * setProps() must be called before any calls to addTrack().
	 *
	 * @param props  Playlist properties to set, can be NULL
	 * @return       Success flag
	 * @since 1.0.0
	 */
	bool setProps(XspfProps const * props);

	/**
	 * Sets playlist-wide properties of the playlist to be written
	 * including the version of XSPF to be used.
	 * The \p props instance passed can be safely deleted
	 * after the call returns.
	 *
	 * @attention
	 * setProps() must be called before any calls to addTrack().
	 *
	 * @param props  Playlist properties to set, can be NULL
	 * @return       Success flag
	 * @since 1.0.0
	 */
	bool setProps(XspfProps const & props);

	/**
	 * Finalizes the playlist and writes it to a file.
	 * You can call this method several times to write
	 * the same playlist to several files but you cannot
	 * add new tracks anymore. Call reset() to start over.
	 *
	 * @param filename	Filename of the file to write to
	 * @return			Error code
	 */
	int writeFile(XML_Char const * filename);

	/**
	 * Finalizes the playlist and writes it to a block of memory.
	 * You can call this method several times to write
	 * the same playlist to several files but you cannot
	 * add new tracks anymore. Call reset() to start over.
	 *
	 * @param memory	Reference to output memory block, delete[] on your own
	 * @param numBytes	Size of the memory block in bytes
	 * @return			Error code
	 */
	int writeMemory(char * & memory, int & numBytes);

	/**
	 * Clears all previously added tracks and makes the writer
	 * reusable by another playlist.
	 *
	 * @param formatter		XML formatter to use
	 * @param baseUri		Absolute base URI to reduce against.
	 * @param embedBase		Embeds base URI as xml:base in root
	 *						node if true or not if false
	 * @since 1.0.0
	 */
	void reset(XspfXmlFormatter & formatter,
			XML_Char const * baseUri, bool embedBase = false);

private:
	/**
	 * Does work common to all writing modes.
	 */
	void onBeforeWrite();

public:
	/// Base URI is embedded into the document
	static bool const EMBED_AS_XML_BASE;

	/// Base URI is not embedded into the document
	static bool const NO_XML_BASE;
};


}


#endif // XSPF_WRITER_H
