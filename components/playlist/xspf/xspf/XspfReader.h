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
 * @file XspfReader.h
 * Interface of XspfReader.
 */

#ifndef XSPF_READER_H
#define XSPF_READER_H


#include "XspfDefines.h"
#include <string>


namespace Xspf {


/// @cond DOXYGEN_IGNORE

// Messages with ONE "%s" in it
#define XSPF_READER_TEXT_ONE_ATTRIBUTE_FORBIDDEN                      _PT("Attribute '%s' not allowed.")
#define XSPF_READER_TEXT_ONE_EXPAT_ERROR                              _PT("Expat error '%s'")
#define XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN                        _PT("Element '%s' not allowed.")
#define XSPF_READER_TEXT_ONE_ELEMENT_FORBIDDEN_VERSION_ZERO           _PT("Element '%s' not allowed in XSPF-0.")
#define XSPF_READER_TEXT_ONE_FILE_READING_ERROR                       _PT("File '%s' could not be read.")
#define XSPF_READER_TEXT_ONE_WRONG_ROOT_NAME                          _PT("Root element must be '") XSPF_NS_HOME XSPF_NS_SEP_STRING _PT("playlist', not '%s'.")
#define XSPF_READER_TEXT_ONE_WRONG_VERSION                            _PT("Version must be '0' or '1', not '%s'.")

// Messages with ZERO "%s" in it
#define XSPF_READER_TEXT_ZERO_ATTRIBUTE_MISSING(name)                 _PT("Attribute '") name _PT("' missing.")
#define XSPF_READER_TEXT_ZERO_ELEMENT_MISSING(ns, name)               _PT("Element '") ns XSPF_NS_SEP_STRING name _PT("' missing.")
#define XSPF_READER_TEXT_ZERO_ELEMENT_MISSING_VERSION_ZERO(ns, name)  _PT("Element '") ns XSPF_NS_SEP_STRING name _PT("' missing. This is not allowed in XSPF-0.")
#define XSPF_READER_TEXT_ZERO_FILENAME_NULL                           _PT("Filename must not be NULL.")
#define XSPF_READER_TEXT_ZERO_TOO_MANY_ELEMENTS(ns, name)             _PT("Only one '") ns XSPF_NS_SEP_STRING name _PT("' allowed.")
#define XSPF_READER_TEXT_ZERO_WRONG_ATTRIBUTE_TYPE(attr, type)        _PT("Attribute '") attr _PT("' is not a valid ") type _PT(".")
#define XSPF_READER_TEXT_ZERO_WRONG_CONTENT_TYPE(ns, elem, type)      _PT("Content of '") ns XSPF_NS_SEP_STRING elem _PT("' is not a valid ") type _PT(".")
#define XSPF_READER_TEXT_ZERO_TEXT_FORBIDDEN(ns, elem)                _PT("Content of '") ns XSPF_NS_SEP_STRING elem _PT("' must be whitespace or child elements, not text.")
#define XSPF_READER_TEXT_ZERO_KEY_WITHOUT_VERSION(name)               _PT("Attribute '") name _PT("' does not carry version information.")
#define XSPF_READER_TEXT_ZERO_KEY_WITH_REL_URI(name)                  _PT("Attribute '") name _PT("' does not contain an absolute URI.")

/// @endcond


/**
 * Specifies the result of a parse operation.
 */
enum XspfReaderReturnCode {
	XSPF_READER_SUCCESS, ///< Everything fine

	XSPF_READER_ERROR_NO_INPUT, ///< No input given
	XSPF_READER_ERROR_ELEMENT_TOOMANY, ///< Element occurs more often than allowed
	XSPF_READER_ERROR_ELEMENT_FORBIDDEN, ///< Element is not allowed at that place
	XSPF_READER_ERROR_ELEMENT_MISSING, ///< Required element missing
	XSPF_READER_ERROR_ATTRIBUTE_INVALID, ///< Attribute with invalid value
	XSPF_READER_ERROR_ATTRIBUTE_MISSING, ///< Required attribute missing
	XSPF_READER_ERROR_ATTRIBUTE_FORBIDDEN, ///< Attribute not allowed at that place
	XSPF_READER_ERROR_CONTENT_INVALID, ///< Element body has invalid format
	XSPF_READER_ERROR_BASE_URI_USELESS, ///< Given base URI is not a valid absolute URI

