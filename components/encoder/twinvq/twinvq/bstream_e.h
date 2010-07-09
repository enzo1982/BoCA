/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */
/*                Released on 2000.09.06 by N. Iwakami */


#ifndef bitstream_e_h
#define bitstream_e_h

#include "Chunk.h"


extern void TvqWriteBsFrame(INDEX  *indexp,
							BFILE	*bfp);        /* Input:  Data file pointer */

extern void TvqInitBsWriter();

extern void TvqPutBsHeaderInfo( BFILE *bfp, CChunkChunk& twinChunk );


#endif
