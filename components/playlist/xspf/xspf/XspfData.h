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
 * @file XspfData.h
 * Interface of XspfData.
 */

#ifndef XSPF_DATA_H
#define XSPF_DATA_H

#include "XspfDefines.h"
#include <deque>

namespace Xspf {


class XspfExtension;
class XspfDataPrivate;


/**
 * Provides shared functionality for XspfTrack and XspfProps.
 */
class XspfData {

	friend class XspfDataPrivate;

private:
	/// @cond DOXYGEN_NON_API
	XspfDataPrivate * const d; ///< D pointer
	/// @endcond

protected:
	/**
	 * Creates a new XspfData object.
	 */
	XspfData();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfData(XspfData const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfData & operator=(XspfData const & source);

	/**
	 * Destroys this XspfData object and deletes all
	 * memory associated with it that has not been stolen before.
	 */
	virtual ~XspfData();

public:
	/**
	 * Overwrites the image property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param image		Image string to set
	 * @param copy		Copy flag
	 */
	void giveImage(XML_Char const * image, bool copy);

	/**
	 * Overwrites the info property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param info		Info string to set
	 * @param copy		Copy flag
	 */
	void giveInfo(XML_Char const * info, bool copy);

	/**
	 * Overwrites the annotation property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param annotation	Annotation string to set
	 * @param copy			Copy flag
	 */
	void giveAnnotation(XML_Char const * annotation, bool copy);

	/**
	 * Overwrites the creator property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param creator	Creator string to set
	 * @param copy		Copy flag
	 */
	void giveCreator(XML_Char const * creator, bool copy);

	/**
	 * Overwrites the title property. If <c>copy</c> is true
	 * the string will be copied, otherwise just assigned.
	 * In both cases the associated memory will be deleted on
	 * object destruction.
	 *
	 * @param title		Title string to set
	 * @param copy		Copy flag
	 */
	void giveTitle(XML_Char const * title, bool copy);

	/**
	 * Appends a link to the link list.
	 *
	 * @param rel			Link rel to append
	 * @param copyRel		Rel copy flag
	 * @param content		Link content to append
	 * @param copyContent	Content copy flag
	 */
	void giveAppendLink(XML_Char const * rel, bool copyRel, XML_Char const * content, bool copyContent);

	/**
	 * Appends a meta to the meta list.
	 *
	 * @param rel			Meta rel to append
	 * @param copyRel		Rel copy flag
	 * @param content		Meta content to append
	 * @param copyContent	Content copy flag
	 */
	void giveAppendMeta(XML_Char const * rel, bool copyRel, XML_Char const * content, bool copyContent);

	/**
	 * Appends an extension to the extension list.
	 *
	 * @param extension		Extension to append
	 * @param copy			Copy flag
	 */
	void giveAppendExtension(XspfExtension const * extension, bool copy);

	/**
	 * Overwrites the image property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param image		Image string to set
	 */
	void lendImage(XML_Char const * image);

	/**
	 * Overwrites the info property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param info		Info string to set
	 */
	void lendInfo(XML_Char const * info);

	/**
	 * Overwrites the annotation property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param annotation		Annotation string to set
	 */
	void lendAnnotation(XML_Char const * annotation);

	/**
	 * Overwrites the creator property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param creator		Creator string to set
	 */
	void lendCreator(XML_Char const * creator);

	/**
	 * Overwrites the title property. The string is
	 * only assigned not copied. The ownership is
	 * not transferred.
	 *
	 * @param title		Title string to set
	 */
	void lendTitle(XML_Char const * title);

	/**
	 * Appends a link pair to the link list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param rel		Link rel to append
	 * @param content	Link content to append
	 */
	void lendAppendLink(XML_Char const * rel, XML_Char const * content);

	/**
	 * Appends a meta pair to the meta list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param rel		Meta rel to append
	 * @param content	Meta content to append
	 */
	void lendAppendMeta(XML_Char const * rel, XML_Char const * content);

	/**
	 * Appends an extension to the extension list.
	 * The associated memory is neither copied nor
	 * deleted on onject destruction.
	 *
	 * @param extension		Extension to append
	 */
	void lendAppendExtension(XspfExtension * extension);

	/**
	 * Steals the image property.
	 *
	 * @return	Image URI, can be NULL
	 */
	XML_Char * stealImage();

	/**
	 * Steals the info property.
	 *
	 * @return	Info URI, can be NULL
	 */
	XML_Char * stealInfo();

	/**
	 * Steals the annotation property.
	 *
	 * @return	Annotation, can be NULL
	 */
	XML_Char * stealAnnotation();

	/**
	 * Steals the creator property.
	 *
	 * @return	Creator, can be NULL
	 */
	XML_Char * stealCreator();

	/**
	 * Steals the title property.
	 *
	 * @return	Title, can be NULL
	 */
	XML_Char * stealTitle();

	/**
	 * Steals the first link pair from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: Do not forget to delete the pair!
	 *
	 * @return		First link pair, can be NULL
	 */
	std::pair<XML_Char *, XML_Char *> * stealFirstLink();

	/**
	 * Steals the first meta pair from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: Do not forget to delete the pair!
	 *
	 * @return		First meta pair, can be NULL
	 */
	std::pair<XML_Char *, XML_Char *> * stealFirstMeta();

	/**
	 * Steals the extension from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: Do not forget to delete the extension!
	 *
	 * @return		First extension, can be NULL
	 */
	XspfExtension * stealFirstExtension();

	/**
	 * Returns the image property.
	 *
	 * @return	Image URI, can be NULL
	 */
	XML_Char const * getImage() const;

	/**
	 * Returns the info property.
	 *
	 * @return	Info URI, can be NULL
	 */
	XML_Char const * getInfo() const;

	/**
	 * Returns the annotation property.
	 *
	 * @return	Annotation, can be NULL
	 */
	XML_Char const * getAnnotation() const;

	/**
	 * Returns the creator property.
	 *
	 * @return	Creator, can be NULL
	 */
	XML_Char const * getCreator() const;

	/**
	 * Returns the title property.
	 *
	 * @return	Title, can be NULL
	 */
	XML_Char const * getTitle() const;

	/**
	 * Gets a specific link pair from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @return		Specified link pair, can be NULL
	 */
	std::pair<XML_Char const *, XML_Char const *> * getLink(int index) const;

	/**
	 * Gets a specific meta pair from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @return		Specified meta pair, can be NULL
	 */
	std::pair<XML_Char const *, XML_Char const *> * getMeta(int index) const;

	/**
	 * Gets a specific extension from the list.
	 * If the list is empty <c>NULL</c> is returned.
	 *
	 * NOTE: The returned extension has to be deleted manually!
	 *
	 * @return		Specified extension, can be NULL
	 */
	XspfExtension const * getExtension(int index) const;

	/**
	 * Returns the number of link pairs.
	 *
	 * @return	Number of link pairs
	 */
	int getLinkCount() const;

	/**
	 * Returns the number of meta pairs.
	 *
	 * @return	Number of meta pairs
	 */
	int getMetaCount() const;

	/**
	 * Returns the number of extensions.
	 *
	 * @return	Number of extensions
	 */
	int getExtensionCount() const;

private:
	/**
	 * Appends a meta or link pair to a container.
	 *
	 * @param container		Container to work with
	 * @param rel			Rel to append
	 * @param ownRel		Rel ownership flag
	 * @param content		Content to append
	 * @param ownContent	Content ownership flag
	 */
	static void appendHelper(std::deque<std::pair<
			std::pair<XML_Char const *, bool> *,
			std::pair<XML_Char const *, bool> *> *> * & container,
			XML_Char const * rel,
			bool ownRel,
			XML_Char const * content,
			bool ownContent);

	/**
	 * Appends an extension to a container.
	 *
	 * @param container		Container to work with
	 * @param extenstion	Extenstion to append
	 * @param own			Ownership flag
	 */
	static void appendHelper(std::deque<
			std::pair<XspfExtension const *, bool> *> * & container,
			XspfExtension const * extenstion,
			bool own);

protected:
	/**
	 * Steals a property. If the property's memory is not owned
	 * a clone is returned. In any case you own the memory
	 * return and have to delete it.
	 *
	 * @param property	Property to steal
	 * @param own		Owner flag
	 * @return			Stolen property value, can be NULL
	 */
	static XML_Char * stealHelper(XML_Char const * & property, bool own);

private:
	/**
	 * Steals the first entry from a container.
	 *
	 * @param container		Container to steal from
	 * @return				First entry, can be NULL
	 */
	static std::pair<XML_Char *, XML_Char *> * stealFirstHelper(
			std::deque<std::pair<std::pair<XML_Char const *, bool> *,
			std::pair<XML_Char const *, bool> *> *> * & container);

	/**
	 * Steals the first entry from a container.
	 *
	 * @param container		Container to steal from
	 * @return				First entry, can be NULL
	 */
	static XspfExtension * stealFirstHelper(std::deque<
			std::pair<XspfExtension const *, bool> *> * & container);

	/**
	 * Returns a specific entry from a container
	 * or <c>NULL</c> if the entry does not exist.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @param container		Container to work with
	 * @param index			Index of the entry to return
	 * @return				Entry content, can be NULL
	 */
	static std::pair<XML_Char const *, XML_Char const *> * getHelper(
			std::deque<std::pair<std::pair<XML_Char const *, bool> *,
			std::pair<XML_Char const *, bool> *> *> * & container,
			int index);

	/**
	 * Returns a specific entry from a container
	 * or <c>NULL</c> if the entry does not exist.
	 *
	 * NOTE: The returned pair has to be deleted manually!
	 *
	 * @param container		Container to work with
	 * @param index			Index of the entry to return
	 * @return				Entry content, can be NULL
	 */
	static XspfExtension const * getHelper(
		std::deque<std::pair<XspfExtension const *, bool> *> * & container,
		int index);

protected:
	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

public:
	static bool const COPY; ///< Memory is copied
	static bool const TRANSFER; ///< Memory ownership is transfered

};


}

#endif // XSPF_DATA_H
