/*
 * xmlgen.c
 *
 *  Created on: Mar 29, 2014
 *      Author: andy
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>

#include "c1219.h"
#include "diag.h"
#include "lputils.h"
#include "xmlgen.h"

extern char retail_customer[] ;
extern char usage_point[] ;

extern char meterinfo_uuid[] ;
extern char timeinfo_uuid[] ;
extern char readinginfo_uuid[] ;
extern char datainfo_uuid[] ;


static const char *intro[] = {
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
		"<?xml-stylesheet type=\"text/xsl\" href=\"GreenButtonDataStyleSheet.xslt\"?>"
};

static const char *feedheader[] = {
		"<feed xmlns=\"http://www.w3.org/2005/Atom\" xmlns:atom=\"http://www.w3.org/2005/Atom\" xmlns:espi=\"http://naesb.org/espi\" xsi:schemaLocation=\"http://naesb.org/espi espiDerived.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">",
        "   <title>Green Button Subscription Feed</title>",
		"   <link rel=\"self\" href=\"/espi/1_1/resource/Subscription/83e269c1\"/>",
		"idtag",
		"updtag"
};

static const char *meterinfo[] = {
		"<entry>",
		"	<link rel=\"self\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s\"/>",
		"	<link rel=\"up\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/ElectricPowerUsageSummary\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/ElectricPowerQualitySummary\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/LocalTimeParameters/01\"/>",
		"	<title>NZERTF Electric Meter</title>",
		"	<content>",
		"		<UsagePoint xmlns=\"http://naesb.org/espi\">",
		"			<ServiceCategory>",
		"				<kind>0</kind>",
		"			</ServiceCategory>",
		"		</UsagePoint>",
		"	</content>",
		"meterinfo_id",
		/*** "idtag", ***/
		"pubtag",
		"updtag",
		"</entry>"
};

static char const *timeinfo[] = {
		"<entry>",
		"	<link rel=\"self\" href=\"/espi/1_1/resource/LocalTimeParameters/01\"/>",
		"	<link rel=\"up\" href=\"/espi/1_1/resource/LocalTimeParameters\"/>",
		"	<title>DST For North America</title>",
		"	<content>",
		"		<LocalTimeParameters xmlns=\"http://naesb.org/espi\">",
		"			<dstEndRule>B40E2000</dstEndRule>",
		"			<dstOffset>3600</dstOffset>",
		"			<dstStartRule>360E2000</dstStartRule>",
		"			<tzOffset>-18000</tzOffset>",
		"		</LocalTimeParameters>",
		"	</content>",
		"timeinfo_id",
		/*** "idtag", ***/
		"pubtag",
		"updtag",
		"</entry>",

};

static char const *meterreadinginfo[] = {
		"<entry>",
		"	<link rel=\"self\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading/01\"/>",
		"	<link rel=\"up\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading/01/IntervalBlock\"/>",
		"	<link rel=\"related\" href=\"/espi/1_1/resource/ReadingType/07\"/>",
		"	<title>One Minute Electricity Consumption</title>",
		"	<content>",
		"		<MeterReading xmlns=\"http://naesb.org/espi\"/>",
		"	</content>",
		"pubtag",
		"updtag",
		"readinginfo_id",
		/*** "idtag", ***/
		"</entry>"
};

static char const *meterdatainfo[] = {
		"<entry>",
		"	<link rel=\"self\" href=\"/espi/1_1/resource/ReadingType/07\"/>",
		"	<link rel=\"up\" href=\"/espi/1_1/resource/ReadingType\"/>",
		"	<title>Type of Meter Reading Data</title>",
		"	<content>",
		"		<ReadingType xmlns=\"http://naesb.org/espi\">",
		"			<accumulationBehaviour>4</accumulationBehaviour>",
		"			<commodity>1</commodity>",
		"			<currency>840</currency>",
		"			<dataQualifier>12</dataQualifier>",
		"			<flowDirection>1</flowDirection>",
		"			<intervalLength>900</intervalLength>",
		"			<kind>12</kind>",
		"			<phase>769</phase>",
		"			<powerOfTenMultiplier>0</powerOfTenMultiplier>",
		"			<timeAttribute>0</timeAttribute>",
		"			<uom>72</uom>",
		"			<interharmonic>",
		"				<numerator>600</numerator>",
		"				<denominator>800</denominator>",
		"			</interharmonic>",
		"			<argument>",
		"				<numerator>1</numerator>",
		"				<denominator>2</denominator>",
		"			</argument>",
		"		</ReadingType>",
		"	</content>",
		"datainfo_id",
		/*** "idtag", ***/
		"pubtag",
		"updtag",
		"</entry>"
} ;

