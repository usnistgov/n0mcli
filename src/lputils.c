/*
 * lputils.c
 *
 *  Created on: Mar 26, 2014
 *      Author: andy
 */

#include <stdio.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "psem.h"
#include "diag.h"
#include "c1219.h"

#include "xmlgen.h"
#include "lputils.h"

// number of LP blocks to read. Read all valid blocks if <=0
int lp_blks_to_read = 10 ;
int lp_price = 1 ;

int lp_channels = 1 ;
double lp_t15RatioF1 =  1.0 ;
double lp_t15RatioP1 = 1.0 ;
double lp_t12Multiplier = 1.0 ;
double lp_LPMultiplier = 1.0 ;
double lp_LPDivisor = 1.0 ;
double lp_LPValueOffset = 0 ;

static unsigned char *ptab0 = NULL ;
static int t0size = 0 ;
static unsigned char *ptab3 = NULL ;
static int t3size = 0 ;

static unsigned char *ptab11 = NULL ;
static int t11size = 0 ;
static unsigned char *ptab12 = NULL ;
static int t12size = 0 ;
static unsigned char *ptab15 = NULL ;
static int t15size = 0 ;
static unsigned char *ptab16 = NULL ;
static int t16size = 0 ;

static unsigned char *ptab21 = NULL ;
static int t21size = 0 ;
static unsigned char *ptab22 = NULL ;
static int t22size = 0 ;
static unsigned char *ptab23 = NULL ;
static int t23size = 0 ;
static unsigned char *ptab27 = NULL ;
static int t27size = 0 ;
static unsigned char *ptab28 = NULL ;
static int t28size = 0 ;

static unsigned char *ptab61 = NULL ;
static int t61size = 0 ;
static unsigned char *ptab62 = NULL ;
static int t62size = 0 ;
static unsigned char *ptab63 = NULL ;
static int t63size = 0 ;
static unsigned char *ptab64 = NULL ;
static int t64size = 0 ;

static meterinfo_t mi ;

static int rlpmode ;
static int total_lp_blocks ;
// for each of 4 Sets, can be potentially different from set to set
static readlp_info_t rlpi[4] ;

// load profile data goes here
static unsigned char *plp[4] = { NULL,  NULL,  NULL,  NULL } ;

static struct {
	int nlpblocks ;	// number of meter LP blocks in our piece of Load Profile
	int nviv ;		// number of valid LP intervals in our piece of Load Profile
	int startoffset ;	// start offset in LP allocated memory
} our_LP_data ;

void lputils_init()
{
	int i ;

	if (ptab0) {
		ptab0 = NULL ;
		t0size = 0 ;
	}
	if (ptab61) {
		ptab61 = NULL ;
		t61size = 0 ;
	}
	if (ptab62) {
		ptab62 = NULL ;
		t62size = 0 ;
	}
	if (ptab63) {
		ptab63 = NULL ;
		t63size = 0 ;
	}
	if (ptab64) {
		ptab64 = NULL ;
		t64size = 0 ;
	}
	for (i=0; i < 4; i++) {
		if (plp[i]) {
			free(plp[i]) ;
			plp[i] = NULL ;
		}
	}
}

void lputils_deinit()
{
	lputils_init() ;
}

static int get_tab(int tabno, unsigned char *p, int size, unsigned char **pptab)
{
	int rc = 0 ;
	readrsphdr_t *ph ;
	int datasz ;
	unsigned char *pnewtab ;

	if (*pptab != NULL) {
		free(*pptab) ;
		*pptab = NULL ;
	}

	if (size >= sizeof(readrsphdr_t)) {
		ph = (readrsphdr_t *) p ;
		datasz = ph->rspsize_h ; datasz = (datasz << 8) + ph->rspsize_l ;
		p += sizeof(readrsphdr_t) ;

		VTRACE("------Tab%d, %d bytes, %d data bytes\n", tabno, size, datasz) ;

		if ((pnewtab = malloc(datasz)) != NULL) {
			memcpy(pnewtab, p, datasz) ;
			*pptab = pnewtab ;
			rc = datasz ;
		}
		else {
			TRACE("--- Tab%d -- can't allocate %d bytes!\n", tabno, datasz) ;
		}
	}

	return rc ;
}

int gettab0(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(0, p, size, &ptab0) ;
	t0size = rc ;

	return rc ;
}

