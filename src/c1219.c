/*
 * c1819.c
 *
 *  Created on: Mar 22, 2014
 *      Author: andy
 */
#include <stdio.h>
#include <string.h>
#include "customtypes.h"
#include "c1219.h"
#include "diag.h"

static lookup_t reqtab[] = {
 { REQ_IDENT 		, "Identification request" },
 { REQ_READ 		, "Full Read request <tableid>" },
 { REQ_PREADI1 	, "Partial Read 1 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI2 	, "Partial Read 2 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI3 	, "Partial Read 3 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI4 	, "Partial Read 4 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI5 	, "Partial Read 5 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI6 	, "Partial Read 6 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI7 	, "Partial Read 7 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI8 	, "Partial Read 8 idx request <tableid> <index>+ <count>" },
 { REQ_PREADI9 	, "Partial Read 9 idx request <tableid> <index>+ <count>" },
 { REQ_PREAD_DFLT 	, "Transfer Default Table" },
 { REQ_PREAD_OFF 	, "Partial Read/Offset request <tableid> <offset> <count>" },
 { REQ_WRITE 		, "Full Write request" },
 { REQ_PWRITEI1 	, "Partial Write 1 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI2 	, "Partial Write 2 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI3 	, "Partial Write 3 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI4 	, "Partial Write 4 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI5 	, "Partial Write 5 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI6 	, "Partial Write 6 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI7 	, "Partial Write 7 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI8 	, "Partial Write 8 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITEI9 	, "Partial Write 9 idx request <tableid> <index>+ <count>" },
 { REQ_PWRITE_OFF 	, "Partial Write/Offset request <tableid> <offset> <count>" },
 { REQ_LOGON 		, "Logon request" },
 { REQ_SECURITY 	, "Security request" },
 { REQ_LOGOFF 		, "Logoff request" },
 { REQ_NEGOTIATE 	, "Negotiate. No baudrate included/stay on default baudrate" },
 { REQ_NEGBR1 		, "Negotiate/ 1 <baud rate> included in request" },
 { REQ_NEGBR2 		, "Negotiate/ 2 <baud rate> included in request" },
 { REQ_NEGBR3 		, "Negotiate/ 3 <baud rate> included in request" },
 { REQ_NEGBR4 		, "Negotiate/ 4 <baud rate> included in request" },
 { REQ_NEGBR5 		, "Negotiate/ 5 <baud rate> included in request" },
 { REQ_NEGBR6 		, "Negotiate/ 6 <baud rate> included in request" },
 { REQ_NEGBR7 		, "Negotiate/ 7 <baud rate> included in request" },
 { REQ_NEGBR8 		, "Negotiate/ 8 <baud rate> included in request" },
 { REQ_NEGBR9 		, "Negotiate/ 9 <baud rate> included in request" },
 { REQ_NEGBR10		, "Negotiate/ 10 <baud rate> included in request" },
 { REQ_NEGBR11		, "Negotiate/ 11 <baud rate> included in request" },
 { REQ_WAIT 		, "Wait request" },
 { REQ_TERMINATE 	, "Terminate request" },
 { -1  				, "unknown" }
};

static lookup_t rsptab[] = {
 { RSP_OK		, "Acknowledge - No problems, request accepted" },
 { RSPNOK_ERR	, "Error - rejection of the received service request" },
 { RSPNOK_SNS	, "Service Not Supported" },
 { RSPNOK_ISC	, "Insufficient Security Clearance" },
 { RSPNOK_ONP	, "Operation Not Possible" },
 { RSPNOK_IAR	, "Inappropriate Action Requested" },
 { RSPNOK_BSY	, "Device Busy" },
 { RSPNOK_DNR	, "Data Not Ready" },
 { RSPNOK_DLK	, "Data Locked" },
 { RSPNOK_RNO	, "Renegotiate request" },
 { RSPNOK_ISSS	, "Invalid Service Sequence State" },
 { -1  				, "unknown" }
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

int parse_request(unsigned char *preq, int len)
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
				switch(n) {
					case REQ_IDENT:
						TRACE("Request: %s\n", pdesc) ;
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
						TRACE("Request: %s\n", pdesc) ;
						tableid = preq[1] ;
						tableid = (tableid<<8) + preq[2] ;
						for (i=0; i < nelem; i++) {
							index[i] = preq[3+i*2] ;
							index[i] = (index[i]<<8) + preq[3+ i*2 + 1] ;
							if (i == 0)
								TRACE("Index=") ;
							TRACE("%d ", index[i]) ;
						}
						if (nelem > 0) {
							count = preq[3+nelem*2] ;
							count = (count<<8) + preq[3+nelem*2 + 1] ;
							TRACE("Tableid=%d Count=%d\n", tableid, count) ;
						}
						else {
							TRACE("Tableid=%d\n", tableid) ;
						}
						break ;
					case REQ_PREAD_DFLT:
						nelem = 0 ;
						TRACE("Request: %s\n", pdesc) ;
						break ;
					case REQ_PREAD_OFF:
						nelem = 0 ;
						TRACE("Request: %s\n", pdesc) ;
						tableid = preq[1] ;
						tableid = (tableid<<8) + preq[2] ;
						offset = preq[3] ;
						offset = (offset<<8) + preq[4] ;
						offset = (offset<<8) + preq[5] ;
						TRACE("\tTableID=%d, Offset=%d\n", tableid, offset) ;
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
						TRACE("Request: %s\n", pdesc) ;
						tableid = preq[1] ;
						tableid = (tableid<<8) + preq[2] ;
						for (i=0; i < nelem; i++) {
							index[i] = preq[3+i*2] ;
							index[i] = (index[i]<<8) + preq[3+ i*2 + 1] ;
							if (i == 0)
								TRACE("Index=") ;
							TRACE("%d ", index[i]) ;
						}
						count = preq[3+nelem*2] ;
						count = (count<<8) + preq[3+nelem*2 + 1] ;
						TRACE("Count=%d\n", count) ;
						DumpHex(&preq[3+nelem*2 + 2], count) ;
						break ;
					case REQ_PWRITE_OFF:
						nelem = 0 ;
						TRACE("Request: %s\n", pdesc) ;
						tableid = preq[1] ;
						tableid = (tableid<<8) + preq[2] ;
						offset = preq[3] ;
						offset = (offset<<8) + preq[4] ;
						offset = (offset<<8) + preq[5] ;
						count = preq[6] ;
						count = (count<<8) + preq[7] ;
						TRACE("\tTableID=%d, Offset=%d Count=%d\n", tableid, offset, count) ;
						DumpHex(&preq[8], count) ;
						break ;
					case REQ_LOGON:
						TRACE("Request: %s\n", pdesc) ;
						userid = preq[1] ;
						userid = (userid<<8) + preq[2] ;
						memcpy(&username[0], &preq[3], 10) ;
						username[10] = '\0' ;
						TRACE("\tUserID=%d, UserName=%s\n", userid, username) ;
						break ;
					case REQ_SECURITY:
						TRACE("Request: %s\n", pdesc) ;
						memcpy(&passwd[0], &preq[1], 20) ;
						passwd[20] = '\0' ;
						TRACE("\tPasswd=%s\n", passwd) ;
						break ;
					case REQ_LOGOFF:
						TRACE("Request: %s\n", pdesc) ;
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
						nelem = n - REQ_NEGOTIATE ;
						TRACE("Request: %s\n", pdesc) ;
						pktsize = preq[1] ;
						pktsize = (pktsize << 8) + preq[2] ;
						maxpkt = preq[2] ;
						baud = preq[3] ;
						TRACE("\tpktsize=%d, maxpkt=%d, baud=%d\n", pktsize, maxpkt, baud) ;
						break ;
					case REQ_WAIT:
						TRACE("Request: %s\n", pdesc) ;
						TRACE("\tSeconds=%d\n", preq[1]) ;
						break ;
					case REQ_TERMINATE:
						TRACE("Request: %s\n", pdesc) ;
						break ;
				}
			}
		}
		else
			pdesc = &wrongcode[0] ;
	}

	return rc ;
}