	XSPF_READER_WARNING_KEY_WITHOUT_VERSION, ///< A key attribute holds an unversioned URI
	XSPF_READER_WARNING_KEY_WITH_REL_URI, ///< A key attribute holds a relative URI

	XSPF_READER_ERROR_MALICIOUS_SPACE, ///< An entity is taking to much space
	XSPF_READER_ERROR_MALICIOUS_LOOKUP_SUM, ///< An entity takes to many lookups in sum
	XSPF_READER_ERROR_MALICIOUS_LOOKUP_DEPTH, //< An entity's lookup depth is too high

	// Insert new codes HERE!

	// This one must come last!
	XSPF_READER_ERROR_EXPAT = 0x1000 /// First Expat error code
};


/*
playlist									1
	title							?
	creator							?
	annotation						?
	info							?
	location						?
	identifier						?
	image							?
	date							?
	license							?
	attribution						?
		location						*
		identifier						*
	link								*
	meta								*
	extension							*
		...								*
	trackList								1
		track							+|*
			location					*
			identifier					*
			title					?
			creator					?
			annotation				?
			info					?
			image					?
			album 					?
			trackNum (uint > 0)		?
			duration (uint)			?
			link						*
			meta						*
			extension					*
*/

/**
 * Specifies the type of tag element.
 */
enum XspfTag {
	// Stack returns 0 if empty
	TAG_UNKNOWN, ///< Unknown type

	TAG_PLAYLIST, ///< playlist tag
	TAG_PLAYLIST_TITLE,  ///< playlist.title tag
	TAG_PLAYLIST_CREATOR, ///< playlist.creator tag
	TAG_PLAYLIST_ANNOTATION, ///< playlist.annotation tag
	TAG_PLAYLIST_INFO, ///< playlist.info tag
	TAG_PLAYLIST_LOCATION, ///< playlist.location tag
	TAG_PLAYLIST_IDENTIFIER, ///< playlist.identifier tag
	TAG_PLAYLIST_IMAGE, ///< playlist.image tag
	TAG_PLAYLIST_DATE, ///< playlist.date tag
	TAG_PLAYLIST_LICENSE, ///< playlist.license tag
	TAG_PLAYLIST_ATTRIBUTION, ///< playlist.attribution tag
	TAG_PLAYLIST_ATTRIBUTION_LOCATION, ///< playlist.attribution.location tag
	TAG_PLAYLIST_ATTRIBUTION_IDENTIFIER, ///< playlist.attribution.identifier tag
	TAG_PLAYLIST_LINK, ///< playlist.link tag
	TAG_PLAYLIST_META, ///< playlist.meta tag
	TAG_PLAYLIST_EXTENSION, ///< playlist.extension tag
	TAG_PLAYLIST_TRACKLIST, ///< playlist.tracklist tag
	TAG_PLAYLIST_TRACKLIST_TRACK, ///< playlist.tracklist.track tag
	TAG_PLAYLIST_TRACKLIST_TRACK_LOCATION, ///< playlist.tracklist.track.location tag
	TAG_PLAYLIST_TRACKLIST_TRACK_IDENTIFIER, ///< playlist.tracklist.track.identifier tag
	TAG_PLAYLIST_TRACKLIST_TRACK_TITLE, ///< playlist.tracklist.track.title tag
	TAG_PLAYLIST_TRACKLIST_TRACK_CREATOR, ///< playlist.tracklist.track.creator tag
	TAG_PLAYLIST_TRACKLIST_TRACK_ANNOTATION, ///< playlist.tracklist.track.annotation tag
	TAG_PLAYLIST_TRACKLIST_TRACK_INFO, ///< playlist.tracklist.track.info tag
	TAG_PLAYLIST_TRACKLIST_TRACK_IMAGE, ///< playlist.tracklist.track.image tag
	TAG_PLAYLIST_TRACKLIST_TRACK_ALBUM, ///< playlist.tracklist.track.album tag
	TAG_PLAYLIST_TRACKLIST_TRACK_TRACKNUM, ///< playlist.tracklist.track.tracknum tag
	TAG_PLAYLIST_TRACKLIST_TRACK_DURATION, ///< playlist.tracklist.track.duration tag
	TAG_PLAYLIST_TRACKLIST_TRACK_LINK, ///< playlist.tracklist.track.link tag
	TAG_PLAYLIST_TRACKLIST_TRACK_META, ///< playlist.tracklist.track.meta tag
	TAG_PLAYLIST_TRACKLIST_TRACK_EXTENSION, ///< playlist.tracklist.track.extension tag

