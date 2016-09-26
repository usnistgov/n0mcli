/*
 * xmlgen.h
 *
 *  Created on: Mar 29, 2014
 *      Author: andy
 */

#ifndef _XMLGEN_H_
#define _XMLGEN_H_

extern FILE *xmlopen(char *pdir, char *xmlfname) ;
extern void xmlclose(FILE *fp) ;
extern void xmlgen(FILE *fp, int nintervals) ;

#endif