int gettab3(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(3, p, size, &ptab3) ;
	t3size = rc ;

	return rc ;
}
////////////////////////////////////////////
int gettab11(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(11, p, size, &ptab11) ;
	t11size = rc ;

	return rc ;
}
int gettab12(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(12, p, size, &ptab12) ;
	t12size = rc ;

	return rc ;
}
int gettab15(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(15, p, size, &ptab15) ;
	t15size = rc ;

	return rc ;
}
int gettab16(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(16, p, size, &ptab16) ;
	t16size = rc ;

	return rc ;
}
////////////////////////////////////////////
int gettab21(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(21, p, size, &ptab21) ;
	t21size = rc ;

	return rc ;
}
int gettab22(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(22, p, size, &ptab22) ;
	t22size = rc ;

	return rc ;
}
int gettab23(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(23, p, size, &ptab23) ;
	t23size = rc ;

	return rc ;
}
int gettab27(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(27, p, size, &ptab27) ;
	t27size = rc ;

	return rc ;
}
int gettab28(unsigned char *p, int size)
{
	int rc = 0 ;

	rc = get_tab(28, p, size, &ptab28) ;
	t28size = rc ;

	return rc ;
}

////////////////////////////////////////////
int gettab61(unsigned char *p, int size)
{
	int rc = 0 ;
	readrsphdr_t *ph ;
	int datasz ;

	ph = (readrsphdr_t *) p ;
	datasz = ph->rspsize_h ; datasz = (datasz << 8) + ph->rspsize_l ;
	p += sizeof(readrsphdr_t) ;

	VTRACE("------Tab61, %d bytes, %d data bytes\n", size, datasz) ;

	if ((ptab61 = malloc(datasz)) != NULL) {
		memcpy(ptab61, p, datasz) ;
		rc = t61size = datasz ;

	}
	else {
		TRACE("--- Tab61 -- can't allocate %d bytes!\n", datasz) ;
	}

	return rc ;
}

int gettab62(unsigned char *p, int size)
{
	int rc = 0 ;
	readrsphdr_t *ph ;
	int datasz ;

	ph = (readrsphdr_t *) p ;
	datasz = ph->rspsize_h ; datasz = (datasz << 8) + ph->rspsize_l ;
	p += sizeof(readrsphdr_t) ;

	VTRACE("------Tab62, %d bytes, %d data bytes\n", size, datasz) ;

	if ((ptab62 = malloc(datasz)) != NULL) {
		memcpy(ptab62, p, datasz) ;
		rc = t62size = datasz ;

	}
	else {
		TRACE("--- Tab62 -- can't allocate %d bytes!\n", datasz) ;
	}

	return rc ;
}

int gettab63(unsigned char *p, int size)
{
	int rc = 0 ;
	readrsphdr_t *ph ;
	int datasz ;

	ph = (readrsphdr_t *) p ;
	datasz = ph->rspsize_h ; datasz = (datasz << 8) + ph->rspsize_l ;
	p += sizeof(readrsphdr_t) ;

	VTRACE("------Tab63, %d bytes, %d data bytes\n", size, datasz) ;

	if ((ptab63 = malloc(datasz)) != NULL) {
		memcpy(ptab63, p, datasz) ;
		rc = t63size = datasz ;

	}
	else {
		TRACE("--- Tab63 -- can't allocate %d bytes!\n", datasz) ;
	}

	get_lp_metadata(NULL, 0) ;

	return rc ;
}

int getlpchunk(unsigned char *p, int size)
{
	int rc = 0 ;
	return rc ;
}

int gettab64(unsigned char *p, int size)
{
	int rc = 0 ;
	readrsphdr_t *ph ;
	int datasz ;
	int idx ;
	readlp_info_t *plpi ;
	int off ;

	if (ptab64)
		free(ptab64) ;

	ph = (readrsphdr_t *) p ;
	datasz = ph->rspsize_h ; datasz = (datasz << 8) + ph->rspsize_l ;
	p += sizeof(readrsphdr_t) ;

	idx = rlpmode-1 ;
	plpi = &rlpi[idx] ;
	off = plpi->curblk * plpi->blksize + plpi->nread ;
	plpi->nread += datasz ;

	VTRACE("\n------LP Set#%d, block #%d, %d data bytes @%d\n", rlpmode, plpi->curblk, datasz, off) ;

	if (plp[idx]) {
		memcpy(&plp[idx][off], p, datasz) ;
		rc =  datasz ;
////		if (verbose > 0)
////			DumpHex(p, datasz) ;
	}
	else {
		TRACE("--- Tab64 -- no space for %d bytes!\n", datasz) ;
	}

	if (plpi->nread >= plpi->blksize) {
		plpi->curblk += 1 ;
		plpi->lpset_offset = (plpi->blksize * plpi->curblk) ;
		plpi->nread = 0 ;
	}
	else {
		plpi->lpset_offset += datasz ;
	}

	if (plpi->curblk >= plpi->blkcount) {
		++idx ;
		if ((idx >= 4) || (rlpi[idx].blkcount <= 0)) {
			rlpmode = 0 ;
			set_rlp_params(rlpmode, NULL) ;
		}
		else {
			rlpmode += 1 ;
			set_rlp_params(rlpmode, &rlpi[idx]) ;
		}
	}
	else {
		set_rlp_params(rlpmode, plpi) ;
	}

	return rc ;
}


