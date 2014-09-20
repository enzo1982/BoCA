/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */
/*                Released on 2000.09.06 by N. Iwakami */

/* bstream.cxx: TwinVQ bitstream reader */
/* 24 Feb. 1999, changed header format by N. Iwakami */
/* 24 Feb. 1999, added function TvqSkipFrame() and TvqGetBsFramePoint() by N. Iwakami */
/* 17 Jun. 1999, bug fix at the function TvqSkipFrame() by N. Iwakami */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "twinvq.h"
#include "tvqdec.h"
#include "bstream.h"

#include "../dllinterface.h"

#define DEBUG_PRINT 0

/*----------------------------------------------------------------------------*/
/* bits table for VQ                                                          */
/*----------------------------------------------------------------------------*/
static int *bits_0[N_INTR_TYPE], *bits_1[N_INTR_TYPE];

/*----------------------------------------------------------------------------*/
/* lookup parameters                                                          */
/*----------------------------------------------------------------------------*/
static tvqConfInfo cf;
static int iframe;
static int TVQ_VERSION;

/*----------------------------------------------------------------------------*/
/* Name:        get_string()                                                  */
/* Description: get string from the bitstream file                            */
/* Return:      none                                                          */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int get_string (char *buf, int nbytes, BFILE *bfp)
{
	int c, ichar, ibit;
	
	for ( ichar=0; ichar<nbytes; ichar++ ) {
		ibit = get_bstm( &c, CHAR_BITS, bfp );
		if ( ibit < CHAR_BITS) {
			break;
		}
		buf[ichar] = c;
	}

	buf[ichar] = '\0';
	return ichar;
}

/*----------------------------------------------------------------------------*/
/* Name:        LoadTwinChunk()                                               */
/* Description: load the TWIN chunk from a bitstream file                     */
/* Return:      (CChunkChunk) the TWIN chunk                                  */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
CChunkChunk* LoadTwinChunk( BFILE *bfp )
{
	int chunkSize;

	// チャンク ID を読んで、バージョンをチェックする
	char chunkID[KEYWORD_BYTES+VERSION_BYTES+1];
	get_string ( chunkID, KEYWORD_BYTES+VERSION_BYTES, bfp );
	TVQ_VERSION = ex_TvqCheckVersion( chunkID );
	if ( TVQ_VERSION == TVQ_UNKNOWN_VERSION ) {
#if DEBUG_PRINT > 0 
		fprintf( stderr, "Header reading error: Unknown version (%s).\n", chunkID );
#endif
		return NULL;
	}

	// チャンクサイズを読む。
	if ( get_bstm( &chunkSize, ELEM_BYTES * CHAR_BITS, bfp ) <= 0 ){
#if DEBUG_PRINT > 0 
		fprintf( stderr, "Header reading error: Failed to get header size.\n" );
#endif
		return NULL;
	};

	// チャンクサイズ分だけデータを読む
	char *chunkData = new char[chunkSize+1];
	if ( get_string ( chunkData, chunkSize, bfp ) < chunkSize ) {
#if DEBUG_PRINT > 0 
		fprintf( stderr, "Header reading error: Failed to read header data.\n" );
#endif
		delete[] chunkData;

		return NULL;
	}

	// TWIN チャンクを生成し、データを書き込む
	CChunkChunk* TwinChunk = new CChunkChunk( chunkID );
	TwinChunk->PutData( chunkSize, chunkData );

	delete[] chunkData;

	return TwinChunk;
}

/*============================================================================*/
/* Name:        TvqGetHeaderInfo()                                            */
/* Description: read bitstream header and create the header chunk data        */
/* Return:      returns 1 when error or 0                                     */
/* Access:      external                                                      */
/*============================================================================*/
CChunkChunk* TvqGetBsHeaderInfo(BFILE *bfp)             // Input:  bitstream file pointer
{

	// TwinVQ ヘッダをファイルから読み込み、ヘッダデータ構造体を得る
	CChunkChunk* twinChunk = LoadTwinChunk( bfp );
	if ( twinChunk == NULL ) {
		fprintf( stderr, "Failed to read header. Check the bitstream file.\n" );
		return NULL;
	}

	// "DATA" チャンクヘッダを空読み
	char lbuf[BUFSIZ];
	get_string(lbuf, KEYWORD_BYTES, bfp);
	if ( strcmp ( lbuf, "DATA" ) ) {
		fprintf ( stderr, "TwinVQ format error. No \"DATA\" chunk was found.\n" );
		delete twinChunk;
		return NULL;
	}

	return twinChunk;
}

