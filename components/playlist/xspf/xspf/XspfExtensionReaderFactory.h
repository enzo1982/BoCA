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
 * @file XspfExtensionReaderFactory.h
 * Interface of XspfExtensionReaderFactory.
 */

#ifndef XSPF_EXTENSION_READER_FACTROY_H
#define XSPF_EXTENSION_READER_FACTROY_H


#include "XspfDefines.h"
#include "XspfToolbox.h"
#include <map>

namespace Xspf {


class XspfExtensionReader;
class XspfReader;
class XspfExtensionReaderFactoryPrivate;


/**
 * Manages creation of XspfExtensionReader instances.
 * XspfExtensionReaders can be registered/unregistered dynamically.
 */
class XspfExtensionReaderFactory {

private:
	/// @cond DOXYGEN_NON_API
	XspfExtensionReaderFactoryPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new XspfExtensionReaderFactory object.
	 */
	XspfExtensionReaderFactory();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReaderFactory(const XspfExtensionReaderFactory
			& source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfExtensionReaderFactory & operator=(
			XspfExtensionReaderFactory const & source);

	/**
	 * Destroys this XspfExtensionReaderFactory object and deletes all
	 * memory associated with it.
	 */
	~XspfExtensionReaderFactory();

	/**
	 * Overwrites the registered reader for the given application URI.
	 * Pass NULL for the URI to make this the catch-all reader.
	 * The reader will be cloned internally so can safely delete
	 * the instance passed for registration.
	 *
	 * @param example		Representative for the extension reader cleass
	 * @param triggerUri	Application URI associate, must not be NULL
	 */
	void registerPlaylistExtensionReader(XspfExtensionReader const * example,
			XML_Char const * triggerUri);

	/**
	 * Overwrites the registered reader for the given application URI.
	 * Pass NULL for the URI to make this the catch-all reader.
	 * The reader will be cloned internally so can safely delete
	 * the instance passed for registration.
	 *
	 * @param example		Representative for the extension reader cleass
	 * @param triggerUri	Application URI associate, must not be NULL
	 */
	void registerTrackExtensionReader(XspfExtensionReader const * example,
			XML_Char const * triggerUri);

	/**
	 * Unregisteres the given application URI.
	 * NOTE: This URI will still be handled if a catch-all
	 * handler has been set.
	 *
	 * @param triggerUri	Application URI to unregister
	 */
	void unregisterPlaylistExtensionReader(
			XML_Char const * triggerUri);

	/**
	 * Unregisteres the given application URI.
	 * NOTE: This URI will still be handled if a catch-all
	 * handler has been set.
	 *
	 * @param triggerUri	Application URI to unregister
	 */
	void unregisterTrackExtensionReader(
			XML_Char const * triggerUri);

	/**
	 * Creates a new XspfExtensionReader whose type was
	 * registered for this application URI.
	 *
	 * @param applicationUri	Application URI
	 * @param reader			XspfReader for the extension reader
	 * @return					New playlist extension reader
	 */
	XspfExtensionReader * newPlaylistExtensionReader(
			XML_Char const * applicationUri, XspfReader * reader);

	/**
	 * Creates a new XspfExtensionReader whose type was
	 * registered for this application URI.
	 *
	 * @param applicationUri	Application URI
	 * @param reader			XspfReader for the extension reader
	 * @return					New track extension reader
	 */
	XspfExtensionReader * newTrackExtensionReader(
			XML_Char const * applicationUri, XspfReader * reader);

private:
	/**
	 * Overwrites the registered reader for the given application URI.
	 * Pass NULL for the URI to make this the catch-all reader.
	 * The reader will be cloned internally so can safely delete
	 * the instance passed for registration.
	 *
	 * @param container		Container to unregister from
	 * @param catchAll		Catch-all slot to modifiy
	 * @param example		Reader class representative
	 * @param triggerUri	Application URI to unregister
	 */
	void registerReader(std::map<XML_Char const *,
			XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & container,
			XspfExtensionReader const * & catchAll,
			XspfExtensionReader const * example,
			XML_Char const * triggerUri);

	/**
	 * Unregisteres the given application URI.
	 *
	 * @param container		Container to unregister from
	 * @param catchAll		Catch-all slot to modifiy
	 * @param triggerUri	Application URI to unregister
	 */
	void unregisterReader(std::map<XML_Char const *,
			XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & container,
			XspfExtensionReader const * & catchAll,
			XML_Char const * triggerUri);

	/**
	 * Creates a new XspfExtensionReader whose type was
	 * registered for this application URI.
	 *
	 * @param container			Container to use
	 * @param catchAll			Catch-all slot to use
	 * @param applicationUri	Application URI
	 * @param reader			XspfReader for the extension reader
	 * @return					New extension reader
	 */
	XspfExtensionReader * newReader(std::map<XML_Char const *,
			XspfExtensionReader const *,
			Toolbox::XspfStringCompare> & container,
			XspfExtensionReader const * catchAll,
			XML_Char const * applicationUri,
			XspfReader * reader);

};


}

#endif // XSPF_EXTENSION_READER_FACTROY_H
