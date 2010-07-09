/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Released on 2000.05.22 by N. Iwakami */

/* bfile.h: definitions related to the bitstream file operating tools */
/* Version 1.1 added function bseek() by N.Iwakami on 1999/2/24 */
/* Version 1.2 added a member "readable" to structure BFILE
               to improve bseek reliability, by NI, 1999/6/18  */

#ifndef bitrw_h
#define bitrw_h
#include <stdio.h>

#define	BYTE_BIT	8
#define	BBUFSIZ		1024		/* Bit buffer size (bytes) */
#define	BBUFLEN		(BBUFSIZ*BYTE_BIT)	/* Bit buffer length (bits) */
#define	N_BFILE		50		/* Number of bit files */

#define CHAR_BITS 8

#define BSEEK_CUR SEEK_CUR
#define BSEEK_SET SEEK_SET
#define BSEEK_END SEEK_END

typedef struct {
    int     ptr;           /* current point in the bit buffer */
    int     nbuf;          /* bit buffer size */
	/* NI, 1999/6/18 */
	int     readable;      /* set to 1 if the bit buffer is readable */
    char    buf[BBUFSIZ];  /* the bit buffer */
    char    *mode;         /* R/W mode */
    FILE    *fp;           /* the file pointer */
    int     _file_id;      /* b-file pointer ID used in tealloc() */
} BFILE;

#ifdef BITSTREAM_MAIN
#define Bextern
#else
#define Bextern extern
#endif

Bextern int	_bfile_flag[N_BFILE];
Bextern BFILE	_bfp_mem[N_BFILE];

extern BFILE *bopen(char *name, char *mode );
extern void   bclose(BFILE *bfp);
extern int    bseek(BFILE *stream, long offset, int origin);
extern int    bread(char *data, int size, int nbits, BFILE *stream);
extern void   bwrite(char *data, int size, int nbits, BFILE *stream);
extern BFILE *tealloc();
extern void   tefree( BFILE *bfp );

extern int get_bstm(int	*data,          /* Input: input data */
		    int	nbits,         /* Input: number of bits */
		    BFILE	*bfp);          /* Input: bit file pointer */
#endif