static unsigned char * sdatafmt(int n)
{
	static const struct {
		int code ;
		unsigned char *fmt ;
	} fmtlkup[] = {
			{ IFMT_UINT8, (unsigned char *) "UINT8" },
			{ IFMT_UINT16, (unsigned char *) "UINT16" },
			{ IFMT_UINT32, (unsigned char *) "UINT32" },
			{ IFMT_INT8, (unsigned char *) "INT8" },
			{ IFMT_INT16, (unsigned char *) "INT16" },
			{ IFMT_INT32, (unsigned char *) "INT32" },
			{ NIFMT_FMAT1, (unsigned char *) "NI_1" },
			{ NIFMT_FMAT2, (unsigned char *) "NI_2" },
			{ 0, (unsigned char *) "Unknown" }
		} ;
		int i ;

		for (i=0; fmtlkup[i].code != 0; i++) {
			if (n == fmtlkup[i].code)
				break ;
		}

		return fmtlkup[i].fmt ;
}

static const char *sdataorder[2] = { "Little Endian", "Big Endian" } ;
static const char *scharfmt[4] = { "ISO-7", "ISO-8", "UTF-8", "Reserved" } ;
static const char *stimefmt[5] = { "NoClock", "BCD", "UINT8(7 bytes total)", "UINT32", "UINT32S" } ;
static const char *sintfmt[3] = { "Two-s complement", "One-s complement", "Sign/Magnitude" } ;
static const char *snifmt[14] = { 	"FLOAT64", "FLOAT32", "FLOAT_CHAR12", "FLOAT_CHAR6",
									"INT32_1", "FIXED_BCD6", "FIXED_BCD4", "INT24",
									"INT32", "INT40", "INT48", "INT64",
									"FIXED_BCD8", "FLOAT_CHAR21"
 	 	 	 	 	 	 	 	 } ;
static const char *sblkorder[2] = { "Ascending (N is older than N+1)", "Descending (N is newer than N+1)." } ;
static const char *slisttype[2] = {"FIFO", "Circular list"} ;
static const char *sintvorder[2] = { "Ascending (N is older than N+1)", "Descending (N is newer than N+1)." } ;
static const char *sbool[2] = { "False", "True" } ;

// non-integer data size
static int nidata_sz(int v)
{
	int rc = 0 ;

	switch (v) {
		case 0:  // FLOAT64
			rc = 8 ;
			break ;
		case 1:  // FLOAT32
			rc = 4 ;
			break ;
		case 2:  // FLOAT_CHAR12 (A STRING Number)
			rc = 12 ;
			break ;
		case 3:  // FLOAT_CHAR6 (A STRING Number)
			rc = 6 ;
			break ;
		case 4:  // INT32 (Implied decimal point between fourth 	and fifth digits from least significant digit)
			rc = 4 ;
			break ;
		case 5:  // FIXED_BCD6
			rc = 3 ;
			break ;
		case 6:  // FIXED_BCD4
			rc = 2 ;
			break ;
		case 7:  // INT24
			rc = 3 ;
			break ;
		case 8:  // INT32
			rc = 4 ;
			break ;
		case 9:  // INT40
			rc = 5 ;
			break ;
		case 10:  // INT48
			rc = 6 ;
			break ;
		default:
			break ;
	}

	return rc ;
}

// interval data size
static int intvdata_sz(int v)
{
	int rc = 0 ;

	switch (v) {
		case 1: // UINT8
			rc = 1 ;
			break ;
		case 2: // UINT16
			rc = 2 ;
			break ;
		case 4: // UINT32
			rc = 4 ;
			break ;
		case 8: // INT8
			rc = 1 ;
			break ;
		case 16: // INT16
			rc = 2 ;
			break ;
		case 32: // INT32
			rc = 4 ;
			break ;
		case 64: // NI_FMAT1
			rc = nidata_sz(mi.nifmt1) ;
			break ;
		case 128: // NI_FMAT2
			rc = nidata_sz(mi.nifmt2) ;
			break ;
		default:
			break ;
	}

	return rc ;
}

