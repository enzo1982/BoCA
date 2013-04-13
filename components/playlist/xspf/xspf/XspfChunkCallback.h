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
 * @file XspfChunkCallback.h
 * Interface of XspfChunkCallback.
 */

#ifndef XSPF_CHUNK_CALLBACK_H
#define XSPF_CHUNK_CALLBACK_H


namespace Xspf {


class XspfChunkCallbackPrivate;


/**
 * Hands out text chunks to a XspfReader.
 * This enables parsing from non-linear input
 * structures. It can also be used to parse
 * bug XSPF files in smaller chunks to lower
 * memory usage.
 */
class XspfChunkCallback {

	friend class XspfReader;

private:
	/// @cond DOXYGEN_NON_API
	XspfChunkCallbackPrivate * const d; ///< D pointer
	/// @endcond

protected:
	/**
	 * Creates a new chunk callback.
	 */
	XspfChunkCallback();

	/**
	 * Copy constructor.
	 *
	 * @param source  Source to copy from
	 */
	XspfChunkCallback(XspfChunkCallback const & source);

	/**
	 * Assignment operator.
	 *
	 * @param source  Source to copy from
	 */
	XspfChunkCallback & operator=(XspfChunkCallback const & source);

	/**
	 * Destroys this chunk callback.
	 */
	virtual ~XspfChunkCallback();

	/**
	 * Returns the minimum size in bytes of the
	 * buffer handed to fillBuffer in a subsequent
	 * call.
	 *
	 * @return  Size in bytes
	 */
	virtual int getMinimumBufferByteSize() = 0;

	/**
	 * Fulls <code>buffer</code> with a new
	 * chunk of input. the chunk copies must
	 * not exceed the number of bytes returned
	 * by the last call to getMinimumBufferByteSize.
	 * NOTE: You do <b>not</b> have to zero-terminate
	 * the string written. If you do so do not count
	 * this into the return value.
	 *
	 * @param buffer  Buffer to write to
	 * @return        Number of bytes actually written
	 */
	virtual int fillBuffer(void * buffer) = 0;

	/**
	 * Notifies this chunk callback, that
	 * no more chunks will be needed. You can close
	 * the resource read from in here.
	 */
	virtual void notifyStop();

	/// @cond DOXYGEN_NON_API
	void virtualHook(int methodId, void * parameters);
	/// @endcond

};


}

#endif // XSPF_CHUNK_CALLBACK_H