int parse_response(unsigned char *prsp, int len, int mrc) // mrc -- matching request code(zero means N/A)
{
	int rc = 0 ;
	int n ;
	int nelem ;
	unsigned char *pdesc ;
	unsigned char *pdmrc ;
	int i ;
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
					pdmrc = "Unmatched request" ;

				VTRACE("Response to: %s -- ", pdmrc) ;

				switch(n) {
					case RSP_OK:
						switch(mrc) {
							case REQ_IDENT:
								VTRACE("IDENT\n\tstd=%d, ver=%d, rev=%d\n", prsp[1], prsp[2], prsp[3]) ;
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
								if (nelem == 0)
									VTRACE("Full Read\n") ;
								else
									VTRACE("Partial Read_%d\n", nelem) ;

								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								VTRACE("\tcount=%d\n", count) ;
								if (verbose)
									DumpHex(&prsp[3], count) ;
								rc = count ;
								break ;
							case REQ_PREAD_DFLT:
								nelem = 0 ;
								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								VTRACE("Default Read\n") ;
								VTRACE("\tcount=%d\n", count) ;
								if (verbose)
									DumpHex(&prsp[3], count) ;
								rc = count ;
								break ;
							case REQ_PREAD_OFF:
								nelem = 0 ;
								count = prsp[1] ;
								count = (count<<8) + prsp[2] ;
								VTRACE("Partial Read_Offset\n") ;
								VTRACE("\tcount=%d\n", count) ;
								if (verbose)
									DumpHex(&prsp[3], count) ;
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
								nelem = mrc - REQ_WRITE ;
								if (nelem == 0)
									TRACE("Full Write\n") ;
								else
									TRACE("Partial Write_%d\n", nelem) ;

								break ;
							case REQ_PWRITE_OFF:
								TRACE("Partial Write_Offset\n") ;
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
								nelem = mrc - REQ_NEGOTIATE ;
								TRACE("Negotiate_%d\n", nelem) ;
								pktsize = prsp[1] ;
								pktsize = (pktsize << 8) + prsp[2] ;
								maxpkt = prsp[2] ;
								baud = prsp[3] ;
								TRACE("\tpktsize=%d, maxpkt=%d, baud=%d\n", pktsize, maxpkt, baud) ;
								break ;
							case REQ_WAIT:
								TRACE("Wait\n") ;
								break ;
							case REQ_TERMINATE:
								TRACE("Terminate\n") ;
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
						TRACE("NOK Response: %s\n", pdesc) ;
						break ;
				}
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