// interval time value data size. Mimics the STIME_DATE format
static int tmdata_sz(int v)
{
	int rc = 0 ;

	switch (v) {
		case 0:  // No clock in the End Device.
			rc = 0 ;
			break ;
		case 1: // BCD type with discrete fields for yr, mo, dy, hr, min
			rc  = 5 ;
			break ;
		case 2: // UINT8 type with discrete fields for yr, mo, dy, hr, min
			rc = 5 ;
			break ;
		case 3: // UINT32 counters where HTIME_DATE, LTIME_DATE and STIME_DATE types are encoded relative
				// to 01/01/1970 @ 00:00:00 UTC
			rc = 4 ;
			break ;
		case 4: // UINT32 counters where HTIME_DATE, LTIME_DATE and STIME_DATE types are encoded relative
				// to 01/01/1970 @ 00:00:00	UTC), including seconds
			rc = 4 ;
			break ;
		default:
			break ;
	}

	return rc ;
}

int get_lp_metadata(unsigned char *p, int size)
{
	int rc = 0 ;
	GEN_CONFIG_RCD *pt0 ;
	LP_SET_RCD *pt61 ;
	////DATA_SELECTION_RCD *pt62 ;
	LP_SET_STATUS_RCD *pt63 ;

	int i, j ;
	unsigned char *pstu ; 	// pointer into the 'standard tables used' set
	unsigned char stumask ;
	LP_DATA_SET_TBL *pdstbl ;
	unsigned int lpsoffset ;	// LP SetX element offset in Table62
	////unsigned int size_of_set ;		// size of a single element of Tab62 corresponding to one LP Set, in bytes

	int extints_sz ;	// size of extended interval status record, in bytes
	int splints_sz ;	// size of simple interval status record, in bytes
	int intentry_sz ;	// size of interval entry: 1+<intv_data_sz>
	int blkendtm_sz ;	// block end time size

	pt0 = (GEN_CONFIG_RCD *) ptab0 ;
	pt61 = (LP_SET_RCD *) ptab61 ;
	////pt62 = (DATA_SELECTION_RCD *) ptab62 ;
	pt63 = (LP_SET_STATUS_RCD *) ptab63 ;

	mi.isbigendian = (pt0->format_control_1.data_order == DATA_ORDER_LSBFIRST) ? 0 : 1  ;
	mi.charfmt = pt0->format_control_1.char_format ;
	mi.timefmt = pt0->format_control_2.tm_format ;
	mi.intfmt = pt0->format_control_2.int_format ;
	mi.nifmt1 = pt0->format_control_3.ni_format1 ;
	mi.nifmt2 = pt0->format_control_3.ni_format2 ;

	mi.bendrdflg = pt61->lp_flags.blk_end_read_flag ;		/* Block End Read flag */
	mi.bendplflg = pt61->lp_flags.blk_end_pulse_flag ;		/* Block End Pulse flag */
	mi.sintvstatus = pt61->lp_flags.simple_int_status_flag ;	/* Simple interval status flag */
	mi.xintvstatus = pt61->lp_flags.extended_int_status_flag ;	/* Extended interval status flag */

	// Tab0 -- General Config
	VTRACE("\nGeneral meter config (from Tab0)\n\tData Order=%s (%d)\n", sdataorder[mi.isbigendian], mi.isbigendian) ;
	VTRACE("\tChar format=%s (%d)\n", scharfmt[mi.charfmt], mi.charfmt) ;
	VTRACE("\tTime format=%s (%d)\n", stimefmt[mi.timefmt], mi.timefmt) ;
	VTRACE("\tInteger format=%s (%d)\n", sintfmt[mi.intfmt], mi.intfmt) ;
	VTRACE("\tNon integer format 1=%s (%d)\n", snifmt[mi.nifmt1], mi.nifmt1) ;
	VTRACE("\tNon integer format 2=%s (%d)\n", snifmt[mi.nifmt2], mi.nifmt2) ;

	VTRACE("\tBlock End Read flag=%s (%d)\n", sbool[mi.bendrdflg], mi.bendrdflg) ;
	VTRACE("\tBlock End Pulse flag=%s (%d)\n", sbool[mi.bendplflg], mi.bendplflg) ;
	VTRACE("\tSimple interval status flag=%s (%d)\n", sbool[mi.sintvstatus], mi.sintvstatus) ;
	VTRACE("\tExtended interval status flag=%s (%d)\n", sbool[mi.xintvstatus], mi.xintvstatus) ;

	if (pt0->default_set_used == 0) {
		VTRACE("\tDefault set used=None\n") ;
	}
	else {
		VTRACE("\tDefault set used=%d\n", pt0->default_set_used) ;
	}

	pstu = &(pt0->nbr_pending)+1 ; // point to the beginning of the set
	pstu += (64/8) ;	// point to the byte describing tables starting from 64 (LP data for Set 1)

	total_lp_blocks = 0 ;
	memset(&rlpi, 0, sizeof(rlpi)) ;

	// scan all 4 sets
	for (stumask=1, i=0; i < 4; stumask <<= 1, i+=1) {
		int chni ;

		if ((*pstu) & stumask) {	// SetX is used
			pdstbl = &(pt61->lp_dstbl[i]) ;
			mi.nbinlp = pdstbl->nbr_blks_set ;
			mi.niperblk = pdstbl->nbr_blk_ints_set ;
			mi.nchnl = pdstbl->nbr_chns_set ;
			mi.intvlen = pdstbl->max_int_time_set ;

			lpsoffset = i * (mi.nchnl * (sizeof(LP_SOURCE_SEL_RCD)+2+2) + 1) ;

			memset(&mi.pidatasrc, 0, sizeof(mi.pidatasrc)) ;
			memset(&mi.pscalar, 0, sizeof(mi.pscalar)) ;
			memset(&mi.pdivisor, 0, sizeof(mi.pdivisor)) ;

			for (chni=0; chni<mi.nchnl; chni++) {
				mi.pidatasrc[chni] = (LP_SOURCE_SEL_RCD *) (ptab62 + lpsoffset + sizeof(LP_SOURCE_SEL_RCD) * chni) ;
			}
			mi.pint_fmt_cde = ((unsigned char *)mi.pidatasrc[0] + sizeof(LP_SOURCE_SEL_RCD)*mi.nchnl) ;
			for (chni=0; chni<mi.nchnl; chni++) {
				mi.pscalar[chni] = (SCALAR_SET) ((unsigned char *)mi.pint_fmt_cde + sizeof(INT_FMT_CDE)+ sizeof(*mi.pscalar[0])*chni) ;
			}
			for (chni=0; chni<mi.nchnl; chni++) {
				mi.pdivisor[chni] = (DIVISOR_SET) ((unsigned char *)mi.pscalar[0] + sizeof(*mi.pscalar[0])*mi.nchnl + sizeof(*mi.pdivisor[0])*chni )  ;
			}

			mi.datafmt = *mi.pint_fmt_cde ;

			VTRACE("SET #%d\n", i+1) ;
			VTRACE("\tData format of all interval data in Tab64 = %s (%d)\n", sdatafmt(mi.datafmt), mi.datafmt) ;
			VTRACE("\tNumber of blocks in load profile=%d\n", mi.nbinlp) ;
			VTRACE("\tNumber of intervals per block=%d\n", mi.niperblk) ;
			VTRACE("\tNumber of channels=%d\n", mi.nchnl) ;
			VTRACE("\tInterval length in minutes=%d\n", mi.intvlen) ;

			VTRACE("\tScalar/Divisor, %d channels: ", mi.nchnl) ;
			for (j=0; j < mi.nchnl; j++) {
				VTRACE("%d/%d ", (int)*mi.pscalar[j], (int)*mi.pdivisor[j]) ;
			}
			VTRACE("\n") ;

			mi.blkorder  = pt63->lp_set_status_flags.order ;
			mi.ofloflg  = pt63->lp_set_status_flags.overflow_flag ;
			mi.listtype  = pt63->lp_set_status_flags.list_type ;
			mi.binhoflo  = pt63->lp_set_status_flags.block_inhibit_overflow_flag ;
			mi.intvorder  = pt63->lp_set_status_flags.interval_order ;
			mi.actmode  = pt63->lp_set_status_flags.active_mode_flag ;
			mi.tstmode  = pt63->lp_set_status_flags.test_mode ;
			mi.nvalidblks  = pt63->nbr_valid_blocks ;
			mi.lastblk  = pt63->last_block_element ;
			mi.lastblkseq  = pt63->last_block_seq_nbr ;
			mi.nunrdblks  = pt63->nbr_unread_blocks ;
			mi.nvalidintv  = pt63->nbr_valid_int ;

			VTRACE("\n\tTable 63 - Load Profile Status\n") ;
			VTRACE("\tBlock Order = %s (%d)\n", sblkorder[mi.blkorder], mi.blkorder) ;
			VTRACE("\tOverflow flag = %s (%d)\n", sbool[mi.ofloflg], mi.ofloflg) ;
			VTRACE("\tList type = %s (%d)\n", slisttype[mi.listtype], mi.listtype) ;
			VTRACE("\tBlock inhibit oflo flag = %s (%d)\n", sbool[mi.binhoflo], mi.binhoflo) ;
			VTRACE("\tInterval order = %s (%d)\n", sintvorder[mi.intvorder], mi.intvorder) ;
			VTRACE("\tActive mode = %s (%d)\n", sbool[mi.actmode], mi.actmode) ;
			VTRACE("\tTest mode = %s (%d)\n", sbool[mi.tstmode], mi.tstmode) ;
			VTRACE("\tNumber of valid blks = %d\n", mi.nvalidblks) ;
			VTRACE("\tElement index of the newest valid data blk = %d\n", mi.lastblk) ;
			VTRACE("\tSequence number of the newest valid data blk = %d\n", mi.lastblkseq) ;
			VTRACE("\tNumber of unread Load Profile blks = %d\n", mi.nunrdblks) ;
			VTRACE("\tNumber of valid intervals stored in the newest LP blk = %d\n", mi.nvalidintv) ;
			VTRACE("--------\n") ;

			blkendtm_sz = tmdata_sz(mi.timefmt) ;	// block end time size
			intentry_sz = (1 + intvdata_sz(mi.datafmt))*mi.nchnl ;	// size of interval entry: 1+<intv_data_sz>
			splints_sz = (mi.sintvstatus) ? 1*mi.nchnl : 0 ;	// size of simple interval status record, in bytes
			extints_sz = (mi.xintvstatus) ? (1*mi.nchnl/2)+1 : 0 ;	// size of extended interval status record, in bytes

			total_lp_blocks += mi.nvalidblks ;
			if (lp_blks_to_read >= 0) {
				if (lp_blks_to_read == 0) {
					rlpi[i].blkcount = mi.nvalidblks ;
					our_LP_data.nlpblocks = rlpi[i].blkcount ;
					our_LP_data.nviv = mi.nvalidintv + ((our_LP_data.nlpblocks-1)* mi.niperblk) ;
				}
				else {
					if (lp_blks_to_read < mi.nvalidblks) {
						rlpi[i].blkcount = lp_blks_to_read ;
						our_LP_data.nlpblocks = rlpi[i].blkcount ;
						our_LP_data.nviv = (our_LP_data.nlpblocks)* mi.niperblk ;
					}
					else {
						rlpi[i].blkcount = mi.nvalidblks ;
						our_LP_data.nlpblocks = rlpi[i].blkcount ;
						our_LP_data.nviv = mi.nvalidintv + ((our_LP_data.nlpblocks-1)* mi.niperblk) ;
					}
				}
				rlpi[i].curblk = 0 ;
				our_LP_data.startoffset = 0 ;
			}
			else {
				lp_blks_to_read = -lp_blks_to_read ;
				if (total_lp_blocks > lp_blks_to_read) {
					rlpi[i].curblk = total_lp_blocks - lp_blks_to_read ;
					rlpi[i].blkcount = mi.nvalidblks ;
					our_LP_data.nlpblocks = lp_blks_to_read ;
					our_LP_data.startoffset = rlpi[i].curblk ;
				}
				else {
					rlpi[i].curblk = 0 ;
					rlpi[i].blkcount = mi.nvalidblks ;
					our_LP_data.nlpblocks = rlpi[i].blkcount ;
					our_LP_data.startoffset = 0 ;
				}
				our_LP_data.nviv = mi.nvalidintv + ((our_LP_data.nlpblocks-1)* mi.niperblk) ;
			}

			if (rlpi[i].blkcount == mi.nvalidblks) {
				rlpi[i].intvcount = (rlpi[i].blkcount-1)*mi.niperblk + mi.nvalidintv ;
			}
			else {
				rlpi[i].intvcount = rlpi[i].blkcount * mi.niperblk ;
			}

			rlpi[i].recoffset = blkendtm_sz + splints_sz /*+ extints_sz*/ ;
			if (mi.bendrdflg)
				rlpi[i].recoffset += nidata_sz(mi.nifmt1)*extints_sz ;
			if (mi.bendplflg)
				rlpi[i].recoffset += 4 ; // sizeof(UINT32)
			rlpi[i].blksize = rlpi[i].recoffset + intentry_sz*mi.niperblk ;

			our_LP_data.startoffset *= rlpi[i].blksize ;

			rlpi[i].lpset_offset = (rlpi[i].blksize * rlpi[i].curblk) ; // set starting offset for LP read

			rlpi[i].recsize = intentry_sz ;

			rlpi[i].size = rlpi[i].blksize * rlpi[i].blkcount ;
			if (i < 3)
				rlpi[i+1].lpset_offset = rlpi[i].lpset_offset + rlpi[i].size ;

			plp[i] = malloc(rlpi[i].size) ;

			VTRACE("\ttotal number of LP blocks in LP = %d\n", rlpi[i].blkcount) ;
			VTRACE("\ttotal number of LP intervals in LP = %d\n", rlpi[i].intvcount) ;
			VTRACE("\tcurrent LP block to read = %d\n", rlpi[i].curblk) ;
			VTRACE("\tLP block size, in bytes = %d\n", rlpi[i].blksize) ;
			VTRACE("\tinterval record size = %d\n", rlpi[i].recsize) ;
			VTRACE("\tfirst interval record offset in block = %d\n", rlpi[i].recoffset) ;
			VTRACE("\toffset of particular LP data Set in device LP memory = %d\n", rlpi[i].lpset_offset) ;
			VTRACE("\ttotal size of the current LP Set, in bytes = %d\n", rlpi[i].size) ;
			VTRACE("\n") ;
		}
		++pt63 ; 	// point to the next set, if any in LP status record/Table 63
	}

	if (plp[0]) {
		rlpmode = 1 ;
		set_rlp_params(rlpmode, &rlpi[0]) ;
	}

	return rc ;
}

