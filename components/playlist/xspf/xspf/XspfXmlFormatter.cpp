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
 * @file XspfXmlFormatter.cpp
 * Implementation of XspfXmlFormatter.
 */

#include <xspf/XspfXmlFormatter.h>
#include <xspf/XspfToolbox.h>
#include <map>
#include <list>
#include <set>
#include <cstdio>

namespace Xspf {


/*static*/ XML_Char const * const XspfXmlFormatter::namespaceKey
		= XSPF_NS_HOME;


/// @cond DOXYGEN_NON_API

/**
 * D object for XspfXmlFormatter.
 */
class XspfXmlFormatterPrivate {

	friend class XspfXmlFormatter;

	int level; ///< Nesting level, 0 before root element has been written, 1 after that
	std::map<XML_Char const *, XML_Char *, Toolbox::XspfStringCompare> namespaceToPrefix; ///< Namespace prefix map
	std::list<XspfNamespaceRegistrationUndo *> undo; ///< Ordered registration undo list
	std::set<XML_Char const *, Toolbox::XspfStringCompare> prefixPool; ///< Set of registered prefixes
	bool declarationWritten; ///< XML declaration written flag
	std::basic_ostringstream<XML_Char> * output; ///< Output accumulator

	/**
	 * Creates a new D object.
	 */
	XspfXmlFormatterPrivate() :
			level(0),
			namespaceToPrefix(),
			undo(),
			prefixPool(),
			declarationWritten(false),
			output(NULL) {

	}

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfXmlFormatterPrivate(XspfXmlFormatterPrivate const & source)
			: level(source.level),
			namespaceToPrefix(),
			undo(),
			prefixPool(),
			declarationWritten(source.declarationWritten),
			output(source.output) {
		assign(source);
	}

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfXmlFormatterPrivate & operator=(XspfXmlFormatterPrivate const & source) {
		if (this != &source) {
			this->level = source.level;
			freeMap(this->namespaceToPrefix);
			freeList(this->undo);
			this->prefixPool.clear(); // Same strings again, don'tdelet twice
			this->declarationWritten = source.declarationWritten;
			this->output = source.output;
			assign(source);
		}
		return *this;
	}

	/**
	 * Destroys this D object.
	 */
	~XspfXmlFormatterPrivate() {
		freeMap(this->namespaceToPrefix);
		freeList(this->undo);
		this->prefixPool.clear(); // Same strings again, don'tdelet twice
	}

	void assign(XspfXmlFormatterPrivate const & source) {
		std::map<XML_Char const *, XML_Char *, Toolbox::XspfStringCompare>
				::const_iterator iter = source.namespaceToPrefix.begin();
		while (iter != source.namespaceToPrefix.end()) {
			XML_Char const * const uri = iter->first;
			XML_Char * const prefix = iter->second;
			registerNamespace(uri, prefix);
			iter++;
		}
	}

	static void freeMap(std::map<XML_Char const *, XML_Char *,
			Toolbox::XspfStringCompare> & container) {
		std::map<XML_Char const *, XML_Char *,
				Toolbox::XspfStringCompare>::iterator
				iter = container.begin();
		while (iter != container.end()) {
			delete [] iter->second;
			iter++;
		}
		container.clear();
	}

	static void freeList(std::list<
			XspfNamespaceRegistrationUndo *> & container) {
		std::list<XspfNamespaceRegistrationUndo *>::iterator
				iter = container.begin();
		while (iter != container.end()) {
			XspfNamespaceRegistrationUndo * const entry = *iter;
			delete entry;
			iter++;
		}
		container.clear();
	}

