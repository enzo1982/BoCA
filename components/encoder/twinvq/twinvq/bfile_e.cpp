/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */

#include <stdio.h>
#include "twinvq.h"
#include "bfile.h"

#define	BITS_INT	(sizeof(int)*8)
#define ASCII_STREAM    0

int put_strm(int   data,          /* Input: input data */
	     int   nbits,         /* Input: number of bits */
	     BFILE *bfp)          /* Input: bit file pointer */
{
    int		ibit;
    int		mask;
    char	tmpbit[BITS_INT];
    int		rtflag;

    rtflag = 0;
    if ( (0x1<<nbits)<= data ){
	rtflag = 1;
    }
    mask = 0x1;
#if ASCII_STREAM
    for ( ibit=0; ibit<nbits; ibit++ ){
         tmpbit[ibit] = (data>>(nbits-1-ibit)) & mask;
         fprintf(bfp->fp, "%d", tmpbit[ibit]);
    }
#else
    for ( ibit=0; ibit<nbits; ibit++ ){
	tmpbit[ibit] = (data>>(nbits-1-ibit)) & mask;
    }
    bwrite( tmpbit, sizeof(*tmpbit), nbits, bfp );
#endif
    return(rtflag);
}

void bwrite(char  *data,		/* Input: Input data array */
	    int	  size,		/* Input: Length of each data */
	    int	  nbits,		/* Input: Number of bits to write */
	    BFILE *stream)	/* Input: File pointer */
{
    /*--- Variables ---*/
    int	 ibits, iptr, idata, ibufadr, ibufbit;
    unsigned char mask, tmpdat;

    /*--- Main operation ---*/
    mask = 0x1;
    for ( ibits=0; ibits<nbits; ibits++ ){
	idata = ibits*size;           /* input data address */
	tmpdat = data[idata]&mask;    /* input data */
	iptr = stream->ptr;           /* current file data buffer pointer */
	ibufadr = iptr/BYTE_BIT;      /* current file data buffer address */
	ibufbit = iptr%BYTE_BIT;      /* current file data buffer bit */
	tmpdat <<= (BYTE_BIT-ibufbit-1);
	             /* shift input data to current file data buffer bit */
	stream->buf[ibufadr] = (char)(stream->buf[ibufadr]|tmpdat);
	                              /* set data into file data buffer */
	++stream->ptr;                /* increment data buffer pointer */
	if ( stream->ptr == BBUFLEN ){     /* when file data buffer is full */
	    fwrite( stream->buf, 1, BBUFSIZ, stream->fp );
	    for ( ibufadr=0; ibufadr<BBUFSIZ; ibufadr++ )
		stream->buf[ibufadr] = 0;
	    stream->ptr = 0;
	}
    }
}
