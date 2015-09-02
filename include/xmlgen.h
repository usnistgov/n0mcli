/*
 * xmlgen.h
 *
 *  Created on: Mar 29, 2014
 *      Author: andy
 */

#ifndef _XMLGEN_H_
#define _XMLGEN_H_

extern int getuuid(unsigned char *puuid) ;

extern void xmlindent(FILE *fp, int indent) ;
extern void xmlidtag(FILE *fp) ;
extern void xmlintro(FILE *fp) ;
extern void xmlinterval(FILE *fp, unsigned int start, unsigned int duration) ;
extern void xmlireading(FILE *fp, unsigned int start, unsigned int duration, float *pval, int nval) ;
extern void xmlfooter(FILE *fp) ;

extern FILE *xmlopen(char *pdir) ;
extern void xmlclose(FILE *fp) ;
extern void xmlgen(FILE *fp) ;

#endif