	bool registerNamespace(XML_Char const * uri,
			XML_Char const * prefixSuggestion) {
		// Uri registered already?
		std::map<XML_Char const *, XML_Char *, Toolbox::XspfStringCompare>
				::iterator found = this->namespaceToPrefix.find(uri);
		if (found != this->namespaceToPrefix.end()) {
			return false; // == Existing
		}

		// Find unbound prefix (appending "x" if occupied)
		XML_Char * testPrefix = Toolbox::newAndCopy(prefixSuggestion);
		while (this->prefixPool.find(testPrefix) != this->prefixPool.end()) {
			int const testPrefixLen = static_cast<int>(::PORT_STRLEN(testPrefix));
			int const charCount = testPrefixLen + 1 + 1;
			XML_Char * nextPrefix = new XML_Char[charCount];
			::PORT_SNPRINTF(nextPrefix, charCount, _PT("%sx"), testPrefix);
			delete [] testPrefix;
			testPrefix = nextPrefix;
		}

		// Add prefix to map and pool
		this->namespaceToPrefix.insert(std::pair<XML_Char const *,
				XML_Char *>(uri, testPrefix));
		this->prefixPool.insert(testPrefix);

		// Create undo entry
		XspfNamespaceRegistrationUndo * undo
					= new XspfNamespaceRegistrationUndo(this->level, uri);
		this->undo.push_front(undo);

		return true; // == Added
	}

};

/// @endcond


XspfXmlFormatter::XspfXmlFormatter()
		: d(new XspfXmlFormatterPrivate()) {

}


XspfXmlFormatter::XspfXmlFormatter(XspfXmlFormatter const & source)
		: d(new XspfXmlFormatterPrivate(*(source.d))) {

}


XspfXmlFormatter &
XspfXmlFormatter::operator=(XspfXmlFormatter const & source) {
	if (this != &source) {
		*(this->d) = *(source.d);
	}
	return *this;
}


XspfXmlFormatter::~XspfXmlFormatter() {
	delete this->d;
}


void
XspfXmlFormatter::writeXmlDeclaration() {
	if (!this->d->declarationWritten) {
		*this->d->output << _PT("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
		this->d->declarationWritten = true;
	}
}


const XML_Char *
XspfXmlFormatter::getPrefix(XML_Char const * nsUri) const {
	std::map<XML_Char const *, XML_Char *, Toolbox::XspfStringCompare>
			::const_iterator found = this->d->namespaceToPrefix.find(nsUri);
	if (found != this->d->namespaceToPrefix.end()) {
		return found->second;
	} else {
		return NULL;
	}
}


XML_Char *
XspfXmlFormatter::makeFullName(XML_Char const * nsUri,
		XML_Char const * localName) const {
	XML_Char const * const prefix = getPrefix(nsUri);
	if (prefix != NULL) {
		int const prefixLen = static_cast<int>(::PORT_STRLEN(prefix));
		int const localNameLen = static_cast<int>(::PORT_STRLEN(localName));
		XML_Char * fullName = NULL;
		if (prefixLen == 0) {
			// Default namespace
			fullName = new XML_Char[localNameLen + 1];
			::PORT_STRCPY(fullName, localName);
		} else {
			// Namespace with prefix
			fullName = new XML_Char[prefixLen + 1 + localNameLen + 1];
			::PORT_STRCPY(fullName, prefix);
			::PORT_STRCPY(fullName + prefixLen, _PT(":"));
			::PORT_STRCPY(fullName + prefixLen + 1, localName);
		}
		return fullName;
	} else {
		// TODO What exactly do we do with unregistered
		// namespace URIs? Register a new prefix and use it?
		return Toolbox::newAndCopy(localName);
	}
}


bool
XspfXmlFormatter::registerNamespace(XML_Char const * uri,
		XML_Char const * prefixSuggestion) {
	return this->d->registerNamespace(uri, prefixSuggestion);
}


void
XspfXmlFormatter::cleanupNamespaceRegs() {
	std::list<XspfNamespaceRegistrationUndo *>::iterator iter = this->d->undo.begin();
	while (iter != this->d->undo.end()) {
		XspfNamespaceRegistrationUndo * const entry = *iter;
		if (entry->level >= this->d->level) {
			std::map<XML_Char const *, XML_Char *, Toolbox::XspfStringCompare>
				::iterator foundUri = this->d->namespaceToPrefix.find(entry->uri);
			if (foundUri != this->d->namespaceToPrefix.end()) {
				XML_Char * & prefix = foundUri->second;

				// Remove prefix
				std::set<XML_Char const *, Toolbox::XspfStringCompare>::iterator foundPrefix
						= this->d->prefixPool.find(prefix);
				if (foundPrefix != this->d->prefixPool.end()) {
					this->d->prefixPool.erase(foundPrefix);
				}
				delete [] prefix;

				// Remove URI
				this->d->namespaceToPrefix.erase(foundUri);
			}
			this->d->undo.erase(iter);
			delete entry;
		} else {
			break;
		}
		iter = this->d->undo.begin();
	}
}


void
XspfXmlFormatter::setOutput(std::basic_ostringstream<XML_Char> & output) {
	this->d->output = &output;
}


void
XspfXmlFormatter::writeStart(XML_Char const * ns,
		XML_Char const * localName, XML_Char const * const * atts,
		XML_Char const * const * nsRegs) {
	if (nsRegs != NULL) {
		std::list<std::pair<XML_Char const *, XML_Char const *> > attribs;

		// Process namespace registrations
		XML_Char const * const * nsRegsWalk = nsRegs;
		while (nsRegsWalk[0] != NULL) {
			XML_Char const * const & uri = nsRegsWalk[0];
			XML_Char const * const & prefix = nsRegsWalk[1];

			// New namespace?
			if (registerNamespace(uri, prefix)) {
				XML_Char const * const finalPrefix = getPrefix(uri);

				XML_Char * finalKey = NULL;
				if (::PORT_STRLEN(finalPrefix) == 0) {
					// Default namespace
					finalKey = new XML_Char[5 + 1];
					::PORT_STRCPY(finalKey, _PT("xmlns"));
				} else {
					// Namespace with prefix
					int const finalPrefixLen = static_cast<int>(::PORT_STRLEN(finalPrefix));
					finalKey = new XML_Char[5 + 1 + finalPrefixLen + 1];
					::PORT_STRCPY(finalKey, _PT("xmlns:"));
					::PORT_STRCPY(finalKey + 6, finalPrefix);
				}

				attribs.push_back(std::pair<XML_Char const *, XML_Char const *>(finalKey, uri));
			}

			nsRegsWalk += 2;
		}

		// Append normal attributes
		XML_Char const * const * attsWalk = atts;
		while (attsWalk[0] != NULL) {
			// TODO Copying the first is a lazy hack. Improve.
			attribs.push_back(std::pair<XML_Char const *, XML_Char const *>(
					Toolbox::newAndCopy(attsWalk[0]), attsWalk[1]));
			attsWalk += 2;
		}

		// Convert
		int const attribCount = static_cast<int>(attribs.size());
		XML_Char const ** finalAtts = new const XML_Char *[2 * attribCount + 1];
		std::list<std::pair<XML_Char const *, XML_Char const *> >::iterator iter = attribs.begin();
		XML_Char const ** finalAttsWalk = finalAtts;
		while (iter != attribs.end()) {
			finalAttsWalk[0] = (*iter).first;
			finalAttsWalk[1] = (*iter).second;
			finalAttsWalk += 2;
			iter++;
		}
		finalAttsWalk[0] = NULL;

		// Write tag
		XML_Char const * fullName = makeFullName(ns, localName);
		writeStart(fullName, finalAtts);

		// Full cleanup
		delete [] fullName;
		finalAttsWalk = finalAtts;
		while (finalAttsWalk[0] != NULL) {
			delete [] finalAttsWalk[0];
			finalAttsWalk += 2;
		}
		delete [] finalAtts;
	} else {
		// No registrations
		XML_Char const * fullName = makeFullName(ns, localName);
		writeStart(fullName, atts);
		delete [] fullName;
	}

	this->d->level++;
}


void
XspfXmlFormatter::writeEnd(XML_Char const * ns,
		XML_Char const * localName) {
	XML_Char const * fullName = makeFullName(ns, localName);
	writeEnd(fullName);
	delete [] fullName;

	cleanupNamespaceRegs();

	this->d->level--;
}


void
XspfXmlFormatter::writeHomeStart(XML_Char const * localName,
		XML_Char const * const * atts, XML_Char const * const * nsRegs) {
	writeStart(XspfXmlFormatter::namespaceKey,
			localName, atts, nsRegs);
}


void
XspfXmlFormatter::writeHomeEnd(XML_Char const * localName) {
	writeEnd(XspfXmlFormatter::namespaceKey,
			localName);
}


void
XspfXmlFormatter::writeCharacterData(XML_Char const * data) {
	if (data == NULL) {
		return;
	}

	// Extensible Markup Language (XML) 1.0 (Fourth Edition)
	// 2.4 Character Data and Markup
	// http://www.w3.org/TR/REC-xml/#syntax
	XML_Char const * start = data;
	XML_Char const * end = data;

	for (;;) {
		switch (*end) {
		case _PT('\0'):
			this->d->output->write(start, static_cast<std::streamsize>(end - start));
			return;

		case _PT('<'):
			this->d->output->write(start, static_cast<std::streamsize>(end - start));
			*this->d->output << _PT("&lt;");
			end++;
			start = end;
			break;

		case _PT('&'):
			this->d->output->write(start, static_cast<std::streamsize>(end - start));
			*this->d->output << _PT("&amp;");
			end++;
			start = end;
			break;

		case _PT('\''):
			this->d->output->write(start, static_cast<std::streamsize>(end - start));
			*this->d->output << _PT("&apos;");
			end++;
			start = end;
			break;

		case _PT('"'):
			this->d->output->write(start, static_cast<std::streamsize>(end - start));
			*this->d->output << _PT("&quot;");
			end++;
			start = end;
			break;

		case _PT(']'):
			if ((*(end + 1) == _PT(']')) && (*(end + 2) == _PT('>'))) {
				this->d->output->write(start, static_cast<std::streamsize>(end - start));
				*this->d->output << _PT("]]&gt;");
				end += 3;
				start = end;
			} else {
				end++;
			}
			break;

		default:
			end++;
			break;

		}
	}
}


std::basic_ostringstream<XML_Char> * &
XspfXmlFormatter::getOutput() {
	return this->d->output;
}


void
XspfXmlFormatter::virtualHook(int /*methodId*/, void * /*parameters*/) { }


} // namespace Xspf