static time_t getblktime(unsigned char *pbt, int timefmt)
{
	time_t rc = 0 ;
	struct tm tm ;

	memset(&tm, 0, sizeof(tm)) ;

	switch (timefmt) {
		case 1: // BCD type with discrete fields for yr, mo, dy, hr, min
			tm.tm_year = ((pbt[0]&0xf0)>>4)*10 + (pbt[0] & 0x0f)  + 2000 - 1900 ;
			tm.tm_mon = ((pbt[1]&0xf0)>>4)*10 + (pbt[1] & 0x0f) - 1 ;
			tm.tm_mday = ((pbt[2]&0xf0)>>4)*10 + (pbt[2] & 0x0f) ;
			tm.tm_hour = ((pbt[3]&0xf0)>>4)*10 + (pbt[3] & 0x0f) ;
			tm.tm_min = ((pbt[4]&0xf0)>>4)*10 + (pbt[4] & 0x0f) ;

			rc = mktime(&tm) ;

			break ;
		case 2: // UINT8 type with discrete fields for yr, mo, dy, hr, min
			tm.tm_year = pbt[0] + 2000 - 1900 ;
			tm.tm_mon = pbt[1] - 1 ;
			tm.tm_mday = pbt[2] ;
			tm.tm_hour = pbt[3] ;
			tm.tm_min = pbt[4] ;

			rc = mktime(&tm) ;

			break ;
		case 3: // UINT32 counters where HTIME_DATE, LTIME_DATE and STIME_DATE types are encoded relative
				// to 01/01/1970 @ 00:00:00 UTC
		case 4: // UINT32 counters where HTIME_DATE, LTIME_DATE and STIME_DATE types are encoded relative
				// to 01/01/1970 @ 00:00:00	UTC), including seconds
			if (mi.isbigendian) {
				rc = pbt[0] ;
				rc = (rc<<8) + pbt[1] ;
				rc = (rc<<8) + pbt[2] ;
				rc = (rc<<8) + pbt[3] ;
			}
			else {
				rc = pbt[3] ;
				rc = (rc<<8) + pbt[2] ;
				rc = (rc<<8) + pbt[1] ;
				rc = (rc<<8) + pbt[0] ;
			}
			break ;
		default:
			break ;
	}

	return rc ;
}

