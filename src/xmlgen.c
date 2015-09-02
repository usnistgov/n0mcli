/*
 * xmlgen.c
 *
 *  Created on: Mar 29, 2014
 *      Author: andy
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <rpc.h>

#include "c1219.h"
#include "diag.h"
#include "lputils.h"
#include "xmlgen.h"

/***
<?xml version="1.0" encoding="UTF-8"?>
<entry xmlns:espi="http://naesb.org/espi" xmlns="http://www.w3.org/2005/Atom">
	<id>urn:uuid:F1BCCA31-5AD3-44AC-8B24-25FD2CCACD32</id>
	<link rel="self" href="https://services.greenbuttondata.org/DataCustodian/espi/1_1/resource/RetailCustomer/9B6C7088/UsagePoint/5446AF3F/MeterReading/01/IntervalBlock/173"/>
	<link rel="up" href="https://services.greenbuttondata.org/DataCustodian/espi/1_1/resource/RetailCustomer/9B6C7088/UsagePoint/5446AF3F/MeterReading/01/IntervalBlock"/>
	<title/>
	<content>
	  <IntervalBlock xmlns="http://naesb.org/espi">
		<interval>
		  <duration>604800</duration>
		  <start>1388995200</start>
		  <!-- start date: 1/6/2014 8:00:00 AM -->
		</interval>
		<IntervalReading>
		  <timePeriod>
			 <duration>60</duration>
			 <start>1388995200</start>
		  </timePeriod>
		  <value>295</value>
		</IntervalReading>
	...
		<IntervalReading>
		  <timePeriod>
			<duration>60</duration>
			<start>1389599940</start>
		  </timePeriod>
		  <value>380</value>
		</IntervalReading>
	  </IntervalBlock>
	</content>
	<published>2014-01-13T08:00:00Z</published>
	<updated>2014-01-13T08:00:00Z</updated>
</entry>

 */

static const char *intro = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	"<entry xmlns:espi=\"http://naesb.org/espi\" xmlns=\"http://www.w3.org/2005/Atom\">";

static const char *links = "<link rel=\"self\" href=\""
		"https://services.greenbuttondata.org/DataCustodian/espi/1_1/"
		"resource/RetailCustomer/9B6C7088/UsagePoint/5446AF3F/MeterReading"
		"/01/IntervalBlock/173\"/>\n\t"
		"<link rel=\"up\" href=\"https://services.greenbuttondata."
		"org/DataCustodian/espi/1_1/resource/RetailCustomer/9B6C7088/UsagePoint/"
		"5446AF3F/MeterReading/01/IntervalBlock\"/>" ;

static const char *titcontag = "<title/>\n\t<content>" ;

static const char *intvblock = "<IntervalBlock xmlns=\"http://naesb.org/espi\">" ;

void xmlindent(FILE *fp, int indent)
{
	if (fp) {
		while (indent > 0) {
			putc('\t', fp) ;
			--indent ;
		}
	}
}

int getuuid(unsigned char *puuid)
{
	int rc = 0 ;
	UUID __RPC_FAR uuid ;

	if (puuid) {
		*puuid = '\0' ;
		UuidCreate(&uuid) ;
		UuidToString(&uuid, &puuid) ;
		rc = strlen(puuid) ;

		VVTRACE("uuid=%s\n", puuid) ;
	}

	return rc ;
}

void xmlidtag(FILE *fp)
{
	static unsigned char uuid[64] ;
	getuuid(&uuid[0]) ;
	fprintf(fp, "<id>urn:uuid:%s</id>\n", &uuid[0]) ;
}

void xmlintro(FILE *fp)
{
	if (fp) {
		fprintf(fp, "%s\n", intro) ;

		xmlindent(fp, 1) ;
		xmlidtag(fp) ;

		xmlindent(fp, 1) ;
		fprintf(fp, "%s\n", links) ;

		xmlindent(fp, 1) ;
		fprintf(fp, "%s\n", titcontag) ;

		xmlindent(fp, 2) ;
		fprintf(fp, "%s\n", intvblock) ;
	}
}