/*============================================================================*/
/* Name:        TvqInitBsReader()                                             */
/* Description: initialize the bitstream reader                               */
/* Return:      Returns 1 when error or returns 0                             */
/* Access:      external                                                      */
/*============================================================================*/
int TvqInitBsReader( headerInfo *setupInfo )
{

	TVQ_VERSION = ex_TvqCheckVersion( setupInfo->ID );
	if ( TVQ_VERSION == TVQ_UNKNOWN_VERSION ) {
		return 1;
	}
	
	ex_TvqGetConfInfo(&cf);

	ex_TvqGetVectorInfo( bits_0, bits_1 );
	
	iframe = 0;

	return 0;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetVqInfo()                                                   */
/* Description: get VQ information                                            */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetVqInfo( tvqConfInfoSubBlock *cfg,
			  int bits0[],
			  int bits1[],
			  int variableBits,
			  INDEX *index,
			  BFILE *bfp)
{
	int idiv;
	int bitcount = 0;

	if ( index->btype == BLK_LONG ){
		ex_TvqUpdateVectorInfo( variableBits, &cfg->ndiv, bits0, bits1 ); // re-calculate VQ bits
	}
	for ( idiv=0; idiv<cfg->ndiv; idiv++ ){
		bitcount += get_bstm( &index->wvq[idiv], bits0[idiv], bfp );       /* CB 0 */
		bitcount += get_bstm( &index->wvq[idiv + cfg->ndiv], bits1[idiv], bfp ); /* CB 1 */
	}

	return bitcount;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetBseInfo()                                                  */
/* Description: get BSE information                                           */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetBseInfo( tvqConfInfo *cf, tvqConfInfoSubBlock *cfg, INDEX *index, BFILE *bfp )
{
	int i_sup, isf, itmp, idiv;
	int bitcount = 0;

	for ( i_sup=0; i_sup<cf->N_CH; i_sup++ ){
		for ( isf=0; isf<cfg->nsf; isf++ ){
			for ( idiv=0; idiv<cfg->fw_ndiv; idiv++ ){
				itmp = idiv + ( isf + i_sup * cfg->nsf ) * cfg->fw_ndiv;
				bitcount += get_bstm( &index->fw[itmp], cfg->fw_nbit, bfp );
			}
		}
	}
	for ( i_sup=0; i_sup<cf->N_CH; i_sup++ ){
		for ( isf=0; isf<cfg->nsf; isf++ ){
			bitcount += get_bstm( &index->fw_alf[i_sup * cfg->nsf + isf], cf->FW_ARSW_BITS, bfp);
		}
	}

	return bitcount;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetGainInfo()                                                 */
/* Description: get gain information                                          */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetGainInfo( tvqConfInfo *cf, tvqConfInfoSubBlock *cfg, INDEX *index, BFILE *bfp )
{
	int i_sup, iptop, isf;
	int bitcount = 0;

	for ( i_sup=0; i_sup<cf->N_CH; i_sup++ ){
		iptop = ( cfg->nsubg + 1 ) * i_sup;
		bitcount += get_bstm( &index->pow[iptop], cf->GAIN_BITS, bfp );
		for ( isf=0; isf<cfg->nsubg; isf++ ){
			bitcount += get_bstm( &index->pow[iptop+isf+1], cf->SUB_GAIN_BITS, bfp );
		}
	}

	return bitcount;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetLspInfo()                                                  */
/* Description: get LSP information                                           */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetLspInfo( tvqConfInfo *cf, INDEX *index, BFILE *bfp )
{
	int i_sup, itmp;
	int bitcount = 0;

	for ( i_sup=0; i_sup<cf->N_CH; i_sup++ ){
		bitcount += get_bstm( &index->lsp[i_sup][0], cf->LSP_BIT0, bfp ); /* pred. switch */
		bitcount += get_bstm( &index->lsp[i_sup][1], cf->LSP_BIT1, bfp ); /* first stage */
		for ( itmp=0; itmp<cf->LSP_SPLIT; itmp++ ){         /* second stage */
			bitcount += get_bstm( &index->lsp[i_sup][itmp+2], cf->LSP_BIT2, bfp );
		}
	}

	return bitcount;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetPpcInfo()                                                  */
/* Description: get PPC information                                           */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetPpcInfo( tvqConfInfo *cf, INDEX *index, BFILE *bfp )
{
	int idiv, i_sup;
	int bitcount = 0;
	
	for ( idiv=0; idiv<cf->N_DIV_P; idiv++ ){
		bitcount += get_bstm( &(index->pls[idiv]), bits_0[BLK_PPC][idiv], bfp );       /*CB0*/
		bitcount += get_bstm( &(index->pls[idiv+cf->N_DIV_P]), bits_1[BLK_PPC][idiv], bfp);/*CB1*/
	}
	for (i_sup=0; i_sup<cf->N_CH; i_sup++){
		bitcount += get_bstm(&(index->pit[i_sup]), cf->BASF_BIT, bfp);
		bitcount += get_bstm(&(index->pgain[i_sup]), cf->PGAIN_BIT, bfp);
	}
	
	return bitcount;
}

/*----------------------------------------------------------------------------*/
/* Name:        GetEbcInfo()                                                  */
/* Description: get EBC information                                           */
/* Return:      (int) number of bits read                                     */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int GetEbcInfo( tvqConfInfo *cf, tvqConfInfoSubBlock *cfg, INDEX *index, BFILE *bfp )
{
	int i_sup, isf, itmp;
	int bitcount = 0;

	for ( i_sup=0; i_sup<cf->N_CH; i_sup++ ){
		for ( isf=0; isf<cfg->nsf; isf++){
			int indexSfOffset = isf * ( cfg->ncrb - cfg->ebc_crb_base ) - cfg->ebc_crb_base;
			for ( itmp=cfg->ebc_crb_base; itmp<cfg->ncrb; itmp++ ){
				bitcount += get_bstm( &index->bc[i_sup][itmp+indexSfOffset], cfg->ebc_bits, bfp);
			}
		}
	}
	
	return bitcount;
}

/*============================================================================*/
/* Name:        TvqReadBsFrame()                                              */
/* Description: read bitstream frame                                          */
/* Return:      (int) 1: successful reading, 0: imcompleted reading           */
/* Access:      external                                                      */
/*============================================================================*/
int TvqReadBsFrame(INDEX *index, /* Output: quantization indexes */
				   BFILE *bfp)	 /* Input:  bitstream file pointer */
{
	/*--- Variables ---*/
	tvqConfInfoSubBlock *cfg;
	int variableBits;
	int bitcount;
	int numFixedBitsPerFrame = ex_TvqGetNumFixedBitsPerFrame();
	
	/*--- Initialization ---*/
	variableBits = 0;
	bitcount = 0;

	/*--- read block independent factors ---*/
	/* Window type */
	bitcount += get_bstm( &index->w_type, cf.BITS_WTYPE, bfp );
	if ( ex_TvqWtypeToBtype( index->w_type, &index->btype ) ) {
		fprintf( stderr, "Error: unknown window type: %d\n", index->w_type );
		return 0;
	}
	int btype = index->btype;

	/*--- read block dependent factors ---*/
	cfg = &cf.cfg[btype]; // set the block dependent paremeters table

	bitcount += variableBits;
	
	/* Interleaved vector quantization */
	bitcount += GetVqInfo( cfg, bits_0[btype], bits_1[btype], variableBits, index, bfp );
	
	/* Bark-scale envelope */
	bitcount += GetBseInfo( &cf, cfg, index, bfp );
	/* Gain */
	bitcount += GetGainInfo( &cf, cfg, index, bfp );
	/* LSP */
	bitcount += GetLspInfo( &cf, index, bfp );
	/* PPC */
	if ( cfg->ppc_enable ){
		bitcount += GetPpcInfo( &cf, index, bfp );
	}
	/* Energy Balance Calibration */
	if ( cfg->ebc_enable ){
		bitcount += GetEbcInfo( &cf, cfg, index, bfp );
	}
	
	iframe += 1;

	return bitcount == numFixedBitsPerFrame ? 1 : 0;
	
}


/*============================================================================*/
/* Name:        TvqSkipFrame()                                                */
/* Description: skip frame                                                    */
/* Return:      (int)                                                         */
/* Access:      external                                                      */
/*============================================================================*/
int TvqSkipFrame(BFILE *bfp, long step)
{
	
	int numBits = ex_TvqGetNumFixedBitsPerFrame();
	iframe += step;
	return bseek(bfp, numBits*step, BSEEK_CUR);

}

/*============================================================================*/
/* Name:        TvqGetFramePoint()                                            */
/* Description: returns current frame point                                   */
/* Return:      (int) current frame point                                     */
/* Access:      external                                                      */
/*============================================================================*/
int TvqGetBsFramePoint()
{
	return iframe;
}