static char const *dataheader[] = {
		"<entry>",
		"idtag",
		"   <link rel=\"self\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading/01/IntervalBlock/173\"/>",
		"	<link rel=\"up\" href=\"/espi/1_1/resource/RetailCustomer/%s/UsagePoint/%s/MeterReading/01/IntervalBlock\"/>",
		"   <title> 1-minute meter Load Profile Data </title>",
		"	<content>",
} ;

static char const *datafooter[] = {
		"   </content>",
		"pubtag",
		"updtag",
		"</entry>"
} ;

static const char *feedfooter[] = {
	"</feed>"
} ;

static void xmlindent(FILE *fp, int indent)
{
	if (fp) {
		while (indent > 0) {
			////putc('\t', fp) ;
			fputs("   ", fp) ;
			--indent ;
		}
	}
}

static int getuuid(unsigned char *puuid)
{
	int rc = 0 ;

	uuid_t uuid ;

	uuid_generate(uuid) ;
	uuid_unparse_upper(uuid, (char *) puuid) ;

	return rc ;
}

static void xmlidtag(FILE *fp)
{
	static unsigned char uuid[64] ;
	getuuid(&uuid[0]) ;
	fprintf(fp, "<id>urn:uuid:%s</id>\n", &uuid[0]) ;
}

static void xmlstaticid(FILE *fp, char *puuid)
{
	fprintf(fp, "   <id>urn:uuid:%s</id>\n", puuid) ;
}

static void do_xml_part(FILE *fp, const char *ppart[], int n, char *tstamp, char *retcust, char *usagep)
{
	const char *p ;
	int i ;

	if (ppart) {
		for (i=0; i < n; i++) {
			p = ppart[i] ;
			if (strcmp(p, "idtag") == 0) {
				xmlidtag(fp) ;
			}
			else if (strcmp(p, "pubtag") == 0) {
				fprintf(fp, "<published>%s</published>\n", tstamp) ;
			}
			else if (strcmp(p, "updtag") == 0) {
				fprintf(fp, "<updated>%s</updated>\n", tstamp) ;
			}
			else if (strcmp(p, "meterinfo_id") == 0) {
				xmlstaticid(fp, meterinfo_uuid) ;
			}
			else if (strcmp(p, "timeinfo_id") == 0) {
				xmlstaticid(fp, timeinfo_uuid) ;
			}
			else if (strcmp(p, "readinginfo_id") == 0) {
				xmlstaticid(fp, readinginfo_uuid) ;
			}
			else if (strcmp(p, "datainfo_id") == 0) {
				xmlstaticid(fp, datainfo_uuid) ;
			}
			else {
				fprintf(fp, p, retcust, usagep) ;
				fprintf(fp, "\n") ;
			}
		}
	}
}


