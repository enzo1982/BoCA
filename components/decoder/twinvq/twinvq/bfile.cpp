/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */

/* bfile.cxx: bitstream file operating tools */
/* Version 0.0 written by N.Iwakami on 1994/4/21 */
/* Version 1.0 modified by N.Iwakami on 1999/1/8 */
/* Version 1.1 added function bseek() by N.Iwakami on 1999/2/24 */
/* Version 1.2 bugfix about skipping frames by NI, 1999/6/18 */
/* Version 1.3 bugfix about skipping frames by NI, 1999/6/24 */

#define BITSTREAM_MAIN

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bfile.h"

BFILE *bopen(char	*name,	/* Input: Data file name */
			 char	*mode )	/* Input: Access mode */
{
    BFILE	*bfp;
    int		i;

	/*--- memory allocation ---*/
    bfp = tealloc();
	
    /*--- Open file ---*/
    if ( (bfp->fp=fopen(name,mode)) == NULL ){
		bfp = NULL;
		return(bfp);
    }
    /*--- Clear pointer ---*/
    bfp->ptr=0;
	/*--- Reset the readable flag ---*/
	bfp->readable = 0;
    /*--- Set mode ---*/
    bfp->mode = mode;
    /*--- Clear buffer ---*/
    for ( i=0; i<BBUFSIZ; i++ ) bfp->buf[i] = 0;
    return(bfp);
}

void bclose(BFILE *bfp)
{
    int		iadr;
	
    /*--- If mode is "w" output data remaining in the buffer */
    if ( strstr(bfp->mode,"w") != NULL ){
		iadr = (bfp->ptr+BYTE_BIT-1)/BYTE_BIT;
		fwrite( bfp->buf, 1, iadr, bfp->fp );
    }
    /*--- Close file ---*/
    fclose( bfp->fp );
	
    tefree( bfp );
	
}

int bseek(BFILE *stream, long offset, int origin)
{
	int fs_ret;

	if (strstr(stream->mode,"w") != NULL){
		fprintf(stderr, "bseek(): No seek support in write mode.\n");
		return 2;
    }

	switch(origin){
	case BSEEK_CUR:
		{
			int offsetTest = stream->ptr + offset;
			if (0 <= offsetTest && offsetTest < stream->nbuf){ // if seeking is within the buffer
				stream->ptr = offsetTest;
			}
			else{
				if (offsetTest >= stream->nbuf){ // forward seek
					int nSeekStep = offsetTest/BBUFLEN;
					if (stream->readable == 1){
						nSeekStep -= 1;
					}
					fs_ret = fseek(stream->fp, nSeekStep*BBUFSIZ, SEEK_CUR);
					if (fs_ret != 0) return fs_ret;

					stream->readable = 0;
					stream->ptr = offsetTest % BBUFLEN;
				}
				else{ // backward seek
					int nSeekStep = (offsetTest-(BBUFLEN-1))/BBUFLEN;
					if (stream->readable == 1){
						nSeekStep -= 1;
					}
					fs_ret =
						fseek(stream->fp, nSeekStep*BBUFSIZ, SEEK_CUR);

					if (fs_ret != 0) return fs_ret;

					stream->readable = 0;
					stream->ptr = (offsetTest - nSeekStep * BBUFLEN) % BBUFLEN;
				}
			}
		}
		break;
	case BSEEK_SET: // top of the file
		if (offset < 0) return 1;
		fs_ret = fseek(stream->fp, (offset/BBUFLEN)*BBUFSIZ, SEEK_SET);
		if (fs_ret != 0) return fs_ret;
		stream->readable = 0;
		stream->nbuf *= 8;
		stream->ptr = offset % BBUFLEN;
		break;
	case BSEEK_END:
		if (offset > 0) return 1;
		fs_ret = fseek(stream->fp, ((offset-BBUFLEN+1)/BBUFLEN-1)*BBUFSIZ, SEEK_END);
		if (fs_ret != 0) return fs_ret;
		stream->readable = 0;
		stream->nbuf *= 8;
		stream->ptr = (offset + BBUFLEN) % BBUFLEN;
		break;
	default:
		fprintf(stderr, "bseek(): %d: Invalid origin ID.\n", origin);
		return 2;
	}
	return 0;
}

