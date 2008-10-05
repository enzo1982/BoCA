/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */
/*                Released on 2000.09.06 by N. Iwakami */

#include "bfile.h"
#include "Chunk.h"

extern CChunkChunk* TvqGetBsHeaderInfo(BFILE *bfp);
extern int TvqReadBsFrame(INDEX *index, BFILE *bfp);
extern int TvqInitBsReader( headerInfo *setupInfo );
extern int TvqSkipFrame(BFILE *bfp, long step);
extern int TvqGetBsFramePoint();

extern CChunkChunk* LoadTwinChunk( BFILE *bfp );
