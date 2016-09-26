/*
 * c1819.c
 *
 *  Created on: Mar 22, 2014
 *      Author: andy
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "c1219.h"
#include "diag.h"

static lookup_t reqtab[] = {
 { REQ_IDENT 		, (unsigned char *) "Identification request" },
 { REQ_READ 		, (unsigned char *) "Full Read request <tableid>" },
 { REQ_PREADI1 	, (unsigned char *) "Partial Read 1 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI2 	, (unsigned char *) "Partial Read 2 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI3 	, (unsigned char *) "Partial Read 3 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI4 	, (unsigned char *) "Partial Read 4 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI5 	, (unsigned char *) "Partial Read 5 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI6 	, (unsigned char *) "Partial Read 6 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI7 	, (unsigned char *) "Partial Read 7 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI8 	, (unsigned char *) "Partial Read 8 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI9 	, (unsigned char *) "Partial Read 9 idx request <tableid> <index>+ <count>" },
 { REQ_PREAD_DFLT 	, (unsigned char *) "Transfer Default Table" },
 { REQ_PREAD_OFF 	, (unsigned char *) "Partial Read/Offset request <tableid> <offset> <count>" },
 { REQ_WRITE 		, (unsigned char *) "Full Write request" },
 { REQ_PWRITEI1 	, (unsigned char *) "Partial Write 1 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI2 	, (unsigned char *) "Partial Write 2 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI3 	, (unsigned char *) "Partial Write 3 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI4 	, (unsigned char *) "Partial Write 4 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI5 	, (unsigned char *) "Partial Write 5 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI6 	, (unsigned char *) "Partial Write 6 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI7 	, (unsigned char *) "Partial Write 7 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI8 	, (unsigned char *) "Partial Write 8 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI9 	, (unsigned char *) "Partial Write 9 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITE_OFF 	, (unsigned char *) "Partial Write/Offset request <tableid> <offset> <count>" },
 { REQ_LOGON 		, (unsigned char *) "Logon request" },
 { REQ_SECURITY 	, (unsigned char *) "Security request" },
 { REQ_LOGOFF 		, (unsigned char *) "Logoff request" },
 { REQ_NEGOTIATE 	, (unsigned char *) "Negotiate/no baudrate specified" },
 { REQ_NEGBR1 		, (unsigned char *) "Negotiate/ 1 <baud rate> included in request" },
 { REQ_NEGBR2 		, (unsigned char *) "Negotiate/ 2 <baud rate> included in request" },
 { REQ_NEGBR3 		, (unsigned char *) "Negotiate/ 3 <baud rate> included in request" },
 { REQ_NEGBR4 		, (unsigned char *) "Negotiate/ 4 <baud rate> included in request" },
 { REQ_NEGBR5 		, (unsigned char *) "Negotiate/ 5 <baud rate> included in request" },
 { REQ_NEGBR6 		, (unsigned char *) "Negotiate/ 6 <baud rate> included in request" },
 { REQ_NEGBR7 		, (unsigned char *) "Negotiate/ 7 <baud rate> included in request" },
 { REQ_NEGBR8 		, (unsigned char *) "Negotiate/ 8 <baud rate> included in request" },
 { REQ_NEGBR9 		, (unsigned char *) "Negotiate/ 9 <baud rate> included in request" },
 { REQ_NEGBR10		, (unsigned char *) "Negotiate/ 10 <baud rate> included in request" },
 { REQ_NEGBR11		, (unsigned char *) "Negotiate/ 11 <baud rate> included in request" },
 { REQ_WAIT 		, (unsigned char *) "Wait request" },
 { REQ_TERMINATE 	, (unsigned char *) "Terminate request" },
 { -1  				, (unsigned char *) "unknown" }
};

static lookup_t rsptab[] = {
 { RSP_OK		, (unsigned char *) "ACK" },
 { RSPNOK_ERR	, (unsigned char *) "ERR" },
 { RSPNOK_SNS	, (unsigned char *) "SNS (Service Not Supported)" },
 { RSPNOK_ISC	, (unsigned char *) "ISC (Insufficient Security Clearance)" },
 { RSPNOK_ONP	, (unsigned char *) "ONP (Operation Not Possible)" },
 { RSPNOK_IAR	, (unsigned char *) "IAR (Inappropriate Action Requested)" },
 { RSPNOK_BSY	, (unsigned char *) "BSY (Device Busy)" },
 { RSPNOK_DNR	, (unsigned char *) "DNR (Data Not Ready)" },
 { RSPNOK_DLK	, (unsigned char *) "DLK (Data Locked)" },
 { RSPNOK_RNO	, (unsigned char *) "RNO (Renegotiate request)" },
 { RSPNOK_ISSS	, (unsigned char *) "ISSS (Invalid Service Sequence State)" },
 { -1  				, (unsigned char *) "?? (unknown)" }
};

unsigned char wrongcode[] = "Wrong code" ;

unsigned char *dsclookup(int n)
{
	lookup_t *p = NULL ;
	unsigned char *pdesc = NULL ;
	
	if ((n >= RSP_OK) && (n <= RSPNOK_ISSS)) {
		p = &rsptab[0] ;
	}
	else if ((n >= REQ_IDENT) && (n <= REQ_WAIT)) {
		p = &reqtab[0] ;
	}
	
	if (p != NULL) {
		while (p->code != -1) {
			if (p->code == n) {
				pdesc = p->descr ;
				break ;
			}
			++p ;
		}
	}
	
	return pdesc ;
}

int parse_request(unsigned char *preq, int len, int vlevel)
{
	int rc = 0 ;
	int n ;
	int nelem ;
	unsigned char *pdesc ;
	int i ;
	int tableid ;
	int offset ;
	int index[9] ;
	int count ;
	int userid ;
	unsigned char username[11] ;
	unsigned char passwd[21] ;
	int pktsize ;
	int maxpkt ;
	int baud ;

	if (preq) {
		n = *preq ;

		pdesc = &wrongcode[0] ;

		if ((n >= REQ_IDENT) && (n <= REQ_WAIT)) {
			if ((pdesc = dsclookup(n)) != NULL) {
				VTRACE("\n> %s\n", pdesc) ;
				switch(n) {
					case REQ_IDENT:
						if (vlevel>1) {

						}
						break ;
					case REQ_READ:
					case REQ_PREADI1:
					case REQ_PREADI2:
					case REQ_PREADI3:
					case REQ_PREADI4:
					case REQ_PREADI5:
					case REQ_PREADI6:
					case REQ_PREADI7:
					case REQ_PREADI8:
					case REQ_PREADI9:
						nelem = n - REQ_READ ;
						if (vlevel>1) {
							tableid = preq[1] ;
							tableid = (tableid<<8) + preq[2] ;
							for (i=0; i < nelem; i++) {
								index[i] = preq[3+i*2] ;
								index[i] = (index[i]<<8) + preq[3+ i*2 + 1] ;
								if (i == 0)
									VTRACE("\tIndex=") ;
								VTRACE("%d ", index[i]) ;
							}
							if (nelem > 0) {
								count = preq[3+nelem*2] ;
								count = (count<<8) + preq[3+nelem*2 + 1] ;
								VTRACE("Tableid=%d Count=%d\n", tableid, count) ;
							}
							else {
								VTRACE("Tableid=%d\n", tableid) ;
							}
						}
						break ;
					case REQ_PREAD_DFLT:
						nelem = 0 ;
						if (vlevel>1) {
						}
						break ;
					case REQ_PREAD_OFF:
						if (vlevel>1) {
							nelem = 0 ;
							tableid = preq[1] ;
							tableid = (tableid<<8) + preq[2] ;
							offset = preq[3] ;
							offset = (offset<<8) + preq[4] ;
							offset = (offset<<8) + preq[5] ;
							VTRACE("\tTableID=%d, Offset=%d\n", tableid, offset) ;
						}
						break ;
					case REQ_WRITE:
					case REQ_PWRITEI1:
					case REQ_PWRITEI2:
					case REQ_PWRITEI3:
					case REQ_PWRITEI4:
					case REQ_PWRITEI5:
					case REQ_PWRITEI6:
					case REQ_PWRITEI7:
					case REQ_PWRITEI8:
					case REQ_PWRITEI9:
						nelem = n - REQ_WRITE ;
						if (vlevel>1) {
							tableid = preq[1] ;
							tableid = (tableid<<8) + preq[2] ;
							for (i=0; i < nelem; i++) {
								index[i] = preq[3+i*2] ;
								index[i] = (index[i]<<8) + preq[3+ i*2 + 1] ;
								if (i == 0)
									VTRACE("\tIndex=") ;
								VTRACE("%d ", index[i]) ;
							}
							count = preq[3+nelem*2] ;
							count = (count<<8) + preq[3+nelem*2 + 1] ;
							VTRACE("\tTableID=%d, byte count=%d\n", tableid, count) ;
							////if (vlevel > 0)
							////DumpHex(&preq[3+nelem*2 + 2], count) ;
						}
						break ;
					case REQ_PWRITE_OFF:
						nelem = 0 ;
						if (vlevel>1) {
							tableid = preq[1] ;
							tableid = (tableid<<8) + preq[2] ;
							offset = preq[3] ;
							offset = (offset<<8) + preq[4] ;
							offset = (offset<<8) + preq[5] ;
							count = preq[6] ;
							count = (count<<8) + preq[7] ;
							VTRACE("\tTableID=%d, Offset=%d Count=%d\n", tableid, offset, count) ;
							if (vlevel > 0)
								DumpHex(&preq[8], count) ;
						}
						break ;
					case REQ_LOGON:
						if (vlevel>1) {
							userid = preq[1] ;
							userid = (userid<<8) + preq[2] ;
							memcpy(&username[0], &preq[3], 10) ;
							username[10] = '\0' ;
							VTRACE("\tUserID=%d, UserName=%s\n", userid, username) ;
						}
						break ;
					case REQ_SECURITY:
						if (vlevel>1) {
							int idx ;

							memcpy(&passwd[0], &preq[1], 20) ;
							passwd[20] = '\0' ;
							VTRACE("\tPasswd=") ;
							for (idx=0; idx<(sizeof(passwd)-1); idx++) {
								VTRACE("%02X", passwd[idx]) ;
							}
							VTRACE("\n") ;
						}
						break ;
					case REQ_LOGOFF:
						if (vlevel>1) {
						}
						break ;
					case REQ_NEGOTIATE:
					case REQ_NEGBR1:
					case REQ_NEGBR2:
					case REQ_NEGBR3:
					case REQ_NEGBR4:
					case REQ_NEGBR5:
					case REQ_NEGBR6:
					case REQ_NEGBR7:
					case REQ_NEGBR8:
					case REQ_NEGBR9:
					case REQ_NEGBR10:
					case REQ_NEGBR11:
						if (vlevel>1) {
							nelem = n - REQ_NEGOTIATE ;
							pktsize = preq[1] ;
							pktsize = (pktsize << 8) + preq[2] ;
							maxpkt = preq[2] ;
							baud = preq[3] ;
							VTRACE("\tpktsize=%d, maxpkt=%d, baud=%d\n", pktsize, maxpkt, baud) ;
						}
						break ;
					case REQ_WAIT:
						if (vlevel>1) {
							VTRACE("\t%d seconds\n", preq[1]) ;
						}
						break ;
					case REQ_TERMINATE:
						if (vlevel>1) {
						}
						break ;
				}
			}
		}
		else {
			VTRACE("> unsupported request %02X\n", n) ;
			pdesc = &wrongcode[0] ;
		}
	}

	return rc ;
}

int parse_response(unsigned char *prsp, int len, int mrc, int vlevel) // mrc -- matching request code(zero means N/A)
{
	int rc = 0 ;
	int n ;
	int nelem ;
	unsigned char *pdesc ;
	unsigned char *pdmrc ;
	////int i ;
	int count ;
	int pktsize ;
	int maxpkt ;
	int baud ;

	if (prsp) {
		n = *prsp ;

		pdesc = &wrongcode[0] ;

		if ((n >= RSP_OK) && (n <= RSPNOK_ISSS)) {
			if ((pdesc = dsclookup(n)) != NULL) {
				if ((mrc < REQ_IDENT) || (mrc > REQ_WAIT) || ((pdmrc = dsclookup(mrc)) == NULL))
					pdmrc = (unsigned char *) "Unmatched request" ;
				VTRACE("< %s\n", pdesc) ;
				switch(n) {
					case RSP_OK:
						switch(mrc) {
							case REQ_IDENT:
								if (vlevel>1) {
									VTRACE("\tstd=%d, ver=%d, rev=%d\n", prsp[1], prsp[2], prsp[3]) ;
								}
								break ;
							case REQ_READ:
							case REQ_PREADI1:
							case REQ_PREADI2:
							case REQ_PREADI3:
							case REQ_PREADI4:
							case REQ_PREADI5:
							case REQ_PREADI6:
							case REQ_PREADI7:
							case REQ_PREADI8:
							case REQ_PREADI9:
								nelem = mrc - REQ_READ ;
								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								if (vlevel>1) {
									if (nelem == 0) {
										VTRACE("\tFull Read ") ;
									}
									else {
										VTRACE("\tPartial Read_%d ", nelem) ;
									}

//									count = prsp[1] ;
//									count = (count<<8) + prsp[2] ;
									VTRACE("\tcount=%d\n", count) ;
									if (vlevel > 0)
										DumpHex(&prsp[3], count) ;
//									rc = count ;
								}
								rc = count ;
								break ;
							case REQ_PREAD_DFLT:
								nelem = 0 ;
								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								if (vlevel>1) {
									VTRACE("\tDefault Read ") ;
									VTRACE(" count=%d\n", count) ;
									if (vlevel > 0)
										DumpHex(&prsp[3], count) ;
								}
								rc = count ;
								break ;
							case REQ_PREAD_OFF:
								nelem = 0 ;
								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								if (vlevel>1) {
									VTRACE("\tPartial Read_Offset ") ;
									VTRACE(" count=%d\n", count) ;
									if (vlevel > 0)
										DumpHex(&prsp[3], count) ;
								}
								rc = count ;
								break ;
							case REQ_WRITE:
							case REQ_PWRITEI1:
							case REQ_PWRITEI2:
							case REQ_PWRITEI3:
							case REQ_PWRITEI4:
							case REQ_PWRITEI5:
							case REQ_PWRITEI6:
							case REQ_PWRITEI7:
							case REQ_PWRITEI8:
							case REQ_PWRITEI9:
								if (vlevel>1) {
									nelem = mrc - REQ_WRITE ;
									if (nelem == 0)
										VTRACE("\tFull Write\n") ;
									else
										VTRACE("\tPartial Write_%d\n", nelem) ;
								}
								break ;
							case REQ_PWRITE_OFF:
								if (vlevel>1) {
									VTRACE("\tPartial Write_Offset\n") ;
								}
								break ;
							case REQ_LOGON:
								break ;
							case REQ_SECURITY:
								break ;
							case REQ_LOGOFF:
								break ;
							case REQ_NEGOTIATE:
							case REQ_NEGBR1:
							case REQ_NEGBR2:
							case REQ_NEGBR3:
							case REQ_NEGBR4:
							case REQ_NEGBR5:
							case REQ_NEGBR6:
							case REQ_NEGBR7:
							case REQ_NEGBR8:
							case REQ_NEGBR9:
							case REQ_NEGBR10:
							case REQ_NEGBR11:
								if (vlevel>1) {
									nelem = mrc - REQ_NEGOTIATE ;
									pktsize = prsp[1] ;
									pktsize = (pktsize << 8) + prsp[2] ;
									maxpkt = prsp[2] ;
									baud = prsp[3] ;
									VTRACE("\tNegotiate_%d, pktsize=%d, maxpkt=%d, baud=%d\n", nelem, pktsize, maxpkt, baud) ;
								}
								break ;
							case REQ_WAIT:
								if (vlevel>1) {
								}
								break ;
							case REQ_TERMINATE:
								if (vlevel>1) {
								}
								break ;
						}
						break ;
					case RSPNOK_ERR:
					case RSPNOK_SNS:
					case RSPNOK_ISC:
					case RSPNOK_ONP:
					case RSPNOK_IAR:
					case RSPNOK_BSY:
					case RSPNOK_DNR:
					case RSPNOK_DLK:
					case RSPNOK_RNO:
					case RSPNOK_ISSS:
						if (vlevel>1) {
						}
						break ;
				}
			}
			else {
				VTRACE("< unknown RESP %02X\n", n) ;
			}
		}
	}

	return rc ;
}

void dirunlink(char *pdir, char *pfname)
{
	char tmp[512] ;

	if (pdir) {
		strcpy(&tmp[0], pdir) ;
	}
	else {
		tmp[0] = '\0' ;
	}
	strcat(&tmp[0], pfname) ;

	unlink(&tmp[0]) ;
}

FILE *fdiropen(char *pdir, char *pfname, char *pmode)
{
	char tmp[512] ;
	FILE *fp ;

	if (pdir) {
		strcpy(&tmp[0], pdir) ;
	}
	else {
		tmp[0] = '\0' ;
	}
	strcat(&tmp[0], pfname) ;

	fp = fopen(&tmp[0], pmode) ;

	if (fp == NULL) {
		TRACE("*** Cannot open %s for '%s'\n", tmp, pmode) ;
	}
	return fp ;
}


int savetable(int tableid, unsigned char *pdata, int length)
{
	int rc = 0 ;
	FILE *fp ;
	char tmp[32] ;
	char *pmode ;

	if (tableid == 64) {
		pmode = "ab" ;
	}
	else {
		pmode = "wb" ;
	}

	sprintf(&tmp[0], "tab%03d.dat", tableid) ;
	if ((fp = fdiropen(&outdir[0], &tmp[0], pmode)) != NULL) {
		rc = fwrite(pdata, length, 1, fp) ;
		fclose(fp) ;
	}

	return rc ;
}
