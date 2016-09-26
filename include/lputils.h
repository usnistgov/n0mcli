/*
 * lputils.h
 *
 *  Created on: Mar 26, 2014
 *      Author: andy
 */

#ifndef _LPUTILS_H_
#define _LPUTILS_H_

// per LP Set data structure...
typedef struct {
	int blkcount ;		// total number of LP blocks in LP
	int intvcount ;		// total number of meter intervals in LP
	int curblk ;		// current LP block to read
	int blksize ;		// LP block size, in bytes
	int recsize ;		// interval record size
	int recoffset ;		// first interval record offset in block
	int lpset_offset ;	// offset of particular LP data Set in device LP memory
	int size ;			// total size of the current LP Set, in bytes
	int nread ;			// number of data bytes already read from current LP block (0..blksize)
} readlp_info_t ;

#define NCHANNELS 4		/* maximum supported number of channels per LP set */
typedef struct {
	int isbigendian ;
	int charfmt ;
	int timefmt ;
	int intfmt ;
	int nifmt1 ;
	int nifmt2 ;

	int bendrdflg ;		/* Block End Read flag */
	int bendplflg ;		/* Block End Pulse flag */
	int sintvstatus ;	/* Simple interval status flag */
	int xintvstatus ;	/* Extended interval status flag */

	int nbinlp ;	/* Number of blocks in load profiler */
	int niperblk ;	/* Number of intervals per block */
	int nchnl ;		/* Number of channels */
	int intvlen ; 	/* Interval length in minutes */
	int datafmt ;	/* Data format of all interval data in Tab64 */

	LP_SEL_SET pidatasrc[NCHANNELS] ;		/* ptr to Interval data source for each channel [nchnl] */
	INT_FMT_CDE *pint_fmt_cde ;
	SCALAR_SET pscalar[NCHANNELS] ;	/* Scalars applied to interval data on a per-channel basis */
	DIVISOR_SET pdivisor[NCHANNELS] ;	/* Divisors applied to interval data on a per-channel basis */

	int blkorder ;		/* Block Order. 0 - ascending(N is older than N+1).), 1 - descending(N is newer than N+1).) */
	int ofloflg ;		/* FALSE -- Overflow has not occurred */
	int listtype ;		/* 0 - FIFO (First In First Out), 1 - Circular */
	int binhoflo ;		/* Block inhibit overflow flag */
	int intvorder ;		/* 0 - Intervals in each block  are in asc order (N is older than N+1) */
	int actmode ;
	int tstmode ;
	int nvalidblks ;	/* Number of valid Load Profile data blocks in meter LP data tables */
	int lastblk ;		/* Array element index of the newest valid block in the LP data array */
	int lastblkseq ;	/* Sequence number of the newest valid data block */
	int nunrdblks ;		/* Number of Load Profile blocks that have not been read */
	int nvalidintv ;	/* Number of valid intervals stored in the newest LP blk */

} meterinfo_t ;

extern int lp_blks_to_read ;

extern int gettab0(unsigned char *p, int size) ;
extern int gettab3(unsigned char *p, int size) ;

extern int gettab11(unsigned char *p, int size) ;
extern int gettab12(unsigned char *p, int size) ;
extern int gettab15(unsigned char *p, int size) ;
extern int gettab16(unsigned char *p, int size) ;

extern int gettab21(unsigned char *p, int size) ;
extern int gettab22(unsigned char *p, int size) ;
extern int gettab23(unsigned char *p, int size) ;
extern int gettab27(unsigned char *p, int size) ;
extern int gettab28(unsigned char *p, int size) ;

extern int gettab61(unsigned char *p, int size) ;
extern int gettab62(unsigned char *p, int size) ;
extern int gettab63(unsigned char *p, int size) ;
extern int gettab64(unsigned char *p, int size) ;
extern int getlpchunk(unsigned char *p, int size) ;

extern int get_lp_metadata(unsigned char *p, int size) ;

extern void lputils_init() ;
extern void lputils_deinit() ;

// set read LP information. Defined in n0mcli.c (main file)
extern void set_rlp_params(int rlpmode, readlp_info_t *pinfo) ;

extern unsigned int lp_getduration(int setidx ) ;
extern unsigned int lp_getstart(int setidx ) ;
extern void lp_getmeterinfo(int setidx, meterinfo_t *pmi, readlp_info_t *prlpi) ;
extern void lp_getivdata(int setidx, int icnt, float *pval, int nval) ;

extern char * lp_get_start_addr(int setidx ) ;
extern int lp_get_tot_size(int setidx ) ;

#endif