char * lp_get_start_addr(int setidx )
{
	char *p = NULL ;

	if ((setidx>=0) && (setidx < 4) && plp[setidx]) {
		p = (char *) &plp[setidx][0] + our_LP_data.startoffset ;
	}

	return p ;
}

int lp_get_tot_size(int setidx )
{
	int n = 0 ;

	if ((setidx>=0) && (setidx < 4) && plp[setidx]) {
		n = our_LP_data.nlpblocks * rlpi[setidx].blksize ;
	}

	return n ;
}


unsigned int lp_getstart(int setidx )
{
	time_t t0 = 0 ;
	////unsigned int dur ;
	unsigned char *pb0t ;	// pointer to Block End time entry of 0th block
	////int n ;

	if ((setidx>=0) && (setidx < 4) && plp[setidx]) {
		if (mi.blkorder == 0) {		// i.e. N is older than N+1, i.e. Block0 is the oldest block
			pb0t = &plp[setidx][0] + our_LP_data.startoffset ;
		}
		else {
			pb0t = &plp[setidx][0] + our_LP_data.startoffset + our_LP_data.nlpblocks * rlpi[setidx].blksize ; // last block is the oldest ..
		}
		// this gives the End time of the last record of our starting LP profile block
		t0 = getblktime(pb0t, mi.timefmt) ;
		// make it a Start time by subtracting the duration in seconds ...
		if (our_LP_data.nlpblocks == 1) {
			t0 = t0 - ( mi.intvlen * 60)*our_LP_data.nviv ;
		}
		else {
			t0 = t0 - ( mi.intvlen * 60)*mi.niperblk ;
		}
	}

	return ((unsigned int) t0) ;
}

