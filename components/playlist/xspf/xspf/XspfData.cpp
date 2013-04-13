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
 * @file XspfData.cpp
 * Implementation of XspfData.
 */

#include <xspf/XspfData.h>
#include <xspf/XspfExtension.h>
#include <xspf/XspfToolbox.h>

namespace Xspf {


/*static*/ const bool XspfData::COPY = true;
/*static*/ const bool XspfData::TRANSFER = false;


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfData.
 */
class XspfDataPrivate {

	friend class XspfData;

	XML_Char const * image; ///< Image URI
	XML_Char const * info; ///< Info URI
	XML_Char const * annotation; ///< Annotation
	XML_Char const * creator; ///< Creator/artist
	XML_Char const * title; ///< Title
	bool ownImage; ///< Image memory ownership flag
	bool ownInfo; ///< Info memory ownership flag
	bool ownAnnotation; ///< Annotation memory ownership flag
	bool ownCreator; ///< Creator memory ownership flag
	bool ownTitle; ///< Title memory ownership flag
	std::deque<std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> *> * links; ///< List of link pairs
	std::deque<std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> *> * metas; ///< List of meta pairs
	std::deque<std::pair<XspfExtension const *, bool> *> * extensions; ///< List of extensions

	/**
	 * Creates a new D object.
	 */
	XspfDataPrivate()
			: image(NULL),
			info(NULL),
			annotation(NULL),
			creator(NULL),
			title(NULL),
			ownImage(false),
			ownInfo(false),
			ownAnnotation(false),
			ownCreator(false),
			ownTitle(false),
			links(NULL),
			metas(NULL),
			extensions(NULL) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfDataPrivate(XspfDataPrivate const & source)
			: image(source.ownImage
				? Toolbox::newAndCopy(source.image)
				: source.image),
			info(source.ownInfo
				? Toolbox::newAndCopy(source.info)
				: source.info),
			annotation(source.ownAnnotation
				? Toolbox::newAndCopy(source.annotation)
				: source.annotation),
			creator(source.ownCreator
				? Toolbox::newAndCopy(source.creator)
				: source.creator),
			title(source.ownTitle
				? Toolbox::newAndCopy(source.title)
				: source.title),
			ownImage(source.ownImage),
			ownInfo(source.ownInfo),
			ownAnnotation(source.ownAnnotation),
			ownCreator(source.ownCreator),
			ownTitle(source.ownTitle),
			links(NULL),
			metas(NULL),
			extensions(NULL) {
		copyMetasOrLinks(this->links, source.links);
		copyMetasOrLinks(this->metas, source.metas);
		copyExtensions(this->extensions, source.extensions);
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfDataPrivate & operator=(XspfDataPrivate const & source) {
		if (this != &source) {
			free();
			assign(source);
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfDataPrivate() {
		free();
	}

	void free() {
		// Frees all data, does not set to NULL
		Toolbox::freeIfOwned(this->title, this->ownTitle);
		Toolbox::freeIfOwned(this->creator, this->ownCreator);
		Toolbox::freeIfOwned(this->annotation, this->ownAnnotation);
		Toolbox::freeIfOwned(this->image, this->ownImage);
		Toolbox::freeIfOwned(this->info, this->ownInfo);
		freeMetasOrLinks(this->links);
		freeMetasOrLinks(this->metas);
		freeExtensions(this->extensions);
	}
	
	void assign(XspfDataPrivate const & source) {
		// Assigns all members, does not free current content
		Toolbox::copyIfOwned(this->title, this->ownTitle, source.title, source.ownTitle);
		Toolbox::copyIfOwned(this->creator, this->ownCreator, source.creator, source.ownCreator);
		Toolbox::copyIfOwned(this->annotation, this->ownAnnotation, source.annotation, source.ownAnnotation);
		Toolbox::copyIfOwned(this->image, this->ownImage, source.image, source.ownImage);
		Toolbox::copyIfOwned(this->info, this->ownInfo, source.info, source.ownInfo);
		copyMetasOrLinks(this->links, source.links);
		copyMetasOrLinks(this->metas, source.metas);
		copyExtensions(this->extensions, source.extensions);
	}

	static inline void freeMetasOrLinks(std::deque<std::pair<std::pair<
			XML_Char const *, bool> *, std::pair<XML_Char const *,
			bool> *> *> * & container) {
		if (container != NULL) {
			std::deque<std::pair<std::pair<XML_Char const *, bool> *,
					std::pair<XML_Char const *, bool> *> *>
					::const_iterator iter = container->begin();
			while (iter != container->end()) {
				std::pair<std::pair<XML_Char const *, bool> *,
						std::pair<XML_Char const *, bool> *>
						* const entry = *iter;
				if (entry->first->second) {
					delete [] entry->first->first;
				}
				delete entry->first;
				if (entry->second->second) {
					delete [] entry->second->first;
				}
				delete entry->second;
				delete entry;
				iter++;
			}
			container->clear();
			delete container;
			container = NULL;
		}
	}

	static inline void copyMetasOrLinks(std::deque<std::pair<
			std::pair<XML_Char const *, bool> *,
			std::pair<XML_Char const *, bool> *> *> * & dest,
			std::deque<std::pair<
			std::pair<XML_Char const *, bool> *,
			std::pair<XML_Char const *, bool> *> *> * const & source) {
		dest = new std::deque<std::pair<
				std::pair<XML_Char const *, bool> *,
				std::pair<XML_Char const *, bool> *> *>();

		if (source != NULL) {
			std::deque<std::pair<std::pair<XML_Char const *, bool> *,
					std::pair<XML_Char const *, bool> *> *>::const_iterator
					iter = source->begin();
			while (iter != source->end()) {
				const std::pair<std::pair<XML_Char const *, bool> *,
						std::pair<XML_Char const *, bool> *>
						* const entry = *iter;

				bool const ownRel = entry->first->second;
				bool const ownContent = entry->second->second;
				XML_Char const * const rel = ownRel
						? Toolbox::newAndCopy(entry->first->first)
						: entry->first->first;
				XML_Char const * const content = ownContent
						? Toolbox::newAndCopy(entry->second->first)
						: entry->second->first;

				XspfData::appendHelper(dest,
						rel, ownRel,
						content, ownContent);
				iter++;
			}
		}
	}

	static inline void freeExtensions(std::deque<std::pair<
			XspfExtension const *, bool> *> * & container) {
		if (container != NULL) {
			std::deque<std::pair<XspfExtension const *, bool> *>::const_iterator
					iter = container->begin();
			while (iter != container->end()) {
				std::pair<XspfExtension const *, bool>
						* const entry = *iter;
				if (entry->second) {
					delete entry->first;
				}
				delete entry;
				iter++;
			}
			container->clear();
			delete container;
			container = NULL;
		}
	}

	static inline void copyExtensions(std::deque<
			std::pair<XspfExtension const *, bool> *> * & dest,
			std::deque<
			std::pair<XspfExtension const *, bool> *> * const & source) {
		dest = new std::deque<std::pair<XspfExtension const *, bool> *>();

		if (source != NULL) {
			std::deque<std::pair<XspfExtension const *, bool> *>::const_iterator
					iter = source->begin();
			while (iter != source->end()) {
				const std::pair<XspfExtension const *, bool>
						* const entry = *iter;

				bool const own = entry->second;
				XspfExtension const * const extension = own
						? entry->first->clone()
						: entry->first;
				XspfData::appendHelper(dest, extension, own);

				iter++;
			}
		}
	}

};

/// @endcond


XspfData::XspfData() : d(new XspfDataPrivate()) {
	// NOOP
}


XspfData::XspfData(XspfData const & source)
		: d(new XspfDataPrivate(*(source.d))) {
	// NOOP
}


XspfData &
XspfData::operator=(XspfData const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfData::~XspfData() {
	delete this->d;
}


void
XspfData::giveAnnotation(XML_Char const * annotation, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->annotation, this->d->ownAnnotation,
			annotation, copy);
}


void
XspfData::giveCreator(XML_Char const * creator, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->creator, this->d->ownCreator,
			creator, copy);
}


void
XspfData::giveInfo(XML_Char const * info, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->info, this->d->ownInfo, info, copy);
}


void
XspfData::giveImage(XML_Char const * image, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->image, this->d->ownImage, image, copy);
}


void
XspfData::giveTitle(XML_Char const * title, bool copy) {
	Toolbox::deleteNewAndCopy(this->d->title, this->d->ownTitle, title, copy);
}


void
XspfData::giveAppendLink(XML_Char const * rel, bool copyRel, XML_Char const * content, bool copyContent) {
	appendHelper(this->d->links, copyRel ? Toolbox::newAndCopy(rel) : rel, true,
			copyContent ? Toolbox::newAndCopy(content) : content, true);
}


void
XspfData::giveAppendMeta(XML_Char const * rel, bool copyRel, XML_Char const * content, bool copyContent) {
	appendHelper(this->d->metas, copyRel ? Toolbox::newAndCopy(rel) : rel, true,
			copyContent ? Toolbox::newAndCopy(content) : content, true);
}


void
XspfData::giveAppendExtension(XspfExtension const * extension, bool copy) {
	appendHelper(this->d->extensions,
			copy
				? extension->clone()
				: extension,
			true);
}


void
XspfData::lendAnnotation(XML_Char const * annotation) {
	Toolbox::deleteNewAndCopy(this->d->annotation, this->d->ownAnnotation,
			annotation, false);
}


void
XspfData::lendCreator(XML_Char const * creator) {
	Toolbox::deleteNewAndCopy(this->d->creator, this->d->ownCreator,
			creator, false);
}


void
XspfData::lendInfo(XML_Char const * info) {
	Toolbox::deleteNewAndCopy(this->d->info, this->d->ownInfo, info, false);
}


void
XspfData::lendImage(XML_Char const * image) {
	Toolbox::deleteNewAndCopy(this->d->image, this->d->ownImage, image, false);
}


void
XspfData::lendTitle(XML_Char const * title) {
	Toolbox::deleteNewAndCopy(this->d->title, this->d->ownTitle, title, false);
}


void
XspfData::lendAppendLink(XML_Char const * rel, XML_Char const * content) {
	appendHelper(this->d->links, rel, false, content, false);
}


void
XspfData::lendAppendMeta(XML_Char const * rel, XML_Char const * content) {
	appendHelper(this->d->metas, rel, false, content, false);
}


void
XspfData::lendAppendExtension(XspfExtension * extension) {
	appendHelper(this->d->extensions, extension, false);
}


XML_Char *
XspfData::stealTitle() {
	return stealHelper(this->d->title, this->d->ownTitle);
}


XML_Char *
XspfData::stealAnnotation() {
	return stealHelper(this->d->annotation, this->d->ownAnnotation);
}


XML_Char *
XspfData::stealCreator() {
	return stealHelper(this->d->creator, this->d->ownCreator);
}


XML_Char *
XspfData::stealInfo() {
	return stealHelper(this->d->info, this->d->ownInfo);
}


XML_Char *
XspfData::stealImage() {
	return stealHelper(this->d->image, this->d->ownImage);
}


std::pair<XML_Char *, XML_Char *> *
XspfData::stealFirstMeta() {
	return stealFirstHelper(this->d->metas);
}


std::pair<XML_Char *, XML_Char *> *
XspfData::stealFirstLink() {
	return stealFirstHelper(this->d->links);
}


XspfExtension *
XspfData::stealFirstExtension() {
	return stealFirstHelper(this->d->extensions);
}


const XML_Char *
XspfData::getImage() const {
	return this->d->image;
}


const XML_Char *
XspfData::getInfo() const {
	return this->d->info;
}


const XML_Char *
XspfData::getAnnotation() const {
	return this->d->annotation;
}


const XML_Char *
XspfData::getCreator() const {
	return this->d->creator;
}


const XML_Char *
XspfData::getTitle() const {
	return this->d->title;
}


std::pair<XML_Char const *, XML_Char const *> *
XspfData::getLink(int index) const {
	return getHelper(this->d->links, index);
}


std::pair<XML_Char const *, XML_Char const *> *
XspfData::getMeta(int index) const {
	return getHelper(this->d->metas, index);
}


const XspfExtension *
XspfData::getExtension(int index) const {
	return getHelper(this->d->extensions, index);
}


int
XspfData::getLinkCount() const {
	return (this->d->links == NULL) ? 0 : static_cast<int>(this->d->links->size());
}


int
XspfData::getMetaCount() const {
	return (this->d->metas == NULL) ? 0 : static_cast<int>(this->d->metas->size());
}


int
XspfData::getExtensionCount() const {
	return (this->d->extensions == NULL) ? 0 : static_cast<int>(this->d->extensions->size());
}


/*static*/ void XspfData::appendHelper(
		std::deque<std::pair<std::pair<XML_Char const *, bool> *,
		std::pair<XML_Char const *, bool> *> *> * & container,
		XML_Char const * rel, bool ownRel,
		XML_Char const * content, bool ownContent) {
	if (container == NULL) {
		container = new std::deque<std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> *>;
	}
	std::pair<XML_Char const *, bool> * const first = new std::pair<XML_Char const *, bool>(rel, ownRel);
	std::pair<XML_Char const *, bool> * const second = new std::pair<XML_Char const *, bool>(content, ownContent);
	std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> * const entry =
			new std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *>(first, second);
	container->push_back(entry);
}


/*static*/ void
XspfData::appendHelper(std::deque<std::pair<XspfExtension const *, bool> *> * &
		container, XspfExtension const * extension, bool own) {
	if (container == NULL) {
		container = new std::deque<std::pair<XspfExtension const *, bool> *>;
	}
	std::pair<XspfExtension const *, bool> * const entry =
			new std::pair<XspfExtension const *, bool>(extension, own);
	container->push_back(entry);
}


/*static*/ XML_Char *
XspfData::stealHelper(XML_Char const * & property,
		bool own) {
	XML_Char const * const res = Toolbox::getSetNull<XML_Char>(property);
	if (own) {
		return const_cast<XML_Char *>(res);
	} else if (res == NULL) {
		return NULL;
	} else {
		return Toolbox::newAndCopy(res);
	}
}


/*static*/ std::pair<XML_Char *, XML_Char *> *
XspfData::stealFirstHelper(
		std::deque<std::pair<std::pair<XML_Char const *, bool> *,
		std::pair<XML_Char const *, bool> *> *> * & container) {
	if ((container == NULL) || container->empty()) {
		return NULL;
	}
	std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> * const entry =
			container->front();
	container->pop_front();
	std::pair<XML_Char *, XML_Char *> * const res = new std::pair<XML_Char *, XML_Char *>(
			entry->first->second
			? const_cast<XML_Char *>(entry->first->first)
			: Toolbox::newAndCopy(entry->first->first), entry->second->second
			? const_cast<XML_Char *>(entry->second->first)
			: Toolbox::newAndCopy(entry->second->first));
	delete entry->first;
	delete entry->second;
	delete entry;
	return res;
}


/*static*/ XspfExtension *
XspfData::stealFirstHelper(
		std::deque<std::pair<XspfExtension const *, bool> *> * & container) {
	if ((container == NULL) || container->empty()) {
		return NULL;
	}
	std::pair<XspfExtension const *, bool> * const entry = container->front();
	container->pop_front();
	XspfExtension * res = entry->second
			? const_cast<XspfExtension *>(entry->first)
			: entry->first->clone();
	delete entry;
	return res;
}


/*static*/ std::pair<XML_Char const *, XML_Char const *> *
XspfData::getHelper(std::deque<std::pair<std::pair<XML_Char const *, bool> *,
		std::pair<XML_Char const *, bool> *> *> * & container, int index) {
	if ((container == NULL) || container->empty() || (index < 0)
			|| (index >= static_cast<int>(container->size()))) {
		return NULL;
	}
	std::pair<std::pair<XML_Char const *, bool> *, std::pair<XML_Char const *, bool> *> * const entry =
			container->at(index);

	// NOTE: getX() just peeps at data so don't clone anything
	std::pair<XML_Char const *, XML_Char const *> * const res =
			new std::pair<XML_Char const *, XML_Char const *>(
				entry->first->first, entry->second->first);
	return res;
}


/*static*/ const XspfExtension *
XspfData::getHelper(std::deque<std::pair<XspfExtension const *, bool> *> * &
		container, int index) {
	if ((container == NULL) || container->empty() || (index < 0)
			|| (index >= static_cast<int>(container->size()))) {
		return NULL;
	}

	// NOTE: getX() just peeps at data so don't clone anything
	std::pair<XspfExtension const *, bool> * const entry
			= container->at(index);
	return entry->first;
}


void
XspfData::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