	// Insert XSPF-2 codes HERE!

	// This one must come last!
	TAG_USER = 0x1000 ///< First user/extension tag code
};


class XspfProps;
class XspfDateTime;
class XspfTrack;
class XspfReaderCallback;
template <class T> class XspfStack;
class XspfChunkCallback;

class XspfExtensionReaderFactory;
class XspfExtensionReader;
class XspfReaderPrivate;


/**
 * Reads a XSPF playlist from a file.
 */
class XspfReader {

private:
	/// @cond DOXYGEN_NON_API
	XspfReaderPrivate * const d; ///< D pointer
	/// @endcond

public:
	/**
	 * Creates a new reader.
	 *
	 * @param handlerFactory  Factory used to create handlers
	 */
	XspfReader(XspfExtensionReaderFactory * handlerFactory = NULL);

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfReader(XspfReader const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfReader & operator=(XspfReader const & source);

	/**
	 * Frees all own memory.
	 */
	~XspfReader();

	/**
	 * Reads an XSPF playlist from a file.
	 *
	 * @param filename	Filename of the file to read
	 * @param callback	Reader callback that will receive the playlist's information.
	 *                    Passing \c NULL will make the reader use a XspfStrictReaderCallback
	 *                    instance internally.
	 * @param baseUri	Absolute external base URI to resolve URIs against
	 * @return          Error/warning code that aborted parsing or \c XSPF_READER_SUCCESS.
	 * @since 1.0.0rc1
	 */
	int parseFile(XML_Char const * filename, XspfReaderCallback * callback,
			XML_Char const * baseUri);

	/**
	 * Reads an XSPF playlist from a block of memory.
	 *
	 * @param memory	Memory block to parse
	 * @param numBytes	Size of <code>memory</code> in bytes
	 * @param callback	Reader callback that will receive the playlist's information
	 *                    Passing \c NULL will make the reader use a XspfStrictReaderCallback
	 *                    instance internally.
	 * @param baseUri	Absolute external base URI to resolve URIs against
	 * @return          Error/warning code that aborted parsing or \c XSPF_READER_SUCCESS.
	 * @since 1.0.0rc1
	 */
	int parseMemory(char const * memory, int numBytes, XspfReaderCallback * callback,
			XML_Char const * baseUri);

	/**
	 * Reads an XSPF playlist from a chunk callback.
	 *
	 * @param inputCallback  Chunk callback, must not be \c NULL
	 * @param dataCallback   Reader callback that will receive the playlist's information
	 *                         Passing \c NULL will make the reader use a XspfStrictReaderCallback
	 *                         instance internally.
	 * @param baseUri        Absolute external base URI to resolve URIs against
	 * @return               Error/warning code that aborted parsing or \c XSPF_READER_SUCCESS.
	 * @since 1.0.0rc1
	 */
	int parseChunks(XspfChunkCallback * inputCallback,
			XspfReaderCallback * dataCallback, XML_Char const * baseUri);

	/**
	 * Enables/disables the restriction on the length of entity values.
	 *
	 * @experimental
	 * @param enabled  Enabled flag
	 * @see setMaxLengthPerEntityValue
	 * @since 1.0.0
	 */
	void limitLengthPerEntityValue(bool enabled);

	/**
	 * Enables/disables the restriction on the sum of lookups per entity value.
	 *
	 * @experimental
	 * @param enabled  Enabled flag
	 * @see setMaxLookupSumPerEntityValue
	 * @since 1.0.0
	 */
	void limitLookupSumPerEntityValue(bool enabled);

	/**
	 * Enables/disables the restriction on the lookup depth of entity values.
	 *
	 * @experimental
	 * @param enabled  Enabled flag
	 * @see setMaxLookupDepthPerEntityValue
	 * @since 1.0.0
	 */
	void limitLookupDepthPerEntityValue(bool enabled);

	/**
	 * Enables/disables all means of malicious XML detection.
	 * This function is provided for convenience.
	 *
	 * @experimental
	 * @param enabled  Enabled flag
	 * @see limitLengthPerEntityValue, setMaxLengthPerEntityValue
	 * @see limitLookupSumPerEntityValue, setMaxLookupSumPerEntityValue
	 * @see limitLookupDepthPerEntityValue, setMaxLookupDepthPerEntityValue
	 * @since 1.0.0
	 */
	void enableMaliciousXmlDetection(bool enabled);

	/**
	 * Adjusts the limit on the length of entity values.
	 *
	 * @experimental
	 * @param maxLength  Inclusive limit to set
	 * @see limitLengthPerEntityValue
	 * @since 1.0.0
	 */
	void setMaxLengthPerEntityValue(int maxLength);

	/**
	 * Adjusts the limit on the sum of lookups per entity value.
	 *
	 * @experimental
	 * @param maxLookupSum  Inclusive limit to set
	 * @see limitLookupSumPerEntityValue
	 * @since 1.0.0
	 */
	void setMaxLookupSumPerEntityValue(int maxLookupSum);

	/**
	 * Adjusts the limit on the lookup depth of entity values.
	 *
	 * @experimental
	 * @param maxLookupDepth  Inclusive limit to set
	 * @see limitLookupDepthPerEntityValue
	 * @since 1.0.0
	 */
	void setMaxLookupDepthPerEntityValue(int maxLookupDepth);

private:
	/**
	 * Make the parser reusable so it can parse another file.
	 */
	void makeReusable();

	/**
	 * Handles the occurrence of a skippable error.
	 *
	 * @param code	Error code
	 * @param text	Error description
	 * @return      Continue parsing flag
	 */
	bool handleError(int code, XML_Char const * text);

	/**
	 * Handles the occurrence of a skippable error.
	 *
	 * @param code		Error code
	 * @param format	Error description format string containing <c>%s</c>
	 * @param param		Text parameter to insert for <c>%s</c>
	 * @return      Continue parsing flag
	 */
	bool handleError(int code, XML_Char const * format, XML_Char const * param);

	/**
	 * Handles the occurrence of a fatal error.
	 *
	 * @param code	Error code
	 * @param text	Error description
	 */
	void handleFatalError(int code, XML_Char const * text);

	/**
	 * Handles the occurrence of a fatal error.
	 *
	 * @param code		Error code
	 * @param format	Error description format string containing <c>%s</c>
	 * @param param		Text parameter to insert for <c>%s</c>
	 */
	void handleFatalError(int code, XML_Char const * format, XML_Char const * param);

	/**
	 * Handles the occurrence of a warning.
	 *
	 * @param code	Warning code
	 * @param text	Warning description
	 * @return      Continue parsing flag
	 */
	bool handleWarning(int code, XML_Char const * text);

	/**
	 * Sets the error code and text based on information from Expat.
	 * This is needed for all non-XSPF errors, e.g. XML errors.
	 */
	void setExpatError();

	/**
	 * Halts the parser.
	 */
	void stop();

	/**
	 * Forwards tag opening handling to the correct handler.
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts		Alternating list of attribute keys and values
	 */
	void handleStart(XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Handles tag opening on level one (e.g. <c>playlist</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts		Alternating list of attribute keys and values
	 * @return			Continue parsing flag
	 */
	bool handleStartOne(XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Handles tag opening on level two (e.g. <c>playlist.title</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts	Alternating list of attribute keys and values
	 * @return		Continue parsing flag
	 */
	bool handleStartTwo(XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Handles tag opening on level three (e.g. <c>playlist.trackList.track</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts		Alternating list of attribute keys and values
	 * @return			Continue parsing flag
	 */
	bool handleStartThree(XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Handles tag opening on level four (e.g. <c>playlist.trackList.track.title</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts		Alternating list of attribute keys and values
	 * @return			Continue parsing flag
	 */
	bool handleStartFour(XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Handles element content.
	 *
	 * @param s		Text content
	 * @param len	Characters allowed to read
	 */
	void handleCharacters(XML_Char const * s, int len);

	/**
	 * Handles entity declarations.
	 *
	 * @param entityName  Name of the entity to be declared
	 * @param value       Value to be assigned
	 */
	void handleEntityDeclaration(XML_Char const * entityName,
			XML_Char const * value);

	/**
	 * Forwards tag closing handling to the correct handler.
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 */
	void handleEnd(XML_Char const * fullName);

	/**
	 * Handles tag closing on level one (e.g. <c>playlist</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @return			Continue parsing flag
	 */
	bool handleEndOne(XML_Char const * fullName);

	/**
	 * Handles tag closing on level two (e.g. <c>playlist.title</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @return			Continue parsing flag
	 */
	bool handleEndTwo(XML_Char const * fullName);

	/**
	 * Handles tag closing on level three (e.g. <c>playlist.trackList.track</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @return			Continue parsing flag
	 */
	bool handleEndThree(XML_Char const * fullName);

	/**
	 * Handles tag closing on level four (e.g. <c>playlist.trackList.track.title</c>).
	 *
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @return			Continue parsing flag
	 */
	bool handleEndFour(XML_Char const * fullName);

	/**
	 * Checks the attributes of the <c>playlist</c> tag
	 * and extracts the XSPF version from it.
	 *
	 * @param atts	Alternating list of attribute keys and values
	 * @return		Attributes okay flag
	 */
	bool handlePlaylistAttribs(XML_Char const ** atts);

	/**
	 * Checks the attributes of the <c>meta</c> tag for
	 * both <c>playlist.meta</c> and <c>playlist.trackList.track.meta</c>.
	 *
	 * @param atts  Alternating list of attribute keys and values
	 * @param rel   Value of the 'rel' attribute or \c NULL
	 * @return      Attributes okay flag
	 */
	bool handleMetaLinkAttribs(XML_Char const ** atts, XML_Char const * & rel);

	/**
	 * Checks the attributes of the <c>extension</c> tag for
	 * both <c>playlist.extension</c> and <c>playlist.trackList.track.extension</c>.
	 *
	 * @param atts			Alternating list of attribute keys and values
	 * @param application	Points to the application URI in case of success
	 * @return				Attributes okay flag
	 */
	bool handleExtensionAttribs(XML_Char const ** atts,
			XML_Char const * & application);

	/**
	 * Checks the attributes of a tag that must not have any attributes.
	 *
	 * @param atts	Alternating list of attribute keys and values
	 * @return		No attributes flag
	 */
	bool handleNoAttribsExceptXmlBase(XML_Char const ** atts);

	/**
	 * Resets the error status to success.
	 */
	void clearError();

	/**
	 * Forwards tag opening handling to the correct class instance.
	 *
	 * @param userData	Instance pointer
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 * @param atts		Alternating list of attribute keys and values
	 */
	static void masterStart(void * userData, XML_Char const * fullName, XML_Char const ** atts);

	/**
	 * Forwards tag closing handling to the correct class instance.
	 *
	 * @param userData	Instance pointer
	 * @param fullName	Full tag name ("<namespace_uri> <localname>")
	 */
	static void masterEnd(void * userData, XML_Char const * fullName);

	/**
	 * Forwards element content handling to the correct class instance.
	 *
	 * @param userData	Instance pointer
	 * @param s			Text content
	 * @param len		Characters allowed to read
	 */
	static void masterCharacters(void * userData, XML_Char const * s, int len);

	/**
	 * Forwards entity declaration handling to the correct class instance.
	 *
	 * @param userData  Instance pointer
	 * @param entityName           Name of the entity to be declared
	 * @param is_parameter_entity  TODO
	 * @param value                Value to be assigned, not necessarily zero-terminated
	 * @param value_length         Length of \p value in characters
	 * @param base                 TODO
	 * @param systemId             TODO
	 * @param publicId             TODO
	 * @param notationName         TODO
	 */
	static void masterEntityDeclaration(void * userData, XML_Char const * entityName,
			int is_parameter_entity, XML_Char const * value, int value_length,
			XML_Char const * base, XML_Char const * systemId, XML_Char const * publicId,
			XML_Char const * notationName);

	/**
	 * Initializes parsing.
	 *
	 * @param callback	Reader callback that will receive the playlist's information
	 * @param baseUri	Absolute external base URI to resolve URIs against
	 * @return			true to continue init, false to abort
	 */
	bool onBeforeParse(XspfReaderCallback * callback,
			XML_Char const * baseUri);

	/**
	 * Cleans up behind parsing.
	 */
	void onAfterParse();

public:
	/**
	 * Checks if \p attributeKey is the xml:base attribute
	 *
	 * @param attributeKey  Attribute key to check
	 * @return              Equality flag
	 */
	static bool isXmlBase(XML_Char const * attributeKey);

private:
	/**
	 * Handles an appearing xml:base attribute.
	 *
	 * @param xmlBase  Value of xml:base attribute
	 * @return         Continue parsing flag
	 */
	bool handleXmlBaseAttribute(XML_Char const * xmlBase);

	/**
	 * Transforms a (possibly relative) URI into an absolute URI
	 * in respect to the current base URI at this point of parsing.
	 *
	 * @param sourceUri  URI to transform
	 * @return           New'ed absolute URI or \c NULL
	 */
	XML_Char * makeAbsoluteUri(XML_Char const * sourceUri) const;

	/**
	 * Returns the internal element stack.
	 *
	 * @return  Element stack
	 */
	XspfStack<unsigned int> & getElementStack() const;

	/**
	 * Returns the internal base URI stack.
	 *
	 * @return  Base URI stack
	 */
	XspfStack<std::basic_string<XML_Char> > & getBaseUriStack() const;

	/**
	 * Puts the reader ninto skipping mode.
	 * If this function is called with //playlist.meta
	 * on top of the element stack the reader will skip
	 * everything until //playlist.meta is closed.
	 */
	void skipFromHere();

	/**
	 * Takes a tag <nameSpace:localName> and sets \c localName
	 * to the start of localName.
	 *
	 * @param fullName   Full name of the element
	 * @param localName  Local name of the element if the
	 *                   namespace matched xspf, original full name
	 *                   otherwise
	 * @return           Continue parsing flag
	 */
	bool checkAndSkipNamespace(XML_Char const * fullName,
			XML_Char const * & localName);

	/**
	 * Notifies the reader callback that parsing has
	 * finished successfully.
	 */
	void notifySuccess() const;

	friend class XspfExtensionReader;

};


} // namespace Xspf

#endif // XSPF_READER_H
