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
 * @file Xspf.h
 * Include-everything header for lazy people with fast machines.
 */

#ifndef XSPF_H
#define XSPF_H


/**
 * @namespace Xspf
 * Provides reading and writing functionality
 * for both XSPF-0 and XSPF-1.
 */

// #include "Xspf.h"
   #include "XspfChunkCallback.h"
// #include "XspfData.h"
   #include "XspfDateTime.h"
// #include "XspfDefines.h"
   #include "XspfExtension.h"
// #include "XspfExtensionReader.h"
   #include "XspfExtensionReaderFactory.h"
   #include "XspfExtensionWriter.h"
   #include "XspfIndentFormatter.h"
   #include "XspfProps.h"
   #include "XspfReader.h"
   #include "XspfReaderCallback.h"
   #include "XspfSeamlessFormatter.h"
   #include "XspfStack.h"
// #include "XspfToolbox.h"
   #include "XspfTrack.h"
// #include "XspfVersion.h"
   #include "XspfWriter.h"
// #include "XspfXmlFormatter.h"


#endif // XSPF_H
