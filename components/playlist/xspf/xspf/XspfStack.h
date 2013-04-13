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
 * @file XspfStack.h
 * Interface and implemenatation of XspfStack.
 */

#ifndef XSPF_STACK_H
#define XSPF_STACK_H


#include "XspfDefines.h"
#include <stack>

namespace Xspf {


/**
 * A generic stack data structure.
 */
template <class T>
class XspfStack : private std::stack<T> {

public:
	/// Holds the height of the stack
	typedef typename std::stack<T>::size_type size_type;

	/**
	 * Returns the number of elements on the stack.
	 */
	size_type size() const {
		return std::stack<T>::size();
	}

	/**
	 * Returns the topmost element on the stack.
	 */
	T const & top() const {
		return std::stack<T>::top();
	}

	/**
	 * Pushes @a value on top of the stack.
	 */
	void push(T const & value) {
		std::stack<T>::push(value);
	}

	/**
	 * Pops the topmost element off the stack.
	 */
	void pop() {
		std::stack<T>::pop();
	}

	/**
	 * Clears the stack.
	 */
	void clear() {
		std::stack<T>::c.clear();
	}
};


}

#endif // XSPF_STACK_H