unsigned int lp_getduration(int setidx )
{
	unsigned int dur = 0 ;

	if ((setidx>=0) && (setidx < 4) && plp[setidx]) {
		dur = mi.intvlen * 60 ;	// meter LP interval length in seconds
		dur = dur*our_LP_data.nviv ;
	}

	return dur ;
}

void lp_getmeterinfo(int setidx, meterinfo_t *pmi, readlp_info_t *prlpi)
{
	memcpy(pmi, &mi, sizeof(mi)) ;
	if ((setidx>=0) && (setidx < 4) && plp[setidx]) {
		memcpy(prlpi, &rlpi[setidx], sizeof(readlp_info_t)) ;
	}
	else {
		memset(prlpi, 0, sizeof(readlp_info_t)) ;
	}
}

void * lp_getinterval(int setno, long long *tm_start)
{
	void *p = NULL ;

	return p ;
}

void lp_getivdata(int setidx, int icnt, float *pval, int nval)
{
	unsigned int offset ;
	int blkno ;
	int i ;
	unsigned char *p ;
	float v ;
	unsigned long iv ;
	int flagsz = 2 ;	// every LP record starts with a two-byte flag field
	int valshift ;

	memset(pval, 0, nval*sizeof(float)) ;

	if ((icnt < 0) || (icnt >= our_LP_data.nviv) )
		return ;

	// note: only ascending LP data order supported! i.e. element N is older than element (N+1)
	//
	if ((setidx>=0) && (setidx < 4) && plp[setidx] ) {
		blkno = icnt / mi.niperblk ;
		i = icnt % mi.niperblk ;

		offset = blkno * rlpi[setidx].blksize + our_LP_data.startoffset ;	// offset of a block of interest
		offset += (i*rlpi[setidx].recsize + rlpi[setidx].recoffset) ;	// offset of a record of interest (from start)

		p = plp[setidx] + offset ;	// points to the requested meter interval record

		for (i=0; i < nval; i++) {
			iv = 0 ;

			switch (mi.datafmt) {
				case 1: // UINT8
				case 8: // INT8
					valshift = sizeof(uint8_t) ;
					iv = p[flagsz + i*valshift] ;
					if ((mi.datafmt > 4) && (iv & 0x80)) {
						iv |= 0xffffff00 ;
					}
					v = (float)((int) iv) ;
					break ;
				case 2: // UINT16
				case 16: // INT16
					valshift = sizeof(uint16_t) ;
					if (mi.isbigendian) {
						iv = p[flagsz + i*valshift] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 1] ;
					}
					else {
						iv = p[flagsz + i*valshift + 1] ;
						iv = (iv << 8) + p[flagsz + i*valshift] ;
					}
					if ((mi.datafmt > 4) && (iv & 0x8000)) {
						iv |= 0xffff0000 ;
					}
					v = (float)((int) iv) ;
					break ;
				case 4: // UINT32
				case 32: // INT32
					valshift = sizeof(uint32_t) ;
					if (mi.isbigendian) {
						iv = p[flagsz + i*valshift] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 1] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 2] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 3] ;
					}
					else {
						iv = p[flagsz + i*valshift + 3] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 2] ;
						iv = (iv << 8) + p[flagsz + i*valshift + 1] ;
						iv = (iv << 8) + p[flagsz + i*valshift] ;
					}
					v = (float)((int) iv) ;
					break ;
				case 64: // NI_FMAT1
				case 128: // NI_FMAT2
					iv = 0 ;	// format not supported
					v = (float)((int) iv) ;
					break ;
				default:
					iv = 0 ;	// format not supported
					v = (float)((int) iv) ;
					break ;
			}

			pval[i] = v /* * (*mi.pdivisor[i]) / (*mi.pscalar[i]) */ ;

			p += rlpi[setidx].recsize ;
		}

	}
}

void lp_makexml(char *pth, char *xmlfname)
{
	FILE *fp ;
	if ((fp=xmlopen(pth, xmlfname)) != NULL) {
		xmlgen(fp, our_LP_data.nviv ) ;
		xmlclose(fp) ;
	}
}