int bread(char	*data,    /* Output: Output data array */
		  int	size,     /* Input:  Length of each data */
		  int	nbits,    /* Input:  Number of bits to write */
		  BFILE	*stream ) /* Input:  File pointer */
{
    /*--- Variables ---*/
    int	 ibits, iptr, idata, ibufadr, ibufbit, icl;
    unsigned char mask, tmpdat;
    int  retval;
	
    /*--- Main operation ---*/
    retval = 0;
    mask = 0x1;
    for ( ibits=0; ibits<nbits; ibits++ ){
		if ( stream->readable == 0 ){  /* when the file data buffer is empty */
			stream->nbuf = fread( stream->buf, 1, BBUFSIZ, stream->fp );
			stream->nbuf *= 8;
			stream->readable = 1;
		}
		iptr = stream->ptr;           /* current file data buffer pointer */
		if ( iptr >= stream->nbuf )   /* If data file is empty then return */
			return(retval);
		ibufadr = iptr/BYTE_BIT;      /* current file data buffer address */
		ibufbit = iptr%BYTE_BIT;      /* current file data buffer bit */
		/*	tmpdat = stream->buf[ibufadr] >> (BYTE_BIT-ibufbit-1); */
		tmpdat = (unsigned char)stream->buf[ibufadr];
		tmpdat >>= (BYTE_BIT-ibufbit-1);
		/* current data bit */
		
		idata = ibits*size;                   /* output data address */
		data[idata] = (char)(tmpdat & mask);  /* set output data */
		for (icl=1; icl<size; icl++)
			data[idata+icl] = 0; /* clear the rest output data buffer */
		stream->ptr += 1;       /* update data buffer pointer */
		if (stream->ptr == BBUFLEN){
			stream->ptr = 0;
			stream->readable = 0;
		}
		++retval;
    }
    return(retval);
}

BFILE *tealloc()
{
    int	ifile;
    static int	InitFlag=1;
    BFILE	*bfp_tmp;
	
    if ( InitFlag ){
		for ( ifile=0; ifile<N_BFILE; ifile++ ) _bfile_flag[ifile] = 0;
		InitFlag = 0;
    }
	
    for ( ifile=0; ifile<N_BFILE; ifile++ ){
		if ( !_bfile_flag[ifile] ){
			_bfile_flag[ifile] = 1;
			bfp_tmp = &_bfp_mem[ifile];
			bfp_tmp->_file_id=ifile;
			return(bfp_tmp);
		}
    }
    fprintf( stderr,"tealloc(): File number exceeds upper limit.\n" );
    exit(1);
    return(0);
}

void tefree( BFILE *bfp )
{
    int	ifile;
	
    ifile = bfp->_file_id;
    _bfile_flag[ifile] = 0;
	
}

#define	BITS_INT	(sizeof(int)*8)

int get_bstm(int	*data,          /* Input: input data */
			 int	nbits,         /* Input: number of bits */
			 BFILE	*bfp)          /* Input: bit file pointer */
{
    int		ibit;
    unsigned	mask;
    unsigned	work;
    char	tmpbit[BITS_INT];
    int		retval;
	
    if ( nbits > (signed) BITS_INT ){
		fprintf( stderr, "get_bstm(): %d: %d Error.\n",
			nbits, BITS_INT);
		exit(1);
    }
    retval = bread( tmpbit, sizeof(*tmpbit), nbits, bfp );
    for (ibit=retval; ibit<nbits; ibit++){
		tmpbit[ibit] = 0;
    }
    mask = 0x1<<(nbits-1);
    work=0;
    for ( ibit=0; ibit<nbits; ibit++ ){
		work += mask*tmpbit[ibit];
		mask >>= 1;
    }
    *data = work;
    return(retval);
}