static void xmlintro(FILE *fp, char *tstamp)
{
	if (fp) {

		do_xml_part(fp, intro, sizeof(intro)/sizeof(intro[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, feedheader, sizeof(feedheader)/sizeof(feedheader[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, meterinfo, sizeof(meterinfo)/sizeof(meterinfo[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, timeinfo, sizeof(timeinfo)/sizeof(timeinfo[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, meterreadinginfo, sizeof(meterreadinginfo)/sizeof(meterreadinginfo[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, meterdatainfo, sizeof(meterdatainfo)/sizeof(meterdatainfo[0]), tstamp, retail_customer, usage_point) ;
		do_xml_part(fp, dataheader, sizeof(dataheader)/sizeof(dataheader[0]), tstamp, retail_customer, usage_point) ;
	}
}

static void xmltrailer(FILE *fp, char *tstamp)
{
	if (fp) {
		do_xml_part(fp, datafooter, sizeof(datafooter)/sizeof(datafooter[0]), tstamp, NULL, NULL) ;
		do_xml_part(fp, feedfooter, sizeof(feedfooter)/sizeof(feedfooter[0]), tstamp, NULL, NULL) ;
	}
}

static void xmlibheader(FILE *fp)
{
	if (fp) {
		fprintf(fp, "    <IntervalBlock xmlns=\"http://naesb.org/espi\">\n") ;
	}
}

static void xmlibfooter(FILE *fp)
{
	if (fp) {
		fprintf(fp, "    </IntervalBlock>\n") ;
	}
}

void xmlinterval(FILE *fp, unsigned int start, unsigned int duration)
{
	int indent = 3 ;
	char sdate[32] = "1/6/2014 8:00:00 AM" ;
	int i ;
	struct tm tm ;
	time_t tmp = (time_t) start ;

	if (fp) {
		gmtime_r(&tmp, &tm ) ;
		asctime_r(&tm, &sdate[0]) ;
		i = strlen(&sdate[0]) ;
		while (sdate[--i] < ' ')
			sdate[i] = '\0' ;


		xmlindent(fp, indent) ; ++indent ;
		fprintf(fp, "<interval>\n") ;
		xmlindent(fp, indent) ;
		fprintf(fp, "<duration>%d</duration>\n", duration) ;
		xmlindent(fp, indent) ;
		fprintf(fp, "<start>%u</start>\n", start) ;
		xmlindent(fp, indent) ; --indent ;
		fprintf(fp, "<!-- start : %s GMT -->\n", sdate) ;
		xmlindent(fp, indent) ;
		fprintf(fp, "</interval>\n") ;
	}
}

void xmlireading(FILE *fp, unsigned int start, unsigned int duration, float *pval, int nval)
{
	int indent = 3 ;
	int i ;
	struct tm tmstart ;

	if (fp) {

		gmtime_r((time_t *) &start, &tmstart) ;

		xmlindent(fp, indent) ; ++indent ;
		fprintf(fp, "<IntervalReading> ") ;
		fprintf(fp, "<!-- %02d/%02d/%04d %02d:%02d:%02d %s -->\n",
			tmstart.tm_mon+1, tmstart.tm_mday, tmstart.tm_year+1900,
			tmstart.tm_hour, tmstart.tm_min, tmstart.tm_sec, tmstart.tm_zone) ;
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
/**** ??
		for ( ; i < 2; i++) {
			xmlindent(fp, indent) ;
			fprintf(fp, "<value>0</value>\n") ;
		}
****/
		--indent ;
		xmlindent(fp, indent) ;
		fprintf(fp, "</IntervalReading>\n") ;
	}
}


FILE *xmlopen(char *pdir, char *xmlfname)
{
	////int indent = 2 ;
	char sfname[256] ;
	struct tm tm ;
	time_t t ;

	time(&t) ;
	gmtime_r(&t, &tm);

	sprintf(&sfname[0], "lpib%d%02d%02d%02d%02d%02d.xml",
		tm.tm_year+1900 , tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec) ;
	strcpy(xmlfname, sfname) ;

	return(fdiropen(pdir, &sfname[0], "w")) ;
}

void xmlclose(FILE *fp)
{
	if (fp)
		fclose(fp) ;
}

void xmlgen(FILE *fp, int nintervals)
{
	extern int lp_channels ;
	meterinfo_t mi ;
	readlp_info_t rlpi ;
	int i ;
	float val[16] ;	// support 16 channels max
	unsigned int start, duration, istart, idur, icnt ;

	int mivduration ;	// meter interval duration, in seconds
	int nipermi ;		// number of 60-sec intervals per meter interval

	char tstamp[64] = "2014-01-13T08:00.00Z" ;
	struct tm tm ;
	time_t t ;

	time(&t) ;
	gmtime_r(&t, &tm);

	sprintf(&tstamp[0], "%d-%02d-%02dT%02d:%02d:%02d.%02dZ",
		tm.tm_year+1900 , tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,0) ;


	idur = 60 ;
	////idur = 60*15 ;

	lp_getmeterinfo(0, &mi, &rlpi) ;
	start = lp_getstart(0) ;
	duration = lp_getduration(0) ;
	mivduration = mi.intvlen * 60 ;
	nipermi = mivduration / idur ;

	xmlintro(fp, tstamp) ;

	xmlibheader(fp);

	xmlinterval(fp, start, duration) ;

	istart = start ;
	for (icnt=0; icnt < nintervals; icnt++) {
		lp_getivdata(0, icnt, &val[0], lp_channels /*mi.nchnl*/) ;
		for (i=0; i<nipermi; i++) {
			xmlireading(fp, istart, idur, &val[0], lp_channels /*mi.nchnl*/) ;
			istart += idur ;
		}
	}

	xmlibfooter(fp) ;

	xmltrailer(fp, tstamp) ;
}

