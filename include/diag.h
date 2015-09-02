/*
 * diag.h
 *
 *  Created on: Mar 22, 2014
 *      Author: andy
 */

#ifndef _DIAG_H_
#define _DIAG_H_

#define TRACE(...) printf (__VA_ARGS__)
#define VTRACE(...) if (verbose > 0) printf (__VA_ARGS__)
#define VVTRACE(...) if (verbose > 1) printf (__VA_ARGS__)

#define SHOWPACKET(a,b,c) ShowPkt((unsigned char *)a,(unsigned char *)b,(int)c, ticCount)
#define VSHOWPACKET(a,b,c) if (verbose > 0) ShowPkt((unsigned char *)a,(unsigned char *)b,(int)c, ticCount)
#define VVSHOWPACKET(a,b,c) if (verbose > 1) ShowPkt((unsigned char *)a,(unsigned char *)b,(int)c, ticCount)

extern int verbose ;
extern void ShowPkt(unsigned char *s, unsigned char *pPkt, int count, unsigned int ticCount) ;
extern void DumpHex(unsigned char *p, int length) ;

#endif