void xmlinterval(FILE *fp, unsigned int start, unsigned int duration)
{
	int indent = 3 ;
	char sdate[32] = "1/6/2014 8:00:00 AM" ;
	int i ;

	if (fp) {
		ctime_r((time_t *) &start, &sdate[0]);
		i = strlen(&sdate[0]) ;
		while (sdate[--i] < ' ')
			sdate[i] = '\0' ;


		xmlindent(fp, indent) ; ++indent ;
		fprintf(fp, "<interval>\n") ;
		xmlindent(fp, indent) ;
		fprintf(fp, "<duration>%d</duration>\n", duration) ;
		xmlindent(fp, indent) ;
		fprintf(fp, "<start>%d</start>\n", start) ;
		xmlindent(fp, indent) ; --indent ;
		fprintf(fp, "<!-- start date: %s -->\n", sdate) ;
		xmlindent(fp, indent) ;
		fprintf(fp, "</interval>\n") ;
	}
}

void xmlireading(FILE *fp, unsigned int start, unsigned int duration, float *pval, int nval)
{
	int indent = 3 ;
	int i ;

	if (fp) {
		xmlindent(fp, indent) ; ++indent ;
		fprintf(fp, "<IntervalReading>\n") ;
		xmlindent(fp, indent) ; ++indent ;
		fprintf(fp, "<timePeriod>\n") ;
		xmlindent(fp, indent) ;
		fprintf(fp, "<duration>%d</duration>\n", duration) ;
		xmlindent(fp, indent) ; --indent ;
		fprintf(fp, "<start>%d</start>\n", start) ;
		xmlindent(fp, indent) ;
		fprintf(fp, "</timePeriod>\n") ;

		for (i=0; i < nval; i++) {
			xmlindent(fp, indent) ;
			fprintf(fp, "<value>%.0f</value>\n", pval[i]) ;
		}
		for ( ; i < 2; i++) {
			xmlindent(fp, indent) ;
			fprintf(fp, "<value>0</value>\n") ;
		}

		--indent ;
		xmlindent(fp, indent) ;
		fprintf(fp, "</IntervalReading>\n") ;
	}
}

void xmlfooter(FILE *fp)
{
	int indent = 2 ;
	char sdate[64] = "2014-01-13T08:00:00Z" ;
	struct tm tm ;
	time_t t ;

	time(&t) ;
	gmtime_r(&t, &tm);

	sprintf(&sdate[0], "%d-%02d-%02dT%02d:%02d:%02d:%02dZ",
		tm.tm_year+1900 , tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) ;

	xmlindent(fp, indent) ; --indent ;
	fprintf(fp, "</IntervalBlock>\n") ;
	xmlindent(fp, indent) ;
	fprintf(fp, "</content>\n") ;
	xmlindent(fp, indent) ;
	fprintf(fp, "<published>%s</published>\n", sdate) ;
	xmlindent(fp, indent) ;
	fprintf(fp, "<updated>%s</updated>\n", sdate) ;
	fprintf(fp, "/entry\n") ;
}

FILE *xmlopen(char *pdir)
{
	int indent = 2 ;
	char sfname[256] ;
	struct tm tm ;
	time_t t ;

	time(&t) ;
	gmtime_r(&t, &tm);

	sprintf(&sfname[0], "lpib%d%02d%02d%02d%02d%02d.xml",
		tm.tm_year+1900 , tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) ;
	return(fdiropen(pdir, &sfname[0], "w")) ;
}

void xmlclose(FILE *fp)
{
	if (fp)
		fclose(fp) ;
}

void xmlgen(FILE *fp)
{
	meterinfo_t mi ;
	readlp_info_t rlpi ;
	int i ;
	float val[16] ;	// support 16 channels max
	unsigned int start, duration, istart, idur, icnt ;

	int mivduration ;	// meter interval duration, in seconds
	int nipermi ;		// number of 60-sec intervals per meter interval

	idur = 60 ;
	////idur = 60*15 ;

	lp_getmeterinfo(0, &mi, &rlpi) ;
	start = lp_getstart(0) ;
	duration = lp_getduration(0) ;
	mivduration = mi.intvlen * 60 ;
	nipermi = mivduration / idur ;

	xmlintro(fp) ;
	xmlinterval(fp, start, duration) ;

	istart = start ;
	for (icnt=0; icnt<rlpi.intvcount; icnt++) {
		lp_getivdata(0, icnt, &val[0], mi.nchnl) ;
		for (i=0; i<nipermi; i++) {
			xmlireading(fp, istart, idur, &val[0], mi.nchnl) ;
			istart += idur ;
		}
	}

	xmlfooter(fp) ;
}
