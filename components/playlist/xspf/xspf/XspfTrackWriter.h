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
 * @file XspfTrackWriter.h
 * Interface of XspfTrackWriter.
 */

#ifndef XSPF_TRACK_WRITER_H
#define XSPF_TRACK_WRITER_H


#include "XspfDataWriter.h"

namespace Xspf {


class XspfXmlFormatter;
class XspfTrack;
class XspfWriter;
class XspfTrackWriterPrivate;


/**
 * Writes a track to an XML formatter.
 */
class XspfTrackWriter : public XspfDataWriter {

private:
	/// @cond DOXYGEN_NON_API
	XspfTrackWriterPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new track writer.
	 */
	XspfTrackWriter();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrackWriter(XspfTrackWriter const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfTrackWriter & operator=(XspfTrackWriter const & source);

	/**
	 * Destroys this track writer.
	 */
	~XspfTrackWriter();

	/**
	 * Sets the track to write.
	 *
	 * @param track		Track to write
	 */
	void setTrack(XspfTrack const * track);

protected:
	/**
	 * Initializes the track writer.
	 * Must be called before writing.
	 *
	 * @param output	Output formatter to write to
	 * @param version	XSPF version to produce
	 * @param baseUri	Absolute base URI to reduce against.
	 */
	void init(XspfXmlFormatter & output, int version,
			XML_Char const * baseUri);

	/**
	 * Writes this track to the formatter.
	 */
	void write();

	/**
	 * Writes the album property.
	 */
	void writeAlbum();

	/**
	 * Writes the duration property.
	 */
	void writeDuration();

	/**
	 * Writes the list of identifiers.
	 */
	void writeIdentifiers();

	/**
	 * Writes the list of locations.
	 */
	void writeLocations();

	/**
	 * Writes the closing track tag.
	 */
	void writeTrackClose();

	/**
	 * Writes the opening track tag.
	 */
	void writeTrackOpen();

	/**
	 * Writes the trackNum property.
	 */
	void writeTrackNum();

	friend class XspfWriter;

};


}

#endif // XSPF_TRACK_WRITER_H
